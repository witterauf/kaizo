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
    ${KAIZO_INCLUDE_DIRECTORY}/text/Table.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableDecoder.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEncoder.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableMapper.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEntry.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEncoding.h
    src/text/Table.cc
    src/text/TableDecoder.cc
    src/text/TableEncoder.cc
    src/text/TableMapper.cc
    src/text/TableEntry.cc
    src/text/TableEncoding.cc
    src/text/TableControlParser.h
    src/text/TableControlParser.cc
    src/text/TableParser.h
    src/text/TableParser.cc
)

set(KAIZO_GRAPHICS_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/ImageFileFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/Palette.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/Tile.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/TileConverter.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/TileFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/graphics/PixelFormat.h
    src/graphics/ImageFileFormat.cc
    src/graphics/PngFileFormat.h
    src/graphics/PngFileFormat.cc
    src/graphics/Tile.cc
    src/graphics/PixelFormat.cc
    src/graphics/TileConverter.cc
    src/graphics/TileFormat.cc
    src/graphics/Palette.cc
)

set(KAIZO_SYSTEMS_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/systems/psp/TextureSwizzle.h
    ${KAIZO_INCLUDE_DIRECTORY}/systems/psp/TileFormats.h
    src/systems/psp/TextureSwizzle.cc
    src/systems/psp/TileFormats.cc
)

set(KAIZO_VFS_SOURCES
  ${KAIZO_INCLUDE_DIRECTORY}/vfs/VirtualFileSystem.h
  ${KAIZO_INCLUDE_DIRECTORY}/vfs/BinarySource.h
  src/vfs/VirtualFileSystem.cc
  src/vfs/BinarySource.cc
)

set(KAIZO_SOURCES
    ${KAIZO_BINARY_SOURCES}
    ${KAIZO_VFS_SOURCES}
    ${KAIZO_TEXT_SOURCES}
    ${KAIZO_GRAPHICS_SOURCES}
    ${KAIZO_SYSTEMS_SOURCES}
)

source_group("Binary" FILES ${KAIZO_BINARY_SOURCES})
source_group("VFS" FILES ${KAIZO_VFS_SOURCES})
source_group("Graphics" FILES ${KAIZO_GRAPHICS_SOURCES})
source_group("Systems" FILES ${KAIZO_SYSTEMS_SOURCES})
source_group("Text" FILES ${KAIZO_TEXT_SOURCES})

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
        Contracts::Library
        LodePNG::LodePNG
    PUBLIC
        Fuse::Fuse
)
target_compile_features(KaizoLibrary PUBLIC cxx_std_20)
target_all_warnings(KaizoLibrary)
target_warnings_are_errors(KaizoLibrary)
add_library(Kaizo::Kaizo ALIAS KaizoLibrary)
