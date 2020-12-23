from fusepy._fusepy import _DataReader, _DataWriter
from fusepy.objects import PackedObjects, PackedObject, UnresolvedReference, ObjectSection

class DataReader:
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
    return UnresolvedReference(int(as_dict['offset']), as_dict['path'], as_dict['layout'])

def _make_PackedObject(as_dict):
    sections = []
    for section in as_dict['sections']:
        sections.append(_make_ObjectSection(section))        
    unresolved = []
    for ref in as_dict['unresolved']:
        unresolved.append(_make_UnresolvedReference(ref))
    return PackedObject(as_dict['path'], int(as_dict['offset']), int(as_dict['size']),
                        int(as_dict['actual_size']), sections, unresolved)

def _make_PackedObjects(as_dict):
    objects = []
    for object in as_dict['objects']:
        objects.append(_make_PackedObject(object))
    return PackedObjects(as_dict['binary'], objects)

class DataWriter:
    def __init__(self):
        self._writer = _DataWriter()
    
    def assemble(self):
        as_dict = self._writer.assemble()
        return _make_PackedObjects(as_dict)