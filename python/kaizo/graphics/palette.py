from kaizopy._kaizopy import _Palette, _PixelFormat

class ColorFormat:
    @staticmethod
    def _make(_format):
        if _format.kind == "indexed":
            if _format.bits_per_pixel == 4:
                return ColorFormat.Indexed4
            else:
                return IndexedColorFormat(_format.bits_per_pixel)
        elif _format.kind == "rgba":
            if _format.bits_per_channel == 8:
                return ColorFormat.Rgba
            else:
                raise ValueError('unsupported ColorFormat')

class IndexedColorFormat(ColorFormat):
    def __init__(self, bpp):
        self.channels = 1
        self.bits_per_pixel = bpp
        self.bits_per_channel = self.bits_per_pixel
        self.is_indexed = True
        self._format = _PixelFormat.make_indexed(self.bits_per_pixel)

    def unpack(self, packed):
        return packed

    def pack(self, unpacked):
        return unpacked

    def __str__(self):
        return f'Indexed ({self.bits_per_pixel}bpp)'

class RgbaColorFormat(ColorFormat):
    def __init__(self):
        self.channels = 4
        self.bits_per_pixel = 32
        self.bits_per_channel = 8
        self.is_indexed = False
        self._format = _PixelFormat.make_channels('R.G.B.A', 8)

    def unpack(self, packed):
        return (packed & 0xff, (packed >> 8) & 0xff, (packed >> 16) & 0xff, (packed >> 24) & 0xff)

    def pack(self, unpacked):
        return (unpacked[0] & 0xff) | ((unpacked[1] & 0xff) << 8) | ((unpacked[2] & 0xff) << 16) | ((unpacked[3] & 0xff) << 24)

    def __str__(self):
        return 'RGBA (8.8.8.8)'

ColorFormat.Rgba = RgbaColorFormat()
ColorFormat.Indexed4 = IndexedColorFormat(4)

class Palette:
    def __init__(self, format, colors):
        self._palette = _Palette(format._format, len(colors))
        self.color_format = format
        if isinstance(colors, dict):
            for index, color in colors.items():
                self._palette.set_color(index, format.pack(color))
        else:
            for index, color in enumerate(colors):
                self._palette.set_color(index, format.pack(color))

    def color(self, index):
        packed = self._palette.color(index)
        return self.color_format.unpack(packed)

    def set_color(self, index, color):
        self._palette.set_color(index, self.color_format.pack(color))        

    @property
    def index_format(self):
        return ColorFormat._make(self._palette.index_format)