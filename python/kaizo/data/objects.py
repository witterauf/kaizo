from pathlib import Path
import json
import base64
from kaizo import Address, Endianness, Signedness
import pprint

class UnresolvedReference:
    def __init__(self, offset, path, layout):
        self.offset = offset
        self.actual_offset = None
        self.path = path
        self.layout = layout

    def resolve(self, address):
        return ResolvedReference(self, address)

    def __repr__(self):
        return f'UnresolvedReference({self.offset}, {self.actual_offset}, {self.path}, {self.layout})'

    def to_dict(self):
        return {
            'offset': self.offset,
            'actual_offset': self.actual_offset,
            'path': self.path,
            'layout': self.layout.to_dict()
        }

class ResolvedReference:
    def __init__(self, unresolved, address):
        self.offset = unresolved.offset
        self.actual_offset = unresolved.actual_offset
        self.path = unresolved.path
        self.layout = unresolved.layout
        self.address = address

    def __repr__(self):
        return f'ResolvedReference({self.offset}, {self.actual_offset}, {self.path}, {self.layout}, {self.address})'

    def to_dict(self):
        return {
            'offset': self.offset,
            'actual_offset': self.actual_offset,
            'path': self.path,
            'layout': self.layout.to_dict(),
            'address': str(self.address)
        }

class ObjectSection:
    def __init__(self, offset, actual_offset, size):
        self.offset = offset
        self.actual_offset = actual_offset
        self.size = size

    def __repr__(self):
        return f'ObjectSection({self.offset}, {self.actual_offset}, {self.size})'

    def to_dict(self):
        return {
            'offset': self.offset,
            'actual_offset': self.actual_offset,
            'size': self.size,
        }

class Constraint:
    pass

class FixedAddressConstraint(Constraint):
    def __init__(self, address):
        self.address = address

    def to_dict(self):
        return {
            'kind': 'fixed',
            'address': str(self.address),
        }

class AddressRangeConstraint(Constraint):
    def __init__(self, lower, upper):
        self.lower = lower
        self.upper = upper

    def to_dict(self):
        return {
            'kind': 'range',
            'lower': str(self.lower),
            'upper': str(self.upper),
        }

class BinaryObject:
    def __init__(self, path, binary, actual_size, sections, unresolved, *,
                 fixed_offset=None, fixed_address=None, alignment=1):
        self.path = path
        self.binary = binary
        self.actual_size = actual_size
        self.sections = sections
        self.resolved = []
        self.alignment = alignment
        self.link_offset = None
        self.link_address = None
        self.constraints = []

        if fixed_offset and fixed_address:
            raise ValueError('can only specify either fixed address or fixed offset')
        elif fixed_offset:
            self.link_offset = fixed_offset
        elif fixed_address:
            self.constraints.append(FixedAddressConstraint(fixed_address))

        self.unresolved = unresolved
        self._compute_actual_offsets()

    def _compute_actual_offsets(self):
        if not self.unresolved:
            return
        self.unresolved = sorted(self.unresolved, key=lambda ref: ref.offset)
        i = 0
        for section in self.sections:
            end_offset = section.offset + section.size
            ref = self.unresolved[i]
            while ref.offset < end_offset:
                ref.actual_offset = ref.offset - section.offset + section.actual_offset
                i += 1
                if i >= len(self.unresolved):
                    return
                ref = self.unresolved[i]

    @property
    def packed_size(self):
        return len(self.binary)

    def resolve_references(self, resolver):
        for ref in self.unresolved:
            if ref.path not in resolver:
                raise ValueError(f'unresolved reference "{ref.path}"')
            address = resolver[ref.path]
            self.resolved.append(ref.resolve(address))
        self.unresolved = []

    def apply(self, target):
        if self.unresolved:
            raise ValueError(f'there are {len(self.unresolved)} unresolved references left')
        if self.resolved:
            self._apply_references(target)
        mem = memoryview(self.binary)
        for section in self.sections:
            target.write(mem[section.offset:section.offset+section.size],
                         self.link_offset + section.actual_offset)

    def _apply_references(self, target):
        for ref in self.resolved:
            print(ref.path)
            patches = ref.layout.encode(ref.address)
            for patch in patches:                
                start = patch.effective_offset(ref.actual_offset)
                start = self.packed_offset(start)

                if patch.is_partial:
                    # Read in original data before applying patch
                    offset = patch.effective_offset(self.link_offset +
                                                    ref.actual_offset)
                    size = len(patch)
                    original = target.read(offset, size)
                    end = start + size
                    self.binary[start:end] = original

                patch.set_relative_offset(0)
                patch.apply(self.binary, start)

    def packed_offset(self, offset):
        for section in self.sections:
            if section.actual_offset <= offset < section.actual_offset + section.size:
                return section.offset + (offset - section.actual_offset)
        print(self.sections)
        raise ValueError(f'offset {hex(offset)} is not part of the object')

    def to_dict(self):
        sections = [section.to_dict() for section in self.sections]
        unresolved = [ref.to_dict() for ref in self.unresolved]
        resolved = [ref.to_dict() for ref in self.resolved]
        constraints = [constraint.to_dict() for constraint in self.constraints]

        as_dict = {
            'path': self.path,
            'binary': self.binary,
        }
        as_dict['actual_size'] = self.actual_size
        if self.link_offset is not None:
            as_dict['link_offset'] = self.link_offset
        if self.link_address is not None:
            as_dict['link_address'] = str(self.link_address)
        if constraints:
            as_dict['constraints'] = constraints
        if len(sections) > 1:
            as_dict['sections'] = sections
        if unresolved:
            as_dict['unresolved'] = unresolved
        if resolved:
            as_dict['resolved'] = resolved

        return as_dict

def _json_serializer(data):
    if isinstance(data, Address):
        return str(data)
    elif isinstance(data, Endianness):
        if data == Endianness.LITTLE:
            return 'little'
        else:
            return 'big'
    elif isinstance(data, Signedness):
        if data == Signedness.UNSIGNED:
            return 'unsigned'
        else:
            return 'signed'
    else:
        type_name = data.__class__.__name__
        raise TypeError(f"Object of type {type_name} is not serializable")        

def _save_objects_json(filepath, objects, config):
    config = config if config is not None else {}
    inline = config.get('inline', False)
    bin_file = config.get('bin_file', None)

    if not inline and bin_file is None:
        bin_file = filepath.with_suffix('.bin')

    if bin_file is not None:
        offsets = []
        offset = 0
        with open(bin_file, 'wb') as f:
            for o in objects:
                f.write(o.binary)
                offsets.append(offset)
                offset += o.packed_size

    object_dicts = []
    for i, o in enumerate(objects):
        as_dict = o.to_dict()
        if inline:
            as_dict['binary'] = base64.b64encode(as_dict['binary']).decode('utf-8')
        else:
            del as_dict['binary']
            as_dict['bin_offset'] = offsets[i]
        object_dicts.append(as_dict)
    
    with open(filepath, 'w', encoding='utf-8') as f:
        if inline:
            json.dump({
                'objects': object_dicts
                }, f, indent=2, default=_json_serializer)
        else:
            json.dump({
                'binary': f'{bin_file.name}',
                'objects': object_dicts
                }, f, indent=2, default=_json_serializer)

def save_objects(filename, objects, config=None):
    filepath = Path(filename)
    if filepath.suffix == '.json':
        _save_objects_json(filepath, objects, config)
    else:
        raise ValueError(f'unsupported binary object format "{filename.suffix}"')
