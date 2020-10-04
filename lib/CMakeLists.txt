set(KAIZO_BASE_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(KAIZO_INCLUDE_DIRECTORY "${KAIZO_BASE_INCLUDE_DIRECTORY}/kaizo")

###[ sources ]#####################################################################################

set(KAIZO_BINARY_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/binary/Binary.h
    ${KAIZO_INCLUDE_DIRECTORY}/binary/BinaryStream.h
    src/binary/Binary.cc
    src/binary/BinaryStream.cc
)

set(KAIZO_BINARY_LOADERS_SOURCES
    binary/loaders/LuaDataFormatLoader.h
    binary/loaders/LuaDataFormatLoader.cc
    binary/loaders/LuaIntegerFormatLoader.h
    binary/loaders/LuaIntegerFormatLoader.cc
    binary/loaders/LuaArrayFormatLoader.h
    binary/loaders/LuaArrayFormatLoader.cc
    binary/loaders/LuaStringFormatLoader.h
    binary/loaders/LuaStringFormatLoader.cc
    binary/loaders/LuaRelativeOffsetFormatLoader.h
    binary/loaders/LuaRelativeOffsetFormatLoader.cc
    binary/loaders/LuaRecordFormatLoader.h
    binary/loaders/LuaRecordFormatLoader.cc
)

set(KAIZO_TEXT_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/text/ShiftJis.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/Table.h
    #${KAIZO_INCLUDE_DIRECTORY}/text/TableReader.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableDecoder.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEncoder.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableMapper.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/LuaTableReader.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/LuaTableWriter.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/LuaTableLibrary.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEntry.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TextEncoding.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEncoding.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/StringSet.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/AsciiEncoding.h
    text/ShiftJis.cc
    text/ShiftJisToUnicode.h
    text/Table.cc
    text/TableDecoder.cc
    #text/TableParser.h
    #text/TableParser.cc
    #text/TableReader.cc
    text/TableEncoder.cc
    text/TableMapper.cc
    text/LuaTableReader.cc
    text/LuaTableWriter.cc
    text/TableEntry.cc
    text/TableControlParser.h
    text/TableControlParser.cc
    text/LuaTableLibrary.cc
    text/TableEncoding.cc
    text/StringSet.cc
    text/AsciiEncoding.cc
)

set(KAIZO_GRAPHICS_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/GraphicsLuaLibrary.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/font/Glyph.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/font/Font.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/font/FontWriter.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/font/FontLoader.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/font/LuaFontLibrary.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/image/Image.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/image/ImageFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/image/LuaImageLibrary.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/Palette.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/Tile.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/TileColorRemapper.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/TileFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/TileImageFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/TileList.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/TileTransformation.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/tiles/TilesLuaLibrary.h
    graphics/GraphicsLuaLibrary.cc
    graphics/font/Glyph.cc
    graphics/font/Font.cc
    graphics/font/FontWriter.cc
    graphics/font/FontLoader.cc
    graphics/font/LuaFontLibrary.cc
    graphics/font/private/LuaFontLoader.h
    graphics/font/private/LuaFontLoader.cc
    graphics/font/private/FontBuilder.h
    graphics/font/private/FontBuilder.cc
    graphics/image/Image.cc
    graphics/image/ImageFormat.cc
    graphics/image/PngFormat.h
    graphics/image/PngFormat.cc
    graphics/image/LuaImageLibrary.cc
    graphics/tiles/Palette.cc
    graphics/tiles/Tile.cc
    graphics/tiles/TileColorRemapper.cc
    graphics/tiles/TileFormat.cc
    graphics/tiles/TileImageFormat.cc
    graphics/tiles/TileList.cc
    graphics/tiles/TilesLuaLibrary.cc
)

set(KAIZO_SYSTEMS_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/systems/LuaSystemsLibrary.h
    ${KAIZO_INCLUDE_DIRECTORY}/systems/gbc/GameboyColorAddress.h
    ${KAIZO_INCLUDE_DIRECTORY}/systems/psp/TextureSwizzle.h
    ${KAIZO_INCLUDE_DIRECTORY}/systems/psp/LuaPspLibrary.h
    systems/LuaSystemsLibrary.cc
    systems/gbc/GameboyColorAddress.cc
    systems/psp/TextureSwizzle.cc
    systems/psp/LuaPspLibrary.cc
)

set(KAIZO_VFS_SOURCES
  ${KAIZO_INCLUDE_DIRECTORY}/vfs/VirtualFileSystem.h
  src/vfs/VirtualFileSystem.cc
)

set(KAIZO_SOURCES
    ${KAIZO_BINARY_SOURCES}
    ${KAIZO_VFS_SOURCES}
    #${KAIZO_TEXT_SOURCES}
    #${KAIZO_GRAPHICS_SOURCES}
    #${KAIZO_SYSTEMS_SOURCES}
)

source_group("Binary" FILES ${KAIZO_BINARY_SOURCES})
source_group("VFS" FILES ${KAIZO_VFS_SOURCES})
#source_group("Graphics" FILES ${KAIZO_GRAPHICS_SOURCES})
#source_group("Systems" FILES ${KAIZO_SYSTEMS_SOURCES})
#source_group("Text" FILES ${KAIZO_TEXT_SOURCES})

add_library(KaizoLibrary
    ${KAIZO_SOURCES}
)
target_include_directories(KaizoLibrary
    PUBLIC
        $<BUILD_INTERFACE:${KAIZO_BASE_INCLUDE_DIRECTORY}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(KaizoLibrary
    PRIVATE
        Diagnostics::Diagnostics
        #LodePNG::LodePNG
    PUBLIC
        Fuse::Fuse
)
target_compile_features(KaizoLibrary PUBLIC cxx_std_20)
target_all_warnings(KaizoLibrary)
target_warnings_are_errors(KaizoLibrary)
add_library(Kaizo::Kaizo ALIAS KaizoLibrary)
