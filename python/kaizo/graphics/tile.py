from kaizo.kaizopy import _Tile, _TileFormat
from kaizo.graphics.palette import ColorFormat
from abc import ABC, abstractmethod

# TODO: put into utilities after merging with fuse
class Rectangle:
    def __init__(self, x1, y1, x2, y2):
        if x1 <= x2:
            self.x1 = x1
            self.x2 = x2
        else:
            self.x1 = x2
            self.x2 = x1

        if y1 <= y2:
            self.y1 = y1
            self.y2 = y2
        else:
            self.y1 = y2
            self.y2 = y1

    @property
    def left(self):
        return self.x1

    @property
    def right(self):
        return self.x2

    @property
    def top(self):
        return self.y1

    @property
    def bottom(self):
        return self.y2

    @property
    def width(self):
        return self.x2 - self.x1

    @property
    def height(self):
        return self.y2 - self.y1

    @property
    def area(self):
        return self.width * self.height

    def __str__(self):
        return f'[{self.left},{self.right})x[{self.top},{self.bottom})'

class Tile:
    @classmethod
    def _make(cls, _tile, format=None):
        tile = cls.__new__(cls)
        tile._tile = _tile
        if format is None:
            tile.format = ColorFormat._make(_tile.format)
        else:
            tile.format = format
        tile.width = _tile.width
        tile.height = _tile.height
        return tile

    @staticmethod
    def load(filename):
        _tile, _ = _Tile.load(filename)
        return Tile._make(_tile), None

    def __init__(self, width, height, format, tile=None):
        if tile is None:
            self._tile = _Tile(width, height, format._format)
        else:
            self._tile = tile
        self.format = format
        self.width = width
        self.height = height

    def fill(self, color):
        self._tile.fill(color)

    def set_pixel(self, x, y, packed):
        self._tile.set_pixel(x, y, packed)

    def set_pixel_color(self, x, y, color):
        self.set_pixel(x, y, self.format.pack(color))

    def get_pixel(self, x, y):
        return self._tile.get_pixel(x, y)

    def get_pixel_color(self, x, y):
        return self.format.unpack(self.get_pixel(x, y))

    def crop(self, x, y, width, height):
        return Tile._make(self._tile.crop(x, y, width, height), self.format)

    def blit(self, tile, x, y, bgcolor, region=None):
        if region:
            self._tile.blit_partial(tile._tile, region.left, region.top,
                                    region.width, region.height, x, y, bgcolor)
        else:
            self._tile.blit(tile._tile, x, y, bgcolor)

    def save(self, filename):
        self._tile.save(filename)

    def bounding_box(self, bgcolor):
        coordinates = self._tile.bounding_box(int(bgcolor))
        return Rectangle(*coordinates)

class Image(Tile):
    pass

class TileFormat(ABC):
    @staticmethod
    def make(format, properties):
        _format = _TileFormat.make(format, properties)
        if _format is None:
            raise ValueError(f'unknown format: {format}')
        return _PyTileFormat(_format)

    @abstractmethod
    def encode_tile(self, binary, tile, offset=0):
        pass

    @abstractmethod
    def decode_tile(self, binary, offset):
        pass

    @abstractmethod
    def encoded_size(self, count):
        pass

    def encode(self, tiles):
        if isinstance(tiles, Tile):
            tiles = [tiles]
        overall_size = self.encoded_size(len(tiles))
        binary = bytearray(overall_size)
        offset = 0
        for tile in tiles:
            offset = self.encode_tile(binary, tile, offset)
        return binary

    def decode(self, binary, offset=0, *, count=None, size=None):
        if size is not None and count is None:
            count = int(size // self.encoded_size)
        elif size is None and count is None:
            count = 1
        elif size is not None and count is not None:
            raise ValueError('cannot give both count and size')

        if offset + self.encoded_size(count) > len(binary):
            raise ValueError(f'binary too small for {count} tiles of size {self.encoded_size}')

        tiles = []
        for _ in range(count):
            tile, offset = self.decode_tile(binary, offset)
            tiles.append(tile)
        return tiles

class _PyTileFormat(TileFormat):
    def __init__(self, format):
        if not isinstance(format, _TileFormat):
            raise TypeError('format must be of type _TileFormat')
        self._format = format

    def encode_tile(self, binary, tile, offset=0):
        return self._format.encode(binary, offset, tile._tile)

    def decode_tile(self, binary, offset=0):
        return Tile._make(self._format.decode(binary, offset))

    def encoded_size(self, count):
        return self._format.encoded_size(count)