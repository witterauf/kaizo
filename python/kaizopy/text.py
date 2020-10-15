from kaizopy._kaizopy import _Table
from enum import Enum

class TableEntryKind(Enum):
    TEXT = 0
    CONTROL = 1
    TABLE_SWITCH = 2
    END = 3

class Table:
    def __init__(self, table=None):
        if table:
            if isinstance(table, _Table):
                self._table = table
            else:
                raise TypeError("must be an instance of _Table")
        else:
            self._table = _Table()

    def insert_text_entry(self, key_bytes, text):
        self._table.insert_text_entry(key_bytes, text)

    def insert_end_entry(self, key_bytes, label):
        self._table.insert_end_entry(key_bytes, label)

    def insert_control_entry(self, key_bytes, label, parameter_format=None, postfix=None):
        if parameter_format:
            self._table.insert_control_entry(key_bytes, label, parameter_format)
        else:
            self._table.insert_control_entry(key_bytes, label)

    def insert_entries(self, entries):
        for key, value in entries.items():
            if "kind" not in value or value["kind"] == TableEntryKind.TEXT:
                self._table.insert_text_entry(key, value["text"])
            elif value["kind"] == TableEntryKind.CONTROL:
                parameters = value.get("parameters", None)
                suffix = value.get("suffix", None)
                self._table.insert_control_entry(key, value["label"], parameters, suffix)
            elif value["kind"] == TableEntryKind.END:
                self._table.insert_end_entry(key, value["label"])

def make_control(label, parameters=None, suffix=None):
    control = {
        "kind": TableEntryKind.CONTROL,
        "label": label,
    }

    if parameters:
        control["parameters"] = parameters
    if suffix:
        control["suffix"] = suffix
        
    return control

def make_end(label):
    return {
        "kind": TableEntryKind.END,
        "label": label
    }