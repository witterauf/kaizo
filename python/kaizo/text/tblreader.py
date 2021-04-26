from kaizo.text import Table, TableTextEntry, TableEndEntry, TableControlEntry, TableEntryKind, TableHookEntry
import parsy
import re

def to_hex(characters):
    return [int(characters[i:i+2], 16) for i in range(0, len(characters), 2)]

parameter_format = parsy.regex('[><]?[0-9]+[dDxXbB]?')

@parsy.generate
def parameter_list():
    comma = yield parsy.string(',').optional()
    if comma:
        parameters = yield parameter_format.many()
        if parameters:
            return ''.join(parameters)
        else:
            return parsy.fail('named parameters not supported yet')

control_label = parsy.regex('\[.+\]').map(lambda x: x[1:-1]).desc('control label')
control_postfix = parsy.regex('(\\\\n)+')

@parsy.generate
def control_rhs():
    label = yield control_label
    postfix = yield control_postfix.optional()
    parameters = yield parameter_list.optional()
    return TableControlEntry(label, postfix, parameters)

@parsy.generate
def end_rhs():
    label = yield control_label
    postfix = yield control_postfix.optional()
    return TableEndEntry(label, postfix)

@parsy.generate
def hook_rhs():
    label = yield control_label
    return TableHookEntry(label)

text_rhs = parsy.regex('.+$', flags=re.M).map(lambda x: TableTextEntry(x)).desc('text entry')

codepoint = parsy.regex('([0-9a-fA-F]{2})+').map(to_hex).desc('code point in hexadecimal')
text_entry = parsy.peek(parsy.regex('[0-9a-fA-F]')).map(lambda x: TableEntryKind.TEXT)
control_entry = parsy.string('$').map(lambda x: TableEntryKind.CONTROL)
end_entry = parsy.string('/').map(lambda x: TableEntryKind.END)
hook_entry = parsy.string('@').map(lambda x: TableEntryKind.HOOK)
entry_kind = text_entry | control_entry | end_entry | hook_entry

@parsy.generate
def line():
    kind = yield entry_kind
    binary = yield codepoint
    yield parsy.string('=')
    if kind == TableEntryKind.TEXT:
        entry = yield text_rhs        
    elif kind == TableEntryKind.CONTROL:
        entry = yield control_rhs
    elif kind == TableEntryKind.END:
        entry = yield end_rhs
    elif kind == TableEntryKind.HOOK:
        entry = yield hook_rhs
    else:
        return parsy.fail('unsupported table entry kind')
    return (binary, entry)

newline = parsy.regex('[\\r\\n(\\r\\n)(\\n\\r)]')
first_line = line.map(lambda x: [x])
remaining_lines = (newline >> line).many()
tbl_file = first_line + remaining_lines << newline.optional()

@parsy.generate
def tbl():    
    entries = yield tbl_file
    return entries

def read_tbl(tbl_contents):
    table = Table()
    entries = tbl.parse(tbl_contents)
    table.insert_entries(entries)
    return table

def read_tbl_file(tbl_file):
    with open(tbl_file, 'r', encoding='utf-8') as tbl:
        lines = tbl.read()
        return read_tbl(lines)