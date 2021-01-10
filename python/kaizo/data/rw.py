from kaizo.kaizopy import _DataReader, _DataWriter
from kaizo.data.objects import BinaryObject, UnresolvedReference, ObjectSection
from kaizo.addresses import AddressLayout

class DataReader:
    """
    Reads binary data into a structured representation according to a given DataFormat.
    """

    def __init__(self, binary):
        self._reader = _DataReader(binary)

    def set_offset(self, offset):
        self._reader.set_offset(offset)

    def set_address_map(self, map):
        self._reader.set_address_map(map._map)

def _make_ObjectSection(as_dict):
    return ObjectSection(int(as_dict['offset']), int(as_dict['actual_offset']),
                         int(as_dict['size']))

def _make_UnresolvedReference(as_dict):
    layout = AddressLayout._make(as_dict['layout'])
    return UnresolvedReference(int(as_dict['offset']), as_dict['path'], layout)

def _make_object(o):
    sections = []
    for section in o['sections']:
        sections.append(_make_ObjectSection(section))        
    unresolved = []
    for ref in o['unresolved']:
        unresolved.append(_make_UnresolvedReference(ref))

    fixed_offset = None
    if 'fixed_offset' in o:
        fixed_offset = int(o['fixed_offset'])
    alignment = 1
    if 'alignment' in o:
        alignment = int(o['alignment'])
    return BinaryObject(o['path'], o['binary'], int(o['actual_size']), sections,
                        unresolved, fixed_offset=fixed_offset, alignment=alignment)

def _make_objects(object_dicts):
    return [_make_object(o) for o in object_dicts]

class DataWriter:
    def __init__(self):
        self._writer = _DataWriter()
    
    def assemble(self):
        object_dicts = self._writer.assemble()
        return _make_objects(object_dicts)
