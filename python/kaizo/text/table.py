from kaizopy._kaizopy import _Table
from enum import Enum

class TableEntryKind(Enum):
    TEXT = 0
    CONTROL = 1
    TABLE_SWITCH = 2
    END = 3

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
        table.insert_end_entry(binary, self.label)

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

class Table:
    @staticmethod
    def load_tbl_file(filename):
        from kaizopy.text.tblreader import read_tbl_file
        return read_tbl_file(filename)

    def __init__(self, table=None):
        if table:
            if isinstance(table, _Table):
                self._table = table
            else:
                raise TypeError("must be an instance of _Table")
        else:
            self._table = _Table()        

    def insert_entries(self, entry_pairs):
        for key, value in entry_pairs:
            self.insert_entry(key, value)

    def insert_entry(self, binary, entry):
        try:
            entry._insert(self._table, binary)
        except:
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

