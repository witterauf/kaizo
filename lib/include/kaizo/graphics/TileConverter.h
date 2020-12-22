#pragma once

#include "Palette.h"
#include "PixelFormat.h"

namespace kaizo {

class Tile;

class TileFilter
{
public:
    virtual ~TileFilter() = default;
    virtual auto convert(const Tile& tile) const -> Tile = 0;
};

class TileConverter : public TileFilter
{
public:
    auto fromFormat() const -> PixelFormat;
    auto toFormat() const -> PixelFormat;

protected:
    TileConverter(const PixelFormat from, const PixelFormat to);
    void setFromFormat(const PixelFormat format);
    void setToFormat(const PixelFormat format);

private:
    PixelFormat m_fromFormat;
    PixelFormat m_toFormat;
};

class Palettizer : public TileConverter
{
public:
    explicit Palettizer(const Palette& palette);

    void setPalette(const Palette& palette);

    auto convert(const Tile& tile) const -> Tile override;

private:
    Palette m_palette;
};

} // namespace kaizo