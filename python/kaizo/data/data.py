import re
import csv

class DataPathElement:
    pass

class DataPathIndexElement:
    def __init__(self, index):
        self.index = int(index)
        self.is_index = True
        self.is_name = False

    def __str__(self):
        return str(self.index)

    def __repr__(self):
        return f'DataPathIndexElement({self.index})'

class DataPathNameElement:
    def __init__(self, name):
        self.name = str(name)
        self.is_index = False
        self.is_name = True

    def __str__(self):
        return self.name

    def __repr__(self):
        return f'DataPathNameElement("{self.name}")'

class DataPath:
    RE_NAME = '[A-Za-z][A-Za-z0-9_]*'
    RE_INDEXED_NAME = '(?P<name>[a-zA-Z][a-zA-Z0-9_]*)?(?:\\[(?:[0-9]+)\\])+'
    RE_INDEX = '(?:\\[(?P<index>[0-9]+)\\])'

    def __init__(self, path):
        self._split(path)
        self.path = path

    def _split(self, path):
        elements = str(path).split('.')
        self.elements = []
        for element in elements:
            if '[' in element:
                self._split_indices(element)
            elif re.fullmatch(DataPath.RE_NAME, element):
                self.elements.append(DataPathNameElement(element))
            else:
                raise ValueError(f'"{element}" is an invalid path element')

    def _split_indices(self, element):
        match = re.fullmatch(DataPath.RE_INDEXED_NAME, element)
        if match:
            if match.group('name'):
                self.elements.append(DataPathNameElement(match.group('name')))
            indices = re.findall(DataPath.RE_INDEX, element)
            for index in indices:
                self.elements.append(DataPathIndexElement(int(index)))
        else:
            raise ValueError('invalid path')

    def __repr__(self):
        return f'DataPath("{self.path}")'

def set_value(data, path, value):
    path = DataPath(path)
    if data is None:
        if path.elements[0].is_index:
            data = []
        else:
            data = {}
    _set_value(data, path.elements, value)
    return data

def _set_value(data, elements, value):
    head, tail = elements[0], elements[1:]
    if head.is_index:
        if len(data) < head.index:
            data += [None] * (head.index - len(data))
        if tail:
            data.append(_make(tail[0]))
            _set_value(data[head.index], tail, value)
        else:
            data.append(value)
    else:
        if tail:
            if head.name not in data:
                data[head.name] = _make(tail[0])
            _set_value(data[head.name], tail, value)
        else:
            data[head.name] = value

def _make(element):
    if element.is_index:
        return []
    else:
        return {}

def _unflatten_array(flat_data):
    data = []
    for path, value in flat_data:
        first = path[0]
        if first.is_index:
            if len(data) <= first.index:
                data += [None] * (first.index - len(data) + 1)
            if data[first.index]:
                data[first.index].append((path[1:], value))
            else:
                data[first.index] = [(path[1:], value)]
        else:
            raise ValueError('inconsistent types; expected array element')
    for index, flat_data in enumerate(data):
        if flat_data and not flat_data[0][0]:
            data[index] = flat_data[-1][1]
        elif flat_data:
            data[index] = _unflatten(flat_data)
    return data

def _unflatten_record(flat_data):
    data = {}
    for path, value in flat_data:
        first = path[0]
        if first.is_name:
            if first.name not in data:
                data[first.name] = []
            data[first.name].append((path[1:], value))
        else:
            raise ValueError('inconsistent types; expected record element')
    for key, flat_data in data.items():
        if not flat_data[0][0]:
            data[key] = flat_data[-1][1]
        else:
            data[key] = _unflatten(flat_data)
    return data

def _unflatten(flat_data):
    if flat_data[0][0][0].is_index:
        return _unflatten_array(flat_data)
    else:
        return _unflatten_record(flat_data)

def unflatten(flat_data):
    expanded = [(DataPath(path).elements, value) for path, value in flat_data]
    return _unflatten(expanded)

def _convert(value, type):
    if type == 'string':
        return str(value)
    elif type == 'int':
        return int(value)
    elif type == 'null':
        return None
    else:
        raise ValueError(f'unsupported type "{type}"')

def read_hierarchical_csv(file):
    with open(file, 'r', newline='', encoding='utf-8') as f:
        reader = csv.reader(f)
        rows = []
        for r, row in enumerate(reader):
            try:
                data = _convert(row[2], row[1])
                rows.append((row[0], data,))
                #rows = [(row[0], _convert(row[2], row[1])) for r, row in enumerate(reader)]
            except IndexError:
                if len(row) > 0:
                    raise IndexError(f'row {r} ({row[0]}) does not have enough columns (must have 3)')
                else:
                    raise IndexError(f'row {r} does not have enough columns (must have 3)') 
        return unflatten(rows)
