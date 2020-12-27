#from fusepy._fusepy import 
class UnresolvedReference:
    def __init__(self, offset, path, layout):
        self.offset = offset
        self.path = path
        self.layout = layout

    def __repr__(self):
        return f'UnresolvedReference({self.offset}, {self.path}, {self.layout})'

    def to_dict(self):
        return {
            'offset': self.offset,
            'path': self.path,
            'layout': self.layout.to_dict()
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

class PackedObject:
    def __init__(self, path, offset, size, actual_size, sections, unresolved):
        self.path = path
        self.offset = offset
        self.size = size
        self.actual_size = actual_size
        self.sections = sections
        self.unresolved = unresolved

    def to_dict(self):
        sections = []
        for section in self.sections:
            sections.append(section.to_dict())
        unresolved = []
        for ref in self.unresolved:
            unresolved.append(ref.to_dict())
        return {
            'path': self.path,
            'offset': self.offset,
            'size': self.size,
            'actual_size': self.actual_size,
            'sections': sections,
            'unresolved': unresolved,
        }

def _numerus(count, singular, plural):
    if count == 1:
        return singular
    else:
        return plural

class PackedObjects:
    def __init__(self, binary, objects):
        self.binary = binary
        self.objects = objects

    def __str__(self):
        return f'{len(self.objects)} {_numerus(len(self.objects), "object", "objects")} packed into {len(self.binary)} bytes'

    def to_dict(self):
        objects = []
        for obj in self.objects:
            objects.append(obj.to_dict())
        return {
            'binary': self.binary,
            'objects': objects,
        }