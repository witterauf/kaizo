from fusepy._fusepy import *
from fusepy._fusepy import _FileOffset, _DataFormat, _StringFormat, _IntegerFormat, _RecordFormat, _ArrayFormat, _PointerFormat
#from fusepy.addresses import AddressLayout, AddressFormat

class DataFormat:
    def _setup(self, **kwargs):
        if "skip_after" in kwargs:
            self._format.set_skip_after(int(kwargs['skip_after']))
        if "skip_before" in kwargs:
            self._format.set_skip_before(int(kwargs['skip_before']))
        if "tag" in kwargs:
            self._format.set_tag(str(kwargs['tag']))
        if "fixed_offset" in kwargs:
            self._format.set_fixed_offset(int(kwargs['fixed_offset']))
        if "alignment" in kwargs:
            self._format.set_alignment(int(kwargs['alignment']))

    def decode(self, reader):
        return self._format.decode(reader._reader)

class IntegerFormat(DataFormat):
    def __init__(self, size, unsignedness=Signedness.UNSIGNED, endianness=Endianness.LITTLE, **kwargs):
        self._format = _IntegerFormat(size, unsignedness, endianness)
        self._setup(**kwargs)

class StringFormat(DataFormat):
    def __init__(self, encoding, **kwargs):
        self._format = _StringFormat(encoding)
        self._setup(**kwargs)

class RecordFormat(DataFormat):
    def __init__(self, elements, **kwargs):
        self._format = _RecordFormat()
        self._setup(**kwargs)
        for element in elements:
            if not isinstance(element[1], DataFormat):
                raise TypeError("expected DataFormat")
            if not element[0]:
                raise ValueError("expected a non-empty string")
            self._format.append_element(element[0], element[1]._format)

class ArrayFormat(DataFormat):
    def __init__(self, length, element, **kwargs):
        self._format = _ArrayFormat()
        self._setup(**kwargs)
        if isinstance(length, int):
            self._format.set_fixed_length(length)
        else:
            raise TypeError("expected an integer")
        if not isinstance(element, DataFormat):
            raise TypeError("expected a DataFormat")
        self._format.set_element_format(element._format)

class PointerFormat(DataFormat):
    def __init__(self, pointee, layout, format, use_address_map=False, null_pointer=None, **kwargs):
        self._format = _PointerFormat()
        self._format.set_pointee_format(pointee._format)
        self._format.set_address_layout(layout._layout)
        self._format.set_address_format(format._format)
        if null_pointer is not None:
            self._format.set_null_pointer(null_pointer)
        self._format.use_address_map(use_address_map)
        self._setup(**kwargs)