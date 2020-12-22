from kaizopy.text.table import *
from kaizopy.text.charactergrid import CharacterGrid
from kaizopy._kaizopy import _TableEncoding
from fusepy import ExtensionTextEncoding

class TableEncoding(ExtensionTextEncoding):
    def __init__(self, table, missing_decoder=None):
        encoding = _TableEncoding(table._table, missing_decoder)
        super().__init__(encoding)
