set(FUSE_BASE_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(FUSE_INCLUDE_DIRECTORY "${FUSE_BASE_INCLUDE_DIRECTORY}/fuse")

###[ sources ]#####################################################################################

set(FUSE_BASE_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/Binary.h
    ${FUSE_INCLUDE_DIRECTORY}/BinaryView.h
    ${FUSE_INCLUDE_DIRECTORY}/BinaryPatch.h
    ${FUSE_INCLUDE_DIRECTORY}/Rectangle.h
    ${FUSE_INCLUDE_DIRECTORY}/UsageMap.h
    ${FUSE_INCLUDE_DIRECTORY}/Integers.h
    Binary.cc
    BinaryView.cc
    BinaryPatch.cc
    UsageMap.cc
    Integers.cc
)

set(FUSE_ADDRESSES_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/addresses/Address.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AddressFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AddressMap.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/IdempotentAddressMap.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/RegionAddressMap.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AbsoluteOffset.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AddressLayout.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/RelativeOffsetLayout.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/MipsLayout.h
    addresses/Address.cc
    addresses/AddressFormat.cc
    addresses/AddressMap.cc
    addresses/IdempotentAddressMap.cc
    addresses/RegionAddressMap.cc
    addresses/AbsoluteOffset.cc
    addresses/AddressLayout.cc
    addresses/RelativeOffsetLayout.cc
    addresses/MipsLayout.cc
)

set(FUSE_BINARY_DATA_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/Data.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/NullData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/IntegerData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/ArrayData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/RecordData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/PointerData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/StringData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/BinaryData.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/data/ReferenceData.h
    binary/data/Data.cc
    binary/data/StringData.cc
    binary/data/IntegerData.cc
    binary/data/PointerData.cc
    binary/data/RecordData.cc
    binary/data/ArrayData.cc
    binary/data/BinaryData.cc
    binary/data/NullData.cc
    binary/data/ReferenceData.cc
)

set(FUSE_BINARY_FORMATS_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/binary/formats/DataFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/formats/ArrayFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/formats/IntegerFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/formats/RecordFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/formats/PointerFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/formats/StringFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/formats/BinaryFormat.h
    binary/formats/DataFormat.cc
    binary/formats/StringFormat.cc
    binary/formats/PointerFormat.cc
    binary/formats/RecordFormat.cc
    binary/formats/ArrayFormat.cc
    binary/formats/IntegerFormat.cc
    binary/formats/BinaryFormat.cc
    # private implementation
    binary/formats/FormatHelpers.h
    binary/formats/FormatHelpers.cc
)

set(FUSE_BINARY_SERIALIZATION_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/binary/serialization/Serialization.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/serialization/CsvSerialization.h
    binary/serialization/CsvSerialization.cc
)

set(FUSE_BINARY_OBJECTS_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/binary/objects/UnresolvedReference.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/objects/AnnotatedBinary.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/objects/Object.h
    binary/objects/UnresolvedReference.cc
    binary/objects/AnnotatedBinary.cc
    binary/objects/Object.cc
)

set(FUSE_BINARY_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataReader.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataPath.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataAnnotation.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataWriter.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataRangeTracker.h
    binary/DataReader.cc
    binary/DataPath.cc
    binary/DataPathParser.h
    binary/DataPathParser.cc
    binary/DataWriter.cc
    binary/DataRangeTracker.cc
)

set(FUSE_TEXT_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/text/ShiftJis.h
    ${FUSE_INCLUDE_DIRECTORY}/text/TextEncoding.h
    ${FUSE_INCLUDE_DIRECTORY}/text/StringSet.h
    ${FUSE_INCLUDE_DIRECTORY}/text/AsciiEncoding.h
    text/ShiftJis.cc
    text/ShiftJisToUnicode.h
    text/StringSet.cc
    text/AsciiEncoding.cc
)

set(FUSE_LINKING_SOURCES
  ${FUSE_INCLUDE_DIRECTORY}/linking/LinkObject.h
  ${FUSE_INCLUDE_DIRECTORY}/linking/Constraint.h
  ${FUSE_INCLUDE_DIRECTORY}/linking/FreeSpace.h
  ${FUSE_INCLUDE_DIRECTORY}/linking/FreeBlock.h
  ${FUSE_INCLUDE_DIRECTORY}/linking/Packer.h
  ${FUSE_INCLUDE_DIRECTORY}/linking/Backtracker.h
  linking/LinkObject.cc
  linking/Constraint.cc
  linking/FreeSpace.cc
  linking/FreeBlock.cc
  linking/BackTracker.cc
)

set(FUSE_UTILITIES_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/utilities/StringAlgorithms.h
    ${FUSE_INCLUDE_DIRECTORY}/utilities/StringCollection.h
    ${FUSE_INCLUDE_DIRECTORY}/utilities/CsvReader.h
    ${FUSE_INCLUDE_DIRECTORY}/utilities/DomReader.h
    ${FUSE_INCLUDE_DIRECTORY}/utilities/DomReaderHelpers.h
    ${FUSE_INCLUDE_DIRECTORY}/utilities/NarrowCast.h
    utilities/CsvReader.cc
    utilities/DomReader.cc
    utilities/DomReaderHelpers.cc
    utilities/StringCollection.cc
)

set(FUSE_SOURCES
    ${FUSE_BASE_SOURCES}
    ${FUSE_BINARY_SOURCES}
    ${FUSE_ADDRESSES_SOURCES}
    ${FUSE_BINARY_FORMATS_SOURCES}
    ${FUSE_BINARY_DATA_SOURCES}
    ${FUSE_BINARY_SERIALIZATION_SOURCES}
    ${FUSE_BINARY_OBJECTS_SOURCES}
    ${FUSE_TEXT_SOURCES}
    ${FUSE_UTILITIES_SOURCES}
    ${FUSE_LINKING_SOURCES}
)

source_group("Addresses" FILES ${FUSE_ADDRESSES_SOURCES})
source_group("Base" FILES ${FUSE_BASE_SOURCES})
source_group("Binary" FILES ${FUSE_BINARY_SOURCES})
source_group("Binary\\Data" FILES ${FUSE_BINARY_DATA_SOURCES})
source_group("Binary\\Formats" FILES ${FUSE_BINARY_FORMATS_SOURCES})
source_group("Binary\\Objects" FILES ${FUSE_BINARY_OBJECTS_SOURCES})
source_group("Binary\\Serialization" FILES ${FUSE_BINARY_SERIALIZATION_SOURCES})
source_group("Graphics" FILES ${FUSE_GRAPHICS_SOURCES})
source_group("Systems" FILES ${FUSE_SYSTEMS_SOURCES})
source_group("Text" FILES ${FUSE_TEXT_SOURCES})
source_group("Linking" FILES ${FUSE_LINKING_SOURCES})
source_group("Utilities" FILES ${FUSE_UTILITIES_SOURCES})

###[ library ]#####################################################################################

add_library(FuseLibrary
    ${FUSE_SOURCES}
)
target_include_directories(FuseLibrary
    PUBLIC
        $<BUILD_INTERFACE:${FUSE_BASE_INCLUDE_DIRECTORY}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(FuseLibrary
    PRIVATE
        Contracts::Library
)
target_compile_features(FuseLibrary PUBLIC cxx_std_20)
target_all_warnings(FuseLibrary)
target_warnings_are_errors(FuseLibrary)
add_library(Fuse::Fuse ALIAS FuseLibrary)
