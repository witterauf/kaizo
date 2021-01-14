from kaizo.data.objects import BinaryObject, FixedAddressConstraint, AddressRangeConstraint
from kaizo.addresses import FileOffset
from kaizo.utilities import IntervalList
from kaizo.kaizopy import _BacktrackingPacker
from pathlib import Path
from sortedcontainers import SortedList
import shutil

class Packer:
    def pack(self, objects, free_blocks):
        pass

class BacktrackingPacker(Packer):
    """
    Packs a list of objects into a set of free blocks, satisfying any constraints
    provided by the individual objects.
    """

    def __init__(self, *, log='packing_log.txt'):
        self._packer = _BacktrackingPacker()
        if log is not None:
            self._packer.set_log_file(log)

    def pack(self, objects, free_blocks):
        # order important: first add free blocks
        for block in free_blocks:
            self._packer.add_free_block(block.offset, block.address, block.size)
        for obj in objects:
            if obj.constraints:
                raise ValueError('constraints not yet supported')
            self._packer.add_object(obj.actual_size)
        if not self._packer.pack():
            return False
        for i, obj in enumerate(objects):
            obj.link_offset = self._packer.get_link_offset(i)
            obj.link_address = self._packer.get_link_address(i)
        return True

class FreeBlock:
    @staticmethod
    def load(path, address_map):
        return []

    def __init__(self, offset, address, size):
        """
        Stores the start of a free, contiguous block using both its offset within
        the target and the address the offset maps to. This combination uniquely
        identifies any free block.
        """
        self.offset = offset
        self.address = address
        self.size = size

    def __len__(self):
        return self.size

    @property
    def end_offset(self):
        return self.offset + self.size

    """
    The following methods implement the Interval interface.
    """

    @property
    def lower(self):
        return self.offset

    @lower.setter
    def lower(self, value):
        self.size = max(self.end_offset - value, 0)
        self.offset = value

    @property
    def upper(self):
        return self.offset + self.size

    @upper.setter
    def upper(self, value):
        self.size = max(self.offset, value - self.offset)

    @property
    def length(self):
        return self.size

    @length.setter
    def length(self, value):
        self.size = max(value, 0)

    def contains(self, value):
        return self.lower <= value < self.upper

    def subtract(self, block):
        new_blocks = []
        if self.lower < block.upper < self.upper:
            size = self.upper - block.upper
            address = self.address.offset(block.upper - self.lower)
            new_blocks.append(FreeBlock(block.upper, address, size))
        if self.lower < block.lower < self.upper:
            size = block.lower - self.lower
            new_blocks.append(FreeBlock(self.lower, self.address, size))
        return new_blocks

class LinkTarget:
    """
    Represents a target for linking BinaryObjects into.
    """
    def __init__(self, address_map, free_blocks):
        self.address_map = address_map

        if isinstance(free_blocks, str):
            self.free_blocks = FreeBlock.load(Path(free_blocks), self.address_map)
        elif isinstance(free_blocks, list):
            self.free_blocks = free_blocks
        else:
            raise TypeError('free_blocks must be a list or a file name')

        self.free_blocks = IntervalList(free_blocks)

    def allocate(self, objects):
        if isinstance(objects, list):
            for obj in objects:
                self._allocate_object(obj)
        else:
            self._allocate_object(objects)

    def _allocate_object(self, obj):
        if obj.link_offset:
            address = self.address_map.map_to_target(FileOffset.from_int(obj.link_offset))
            if address is None:
                raise ValueError(f'could not map offset {obj.link_offset} to address')
            block = FreeBlock(obj.link_offset, address, obj.packed_size)
            self.free_blocks.subtract(block)

class FileLinkTarget(LinkTarget):
    """
    Represents a physical file to link BinaryObjects into.
    """

    def __init__(self, path, address_map, free_blocks=[], destination=None):
        super().__init__(address_map, free_blocks)
        self.path = path
        self.destination = destination

    def apply(self, objects):
        """
        Write the contents of the given objects to the destination file.
        """
        if self.destination:
            shutil.copy(self.path, self.destination)
            self._f = open(self.destination, 'r+b')
        else:
            self._f = open(self.path, 'r+b')
        for obj in objects:
            obj.apply(self)
        self._f.close()

    def read(self, offset, size):
        self._f.seek(offset, 0)
        return self._f.read(size)

    def write(self, data, offset):
        print(f'Writing to {offset}')
        self._f.seek(offset, 0)
        self._f.write(data)

def _pack_with_fixed_offset(objects, target):
    remaining = []
    for obj in objects:
        if obj.link_offset is not None:
            target.allocate(obj)
        else:
            remaining.append(obj)
    return remaining

def pack_objects(objects, targets, *, packer=None):
    """
    Pack the given objects into the free blocks of the given targets.
    The objects may provide constraints.
    """
    if packer is None:
        packer = BacktrackingPacker()
    if not isinstance(targets, list):
        targets = [targets]
    assert len(targets) == 1, "only one simultaneous target supported at the moment"

    objects = _pack_with_fixed_offset(objects, targets[0])
    return packer.pack(objects, targets[0].free_blocks)

def resolve_references(objects):
    """
    Resolve any unresolved references within the given objects.
    All referenced data paths need to have a corresponding object that has a
    link_offset specified.
    """
    link_addresses = {}
    for obj in objects:
        link_addresses[obj.path] = obj.link_address
    for obj in objects:
        obj.resolve_references(link_addresses)

def update_targets(objects, targets):
    """
    Write the contents of the given objects into the allocated blocks of the given
    targets.
    """
    if not isinstance(targets, list):
        targets = [targets]
    assert len(targets) == 1, "only one simultaneous target supported at the moment"
    targets[0].apply(objects)