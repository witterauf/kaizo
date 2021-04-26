from kaizo.kaizopy import _Table, _TableEncoding
from kaizo.text.encoding import ExtensionTextEncoding
from enum import Enum

class TableEntryKind(Enum):
    TEXT = 0
    CONTROL = 1
    TABLE_SWITCH = 2
    END = 3
    HOOK = 4

def _to_hex(binary):
    return "".join(f'{byte:0>2X}' for byte in binary)

def _escape_postfix(postfix):
    postfix = postfix.replace('\n', '\\n')
    postfix = postfix.replace('\r', '')
    postfix = postfix.replace('\t', '\\t')
    return postfix

class TableEntry:
    @staticmethod
    def _make(entry):
        if entry[0] == 0:
            return TableTextEntry(entry[1])
        elif entry[0] == 1:
            return TableControlEntry(entry[1], entry[2], entry[3])
        elif entry[0] == 2:
            return TableSwitchEntry(entry[1])
        elif entry[0] == 3:
            return TableEndEntry(entry[1], entry[2])
        elif entry[0] == 4:
            return TableHookEntry(entry[1])
        else:
            raise ValueError()

    @property
    def is_text(self):
        return self.kind == TableEntryKind.TEXT

    @property
    def is_end(self):
        return self.kind == TableEntryKind.END

    @property
    def is_control(self):
        return self.kind == TableEntryKind.CONTROL

    @property
    def is_hook(self):
        return self.kind == TableEntryKind.HOOK    

class TableControlEntry(TableEntry):
    def __init__(self, label, postfix=None, parameters=None):
        if not label:
            raise ValueError('label must be non-empty')
        self.kind = TableEntryKind.CONTROL
        self.label = label
        self.postfix = postfix
        self.parameters = parameters

    def _insert(self, table, binary):
        table.insert_control_entry(binary, self.label, self.parameters, self.postfix)

    def __str__(self):
        string = f'[{self.label}]'
        if self.postfix:
            string += _escape_postfix(self.postfix)
        if self.parameters:
            string += f',{self.parameters}'
        return string

    def to_tbl(self, binary):
        return f'${_to_hex(binary)}={str(self)}'

class TableTextEntry(TableEntry):
    def __init__(self, text):
        self.kind = TableEntryKind.TEXT
        self.text = text

    def _insert(self, table, binary):
        table.insert_text_entry(binary, self.text)

    def __str__(self):
        return f'"{self.text}"'

    def to_tbl(self, binary):
        return f'{_to_hex(binary)}={self.text}'

class TableEndEntry(TableEntry):
    def __init__(self, label, postfix=None):
        if not label:
            raise ValueError('label must be non-empty')
        self.kind = TableEntryKind.END
        self.label = label
        self.postfix = postfix

    def _insert(self, table, binary):
        table.insert_end_entry(binary, self.label, self.postfix)

    def __str__(self):
        string = f'[{self.label}]'
        if self.postfix:
            string += _escape_postfix(self.postfix)
        return string

    def to_tbl(self, binary):
        return f'/{_to_hex(binary)}={self}'

class TableSwitchEntry(TableEntry):
    def __init__(self, table):
        self.kind = TableEntryKind.TABLE_SWITCH
        self.table = table

    def __str__(self):
        return f'{self.table}'

    def to_tbl(self, binary):
        return f'!{_to_hex(binary)}=[{self.table}]'

class TableHookEntry(TableEntry):
    def __init__(self, name):
        self.kind = TableEntryKind.HOOK
        self.name = name

    def __str__(self):
        return f'{self.name}'

    def to_tbl(self, binary):
        return f'@{_to_hex(binary)}=[{self}]'

    def _insert(self, table, binary):
        table.insert_hook_entry(binary, self.name)

class Table:
    @staticmethod
    def load_tbl_file(filename):
        from kaizo.text.tblreader import read_tbl_file
        return read_tbl_file(filename)

    def __init__(self, table=None, entries=None):
        if table:
            if isinstance(table, _Table):
                self._table = table
            else:
                raise TypeError("must be an instance of _Table")
        else:
            self._table = _Table()
        
        if entries is not None:
            self.insert_entries(entries)

    def insert_entries(self, entry_pairs):
        for key, value in entry_pairs:
            self.insert_entry(key, value)

    def _as_binary(self, key):
        if isinstance(key, bytes):
            return key
        elif isinstance(key, int) and key < 0x100:
            return bytes([key])
        else:
            return bytes(key)

    def insert_entry(self, binary, entry):
        binary = self._as_binary(binary)
        try:
            entry._insert(self._table, binary)
        except AttributeError:
            if isinstance(entry, str):
                self._table.insert_text_entry(binary, entry)
            else:
                raise ValueError('invalid table entry')

    def get_entry(self, index):
        binary, _entry = self._table.get_entry(index)
        return binary, TableEntry._make(_entry)

    def __iter__(self):
        for i in range(self._table.entry_count):
            yield self.get_entry(i)

    def __len__(self):
        return self._table.entry_count

    def save_as_tbl(self, f):
        for binary, entry in self:
            f.write(entry.to_tbl(binary))
            f.write('\n')

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
    def __init__(self, table):
        encoding = _TableEncoding(table._table)
        super().__init__(encoding)

    def chunks(self, text):
        _chunks = self._encoding.chunks(text)
        return [Chunk(_chunk[0], _chunk[1], TableEntry._make(_chunk[2]),
                      None if len(_chunk) == 3 else _chunk[3]) for _chunk in _chunks]

    def add_hook(self, name, hook):
        if isinstance(hook, tuple):
            decoder, encoder = hook
            self._encoding.add_hook(name, decoder, encoder)
        else:
            raise ValueError('invalid hook handler')