from fusepy._fusepy import *
from fusepy._fusepy import _FileOffset, _AddressFormat, _AddressLayout, _RelativeAddressLayout, _AddressMap, _RegionedAddressMap, _MipsEmbeddedLayout

class AddressLayout:
    def __init__(self, layout):
        if not isinstance(format, _AddressLayout):
            raise TypeError("expected an _AddressLayout")
        self._layout = layout

class RelativeAddressLayout(AddressLayout):
    def __init__(self, base, size, null_pointer=None, signedness=Signedness.UNSIGNED, endianness=Endianness.LITTLE):
        self._layout = _RelativeAddressLayout()
        self._layout.set_layout(size, signedness.value, endianness.value)
        if isinstance(base, int):
            self._layout.set_fixed_base_address(int(base))
        else:
            raise TypeError("expected an integer as base")
        if null_pointer is not None:
            self._layout.set_null_pointer(null_pointer[0], null_pointer[1])

class MipsEmbeddedLayout(AddressLayout):
    def __init__(self, base, lo16, hi16):
        self._layout = _MipsEmbeddedLayout()
        self._layout.set_offsets(hi16, lo16)
        self._layout.set_base_address(base)

class AddressFormat:
    def __init__(self, format):
        if not isinstance(format, _AddressFormat):
            raise TypeError("expected an _AddressFormat")
        self._format = format

FileOffset = AddressFormat(_FileOffset)

class AddressMap:
    def __init__(self, map):
        if not isinstance(map, _AddressMap):
            raise TypeError("expected an _AddressMap")
        self._map = map

class RegionedAddressMap(AddressMap):
    def __init__(self, source_format, target_format):
        self._map = _RegionedAddressMap(source_format._format, target_format._format)

    def add_region(self, source, target, size):
        self._map.add_region(source, target, size)