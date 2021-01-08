from fuse.fusepy import Signedness, Endianness
from fuse.fusepy import Address, _FileOffset, _AddressFormat, _AddressLayout, _RelativeAddressLayout, _MipsEmbeddedLayout, _AddressMap, _RegionedAddressMap

class AddressLayout:
    @staticmethod
    def _make(layout):
        if isinstance(layout, _RelativeAddressLayout):
            return RelativeAddressLayout._make(layout)
        elif isinstance(layout, _MipsEmbeddedLayout):
            return MipsEmbeddedLayout._make(layout)
        elif isinstance(layout, _AddressLayout):
            return AddressLayout(layout)
        else:
            raise TypeError("expected an _AddressLayout or subclass")

    def __init__(self, layout):
        self._layout = layout

    def to_dict(self):
        return self._layout.to_dict()

    def id(self):
        return self._layout.id()

    def encode(self, address):
        return self._layout.encode(address)

    def write(self, target, offset, address):
        patches = self._layout.encode(address)
        for patch in patches:
            patch.apply(target, offset)

class RelativeAddressLayout(AddressLayout):
    ID = 'relative'

    @staticmethod
    def _make(_layout):
        layout = RelativeAddressLayout.__new__(RelativeAddressLayout)
        layout._layout = _layout
        return layout

    def __init__(self, base, size, null_pointer=None, signedness=Signedness.UNSIGNED,
                 endianness=Endianness.LITTLE):
        self._layout = _RelativeAddressLayout()
        self._layout.set_layout(size, signedness, endianness)
        if isinstance(base, Address):
            self._layout.set_fixed_base_address(base)
        else:
            self._layout.set_fixed_base_address(FileOffset.from_int(int(base)))
        if null_pointer is not None:
            self._layout.set_null_pointer(null_pointer[0], null_pointer[1])

class MipsEmbeddedLayout(AddressLayout):
    ID = 'mips'

    @staticmethod
    def _make(_layout):
        layout = MipsEmbeddedLayout.__new__(MipsEmbeddedLayout)
        layout._layout = _layout
        return layout

    def __init__(self, base, lo16, hi16):
        self._layout = _MipsEmbeddedLayout()
        self._layout.set_offsets(hi16, lo16)
        self._layout.set_base_address(base)

class AddressFormat:
    def __init__(self, format):
        if not isinstance(format, _AddressFormat):
            raise TypeError("expected an _AddressFormat")
        self._format = format

    def from_int(self, value):
        as_int = int(value)
        return self._format.from_int(as_int)

    def __call__(self, value):
        return self.from_int(value)

FileOffset = AddressFormat(_FileOffset)

class AddressMap:
    def __init__(self, map):
        if not isinstance(map, _AddressMap):
            raise TypeError("expected an _AddressMap")
        self._map = map

    def map_to_sources(self, target):
        return self._map.map_to_sources(target)

    def map_to_source(self, target):
        sources = self.map_to_sources(target)
        if len(sources) > 1:
            raise ValueError('target address maps to more than one source')
        elif len(sources) == 0:
            raise ValueError('target address maps to no source')
        return sources[0]

    def map_to_target(self, source):
        return self._map.map_to_target(source)

class RegionedAddressMap(AddressMap):
    def __init__(self, source_format, target_format):
        self._map = _RegionedAddressMap(source_format._format, target_format._format)

    def add_region(self, source, target, size):
        self._map.add_region(source, target, size)