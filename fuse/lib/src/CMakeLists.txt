set(FUSE_BASE_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(FUSE_INCLUDE_DIRECTORY "${FUSE_BASE_INCLUDE_DIRECTORY}/fuse")

###[ sources ]#####################################################################################

set(FUSE_BASE_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/Binary.h
    ${FUSE_INCLUDE_DIRECTORY}/Rectangle.h
    Binary.cc
)

set(FUSE_BINARY_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataReader.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/ArrayFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/IntegerFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/RecordFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/PointerFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/StringFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/Data.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/IntegerData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/ArrayData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/RecordData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/PointerData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/StringData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/BinaryFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/BinaryData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/LuaWriter.h
    binary/StringData.cc
    binary/IntegerData.cc
    binary/PointerData.cc
    binary/RecordData.cc
    binary/ArrayData.cc
    binary/Data.cc
    binary/DataFormat.cc
    binary/StringFormat.cc
    binary/PointerFormat.cc
    binary/RecordFormat.cc
    binary/ArrayFormat.cc
    binary/IntegerFormat.cc
    binary/DataReader.cc
    binary/BinaryFormat.cc
    binary/BinaryData.cc
    binary/LuaWriter.cc
)

set(FUSE_TEXT_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/text/Table.h
    ${FUSE_INCLUDE_DIRECTORY}/text/TableReader.h
    ${FUSE_INCLUDE_DIRECTORY}/text/TableDecoder.h
    text/Table.cc
    text/TableReader.cc
    text/TableParser.h
    text/TableParser.cc
    text/TableDecoder.cc
)

set(FUSE_GRAPHICS_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/graphics/font/Glyph.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/font/Font.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/font/FontWriter.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/image/Image.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/image/ImageFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/tiles/Palette.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/tiles/Tile.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/tiles/TileColorRemapper.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/tiles/TileFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/tiles/TileImageFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/tiles/TileList.h
    ${FUSE_INCLUDE_DIRECTORY}/graphics/tiles/TileTransformation.h
    graphics/font/Glyph.cc
    graphics/font/Font.cc
    graphics/font/FontWriter.cc
    graphics/image/Image.cc
    graphics/image/ImageFormat.cc
    graphics/image/PngFormat.h
    graphics/image/PngFormat.cc
    graphics/tiles/Palette.cc
    graphics/tiles/Tile.cc
    graphics/tiles/TileColorRemapper.cc
    graphics/tiles/TileFormat.cc
    graphics/tiles/TileImageFormat.cc
    graphics/tiles/TileList.cc
)

set(FUSE_SOURCES
    ${FUSE_BASE_SOURCES}
    ${FUSE_BINARY_SOURCES}
    ${FUSE_TEXT_SOURCES}
    ${FUSE_GRAPHICS_SOURCES}
)

source_group("Base" FILES ${FUSE_BASE_SOURCES})
source_group("Binary" FILES ${FUSE_BINARY_SOURCES})
source_group("Graphics" FILES ${FUSE_GRAPHICS_SOURCES})
source_group("Text" FILES ${FUSE_TEXT_SOURCES})

###[ library ]#####################################################################################

add_library(FuseLibrary
    ${FUSE_SOURCES}
)
target_include_directories(FuseLibrary
    PUBLIC
        $<BUILD_INTERFACE:${FUSE_BASE_INCLUDE_DIRECTORY}>
)
target_link_libraries(FuseLibrary
    PRIVATE
        Diagnostics::Diagnostics
        LodePNG::LodePNG
)
target_compile_features(FuseLibrary PUBLIC cxx_std_17)
target_all_warnings(FuseLibrary)
target_warnings_are_errors(FuseLibrary)
add_library(Fuse::Fuse ALIAS FuseLibrary)