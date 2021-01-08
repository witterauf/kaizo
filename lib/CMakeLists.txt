set(KAIZO_BASE_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(KAIZO_INCLUDE_DIRECTORY "${KAIZO_BASE_INCLUDE_DIRECTORY}/kaizo")

###[ sources ]#####################################################################################

set(KAIZO_BINARY_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/binary/Binary.h
    ${KAIZO_INCLUDE_DIRECTORY}/binary/BinaryView.h
    ${KAIZO_INCLUDE_DIRECTORY}/binary/BinaryStream.h
    ${KAIZO_INCLUDE_DIRECTORY}/binary/BinaryPatch.h
    src/binary/Binary.cc
    src/binary/BinaryView.cc
    src/binary/BinaryStream.cc
    src/binary/BinaryPatch.cc
)

set(KAIZO_TEXT_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/text/Table.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableDecoder.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEncoder.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableMapper.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEntry.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TableEncoding.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/ShiftJis.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/TextEncoding.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/StringSet.h
    ${KAIZO_INCLUDE_DIRECTORY}/text/AsciiEncoding.h
    src/text/ShiftJis.cc
    src/text/ShiftJisToUnicode.h
    src/text/StringSet.cc
    src/text/AsciiEncoding.cc
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

set(KAIZO_ADDRESSES_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/Address.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/AddressFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/AddressMap.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/IdempotentAddressMap.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/RegionAddressMap.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/AbsoluteOffset.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/AddressLayout.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/RelativeOffsetLayout.h
    ${KAIZO_INCLUDE_DIRECTORY}/addresses/MipsLayout.h
    src/addresses/Address.cc
    src/addresses/AddressFormat.cc
    src/addresses/AddressMap.cc
    src/addresses/IdempotentAddressMap.cc
    src/addresses/RegionAddressMap.cc
    src/addresses/AbsoluteOffset.cc
    src/addresses/AddressLayout.cc
    src/addresses/RelativeOffsetLayout.cc
    src/addresses/MipsLayout.cc
)

set(KAIZO_DATA_REPRESENTATION_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/Data.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/NullData.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/IntegerData.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/ArrayData.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/RecordData.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/PointerData.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/StringData.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/BinaryData.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/data/ReferenceData.h
    src/data/data/Data.cc
    src/data/data/StringData.cc
    src/data/data/IntegerData.cc
    src/data/data/PointerData.cc
    src/data/data/RecordData.cc
    src/data/data/ArrayData.cc
    src/data/data/BinaryData.cc
    src/data/data/NullData.cc
    src/data/data/ReferenceData.cc
)

set(KAIZO_DATA_FORMATS_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/data/formats/DataFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/formats/ArrayFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/formats/IntegerFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/formats/RecordFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/formats/PointerFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/formats/StringFormat.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/formats/BinaryFormat.h
    src/data/formats/DataFormat.cc
    src/data/formats/StringFormat.cc
    src/data/formats/PointerFormat.cc
    src/data/formats/RecordFormat.cc
    src/data/formats/ArrayFormat.cc
    src/data/formats/IntegerFormat.cc
    src/data/formats/BinaryFormat.cc
    src/data/formats/FormatHelpers.h
    src/data/formats/FormatHelpers.cc
)

set(KAIZO_DATA_SERIALIZATION_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/data/serialization/Serialization.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/serialization/CsvSerialization.h
    src/data/serialization/CsvSerialization.cc
)

set(KAIZO_DATA_OBJECTS_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/data/objects/UnresolvedReference.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/objects/AnnotatedBinary.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/objects/Object.h
    src/data/objects/UnresolvedReference.cc
    src/data/objects/AnnotatedBinary.cc
    src/data/objects/Object.cc
)

set(KAIZO_DATA_LINKING_SOURCES
  ${KAIZO_INCLUDE_DIRECTORY}/data/linking/LinkObject.h
  ${KAIZO_INCLUDE_DIRECTORY}/data/linking/Constraint.h
  ${KAIZO_INCLUDE_DIRECTORY}/data/linking/FreeSpace.h
  ${KAIZO_INCLUDE_DIRECTORY}/data/linking/FreeBlock.h
  ${KAIZO_INCLUDE_DIRECTORY}/data/linking/Packer.h
  ${KAIZO_INCLUDE_DIRECTORY}/data/linking/Backtracker.h
  src/data/linking/LinkObject.cc
  src/data/linking/Constraint.cc
  src/data/linking/FreeSpace.cc
  src/data/linking/FreeBlock.cc
  src/data/linking/BackTracker.cc
)

set(KAIZO_DATA_BASE_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/data/DataReader.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/DataPath.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/DataAnnotation.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/DataWriter.h
    ${KAIZO_INCLUDE_DIRECTORY}/data/DataRangeTracker.h
    src/data/DataReader.cc
    src/data/DataPath.cc
    src/data/DataPathParser.h
    src/data/DataPathParser.cc
    src/data/DataWriter.cc
    src/data/DataRangeTracker.cc
)

set(KAIZO_DATA_SOURCES
  ${KAIZO_DATA_BASE_SOURCES}
  ${KAIZO_DATA_REPRESENTATION_SOURCES}
  ${KAIZO_DATA_FORMATS_SOURCES}
  ${KAIZO_DATA_OBJECTS_SOURCES}
  ${KAIZO_DATA_LINKING_SOURCES}
  ${KAIZO_DATA_SERIALIZATION_SOURCES}
)

set(KAIZO_UTILITIES_SOURCES
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/StringAlgorithms.h
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/StringCollection.h
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/CsvReader.h
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/DomReader.h
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/DomReaderHelpers.h
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/NarrowCast.h
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/Rectangle.h
    ${KAIZO_INCLUDE_DIRECTORY}/utilities/UsageMap.h
    src/utilities/CsvReader.cc
    src/utilities/DomReader.cc
    src/utilities/DomReaderHelpers.cc
    src/utilities/StringCollection.cc
    src/utilities/UsageMap.cc
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
  ${KAIZO_UTILITIES_SOURCES}
  ${KAIZO_ADDRESSES_SOURCES}
  ${KAIZO_DATA_SOURCES}
)

source_group("Binary" FILES ${KAIZO_BINARY_SOURCES})
source_group("VFS" FILES ${KAIZO_VFS_SOURCES})
source_group("Graphics" FILES ${KAIZO_GRAPHICS_SOURCES})
source_group("Systems" FILES ${KAIZO_SYSTEMS_SOURCES})
source_group("Text" FILES ${KAIZO_TEXT_SOURCES})
source_group("Addresses" FILES ${KAIZO_ADDRESSES_SOURCES})
source_group("Utilities" FILES ${KAIZO_UTILITIES_SOURCES})
source_group("Data" FILES ${KAIZO_DATA_BASE_SOURCES})
source_group("Data\\Representation" FILES ${KAIZO_DATA_REPRESENTATION_SOURCES})
source_group("Data\\Formats" FILES ${KAIZO_DATA_FORMATS_SOURCES})
source_group("Data\\Objects" FILES ${KAIZO_DATA_OBJECTS_SOURCES})
source_group("Data\\Linking" FILES ${KAIZO_DATA_LINKING_SOURCES})
source_group("Data\\Serialization" FILES ${KAIZO_DATA_SERIALIZATION_SOURCES})

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
        Contracts::Library
        LodePNG::LodePNG
)
target_compile_features(KaizoLibrary PUBLIC cxx_std_20)
target_all_warnings(KaizoLibrary)
target_warnings_are_errors(KaizoLibrary)
add_library(Kaizo::Kaizo ALIAS KaizoLibrary)
