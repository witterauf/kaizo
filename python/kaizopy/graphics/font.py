from kaizopy._kaizopy import Tile
from pathlib import PurePath
import json
from enum import Enum

def _build_font(description):
    metrics = description['metrics']
    pixel_format = description['pixel_format']
    font = BitmapFont(lineheight=metrics['lineheight'], baseline=metrics['baseline'],
                        bpp=pixel_format['bits_per_pixel'],
                        bgcolor=pixel_format['background_color'])

    bitmaps = []
    for path in description['bitmaps']:
        image, _ = Tile.load_from_file(path)
        bitmaps.append(image)

    glyphs = description['glyphs']
    for glyphdesc in glyphs:
        if glyphdesc['shrink']:
            raise ValueError('shrinking not yet supported')

        bounding_box = glyphdesc['bounding_box']
        x1 = bounding_box[0]
        y1 = bounding_box[1]
        x2 = bounding_box[2]
        y2 = bounding_box[3]
        bitmap = int(glyphdesc['bitmap'])
        glyph_data = bitmaps[bitmap].crop(x1, y1, x2 - x1, y2 - y1)

        baseline = glyphdesc['baseline']

        font.append_glyph(BitmapGlyph(glyphdesc['characters'], glyph_data, baseline))

    return font

def _load_json_font(path):
    with open(path, 'r') as f:
        description = json.load(f)
        return _build_font(description)

class BitmapGlyph:
    def __init__(self, characters, tile, baseline, bgcolor=0):
        if not characters:
            raise ValueError('characters of a BitmapGlyph must not be empty')
        if not 0 <= baseline < tile.height:
            raise ValueError('baseline of a BitmapGlyph must be between 0 and its height')

        self.characters = characters
        self.tile = tile
        self.baseline = baseline
        self.bgcolor = bgcolor

    @property
    def width(self):
        return self.tile.width

    @property
    def height(self):
        return self.tile.height

    @property
    def ascent(self):
        return self.baseline

    @property
    def descent(self):
        return self.height - self.baseline

    @property
    def advance_width(self):
        return self.width + 1

    def get_pixel(self, x, y):
        return None

    def bounding_box(self):
        return ((0, 0), (0, 0))

    def __str__(self):
        return self.characters

class BitmapFont:
    @staticmethod
    def load(filename):
        path = PurePath(filename)
        if path.suffix == '.json':
            return _load_json_font(path)
        else:
            raise ValueError('unsupported bitmap font description file')

    def __init__(self, lineheight, baseline, bpp=32, bgcolor=0):
        if lineheight <= 0:
            raise ValueError('the lineheight of a BitmapFont must be positive')
        if not (1 <= baseline < lineheight):
            raise ValueError('the baseline of a BitmapFont must be between 1 and its lineheight')
        self.lineheight = lineheight
        self.baseline = baseline
        self.bpp = bpp
        self.bgcolor = bgcolor
        self.glyphs = []

    def append_glyph(self, glyph):
        glyph.bgcolor = self.bgcolor
        self.glyphs.append(glyph)

    def glyph_count(self):
        return len(self.glyphs)

    @property
    def glyph(self, index):
        return self.glyphs[index]

    def to_glyphs(self, string):
        glyphs = []
        i = 0
        while i < len(string):
            match = self._longest_match(string, i)
            if match is None:
                raise ValueError('no matching glyph')
            glyphs.append(self.glyphs[match])
            i += len(self.glyphs[match].characters)
        return glyphs

    def _longest_match(self, string, start):
        # TODO: optimize using a binary tree or similar
        longest_match = None
        match_length = 0
        for index, glyph in enumerate(self.glyphs):
            if len(glyph.characters) > match_length and string.startswith(glyph.characters, start):
                longest_match = index
                match_length = len(glyph.characters)
        return longest_match

class VerticalAnchor(Enum):
    TOP = 0
    BASELINE = 1
    BOTTOM = 2

class BitmapFontWriter:
    def __init__(self):
        self.vertical_anchor = VerticalAnchor.BASELINE

    def set_font(self, font):
        self.font = font

    def set_anchor(self, anchor):
        self.vertical_anchor = anchor

    def write(self, text, canvas, x, y):
        if self.vertical_anchor == VerticalAnchor.TOP:
            y += self.font.baseline
        elif self.vertical_anchor == VerticalAnchor.BOTTOM:
            y -= self.font.lineheight

        glyphs = self.font.to_glyphs(text)
        for glyph in glyphs:
            #self._write_glyph(glyph, canvas, x, y - glyph.ascent)
            canvas.blit(glyph.tile, x, y - glyph.ascent, glyph.bgcolor)
            x += glyph.advance_width

    def _write_glyph(self, glyph, canvas, x, y):
            for gy in range(glyph.height):
                for gx in range(glyph.width):
                    if glyph.tile.get_pixel(gx, gy) != glyph.bgcolor:
                        print(x + gx, y + gy, glyph.tile.get_pixel(gx, gy))
                        canvas.set_pixel(x + gx, y + gy, glyph.tile.get_pixel(gx, gy))
