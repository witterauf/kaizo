from kaizopy.text.table import *
from kaizopy.text.charactergrid import CharacterGrid
from kaizopy._kaizopy import _TableEncoding
from fusepy import ExtensionTextEncoding

class Chunk:
    def __init__(self, text, binary, entry, arguments=None):
        self.text = str(text)
        self.binary = binary
        self.entry = entry
        self.arguments = arguments

    def __repr__(self):
        if self.arguments:
            return f'Chunk("{self.binary}, {self.entry}, {self.arguments})'
        else:
            return f'Chunk("{self.binary}, {self.entry})'

class TableEncoding(ExtensionTextEncoding):
    def __init__(self, table, missing_decoder=None):
        encoding = _TableEncoding(table._table, missing_decoder)
        super().__init__(encoding)

    def chunks(self, text):
        _chunks = self._encoding.chunks(text)
        return [Chunk(_chunk[0], _chunk[1], TableEntry._make(_chunk[2]),
                      None if len(_chunk) == 3 else _chunk[3]) for _chunk in _chunks]
