from kaizo.graphics.tile import TileFormat
from kaizo.kaizopy import _psp_swizzle, _psp_unswizzle

def _psp4bpp(width, height):
    return TileFormat.make('psp.4bpp', {'width': width, 'height': height})

def swizzle(binary, width, start=0, end=0):
    return _psp_swizzle(binary, width, start, end)

def unswizzle(binary, width, start=0, end=0):
    return _psp_unswizzle(binary, width, start, end)

TileFormat.Psp4bpp = _psp4bpp
