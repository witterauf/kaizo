set(FUSE_BASE_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(FUSE_INCLUDE_DIRECTORY "${FUSE_BASE_INCLUDE_DIRECTORY}/fuse")

###[ sources ]#####################################################################################

set(FUSE_BASE_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/Binary.h
    ${FUSE_INCLUDE_DIRECTORY}/BinaryStream.h
    ${FUSE_INCLUDE_DIRECTORY}/BinaryView.h
    ${FUSE_INCLUDE_DIRECTORY}/BinaryPatch.h
    ${FUSE_INCLUDE_DIRECTORY}/Rectangle.h
    ${FUSE_INCLUDE_DIRECTORY}/UsageMap.h
    ${FUSE_INCLUDE_DIRECTORY}/LuaBaseLibrary.h
    ${FUSE_INCLUDE_DIRECTORY}/LuaFuseLibrary.h
    ${FUSE_INCLUDE_DIRECTORY}/Integers.h
    ${FUSE_INCLUDE_DIRECTORY}/FuseException.h
    Binary.cc
    BinaryStream.cc
    BinaryView.cc
    BinaryPatch.cc
    UsageMap.cc
    LuaBaseLibrary.cc
    LuaFuseLibrary.cc
    LuaIntegerLayoutLoader.h
    LuaIntegerLayoutLoader.cc
    Integers.cc
)

set(FUSE_ADDRESSES_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/addresses/Address.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AddressFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AddressMap.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/IdempotentAddressMap.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/RegionAddressMap.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AbsoluteOffset.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AddressStorageFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/RelativeStorageFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/AbsoluteStorageFormat.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/MipsEmbeddedLayout.h
    ${FUSE_INCLUDE_DIRECTORY}/addresses/LuaAddressLibrary.h
    addresses/Address.cc
    addresses/AddressFormat.cc
    addresses/AddressMap.cc
    addresses/IdempotentAddressMap.cc
    addresses/RegionAddressMap.cc
    addresses/LuaAddressLibrary.cc
    addresses/AbsoluteOffset.cc
    addresses/RelativeStorageFormat.cc
    addresses/AddressStorageFormat.cc
    addresses/MipsEmbeddedLayout.cc
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
    ${FUSE_INCLUDE_DIRECTORY}/binary/serialization/LuaSerialization.h
    binary/serialization/CsvSerialization.cc
    binary/serialization/LuaSerialization.cc
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
    ${FUSE_INCLUDE_DIRECTORY}/binary/LuaBinaryLibrary.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataWriter.h
    ${FUSE_INCLUDE_DIRECTORY}/binary/DataRangeTracker.h
    binary/DataReader.cc
    binary/DataPath.cc
    binary/DataPathParser.h
    binary/DataPathParser.cc
    binary/LuaBinaryLibrary.cc
    binary/DataWriter.cc
    binary/DataRangeTracker.cc
)

set(FUSE_BINARY_LOADERS_SOURCES
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

set(FUSE_LUA_SOURCES
    ${FUSE_INCLUDE_DIRECTORY}/lua/LuaLoader.h
    ${FUSE_INCLUDE_DIRECTORY}/lua/Utilities.h
    ${FUSE_INCLUDE_DIRECTORY}/lua/FuseLuaException.h
    ${FUSE_INCLUDE_DIRECTORY}/lua/LuaWriter.h
    ${FUSE_INCLUDE_DIRECTORY}/lua/LuaReader.h
    lua/LuaLoader.cc
    lua/LuaWriter.cc
    lua/LuaReader.cc
    lua/Utilities.cc
)

set(FUSE_SOURCES
    ${FUSE_BASE_SOURCES}
    ${FUSE_BINARY_SOURCES}
    ${FUSE_ADDRESSES_SOURCES}
    ${FUSE_BINARY_FORMATS_SOURCES}
    ${FUSE_BINARY_DATA_SOURCES}
    ${FUSE_BINARY_LOADERS_SOURCES}
    ${FUSE_BINARY_SERIALIZATION_SOURCES}
    ${FUSE_BINARY_OBJECTS_SOURCES}
    ${FUSE_TEXT_SOURCES}
    ${FUSE_UTILITIES_SOURCES}
    ${FUSE_LUA_SOURCES}
    ${FUSE_ELF_SOURCES}
)

source_group("Addresses" FILES ${FUSE_ADDRESSES_SOURCES})
source_group("Base" FILES ${FUSE_BASE_SOURCES})
source_group("Binary" FILES ${FUSE_BINARY_SOURCES})
source_group("Binary\\Loaders" FILES ${FUSE_BINARY_LOADERS_SOURCES})
source_group("Binary\\Data" FILES ${FUSE_BINARY_DATA_SOURCES})
source_group("Binary\\Formats" FILES ${FUSE_BINARY_FORMATS_SOURCES})
source_group("Binary\\Objects" FILES ${FUSE_BINARY_OBJECTS_SOURCES})
source_group("Binary\\Serialization" FILES ${FUSE_BINARY_SERIALIZATION_SOURCES})
source_group("Graphics" FILES ${FUSE_GRAPHICS_SOURCES})
source_group("Lua" FILES ${FUSE_LUA_SOURCES})
source_group("Systems" FILES ${FUSE_SYSTEMS_SOURCES})
source_group("Text" FILES ${FUSE_TEXT_SOURCES})
source_group("Utilities" FILES ${FUSE_UTILITIES_SOURCES})

###[ Lua sources ]#################################################################################

compile_lua_bytecode(PrettyPrint SOURCES PrettyPrint.lua)

add_collection_library(FuseLuaRuntime
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PrettyPrint.lua"
  WORKING_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}"
  NAMESPACE
    lua::fuse
  OUTPUT_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_sources(FuseLuaRuntime PRIVATE PrettyPrint.lua)

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
        Diagnostics::Diagnostics
        LodePNG::LodePNG
        sol2
        Lua::FileSystem
        FuseLuaRuntime
)
target_compile_features(FuseLibrary PUBLIC cxx_std_20)
target_all_warnings(FuseLibrary)
target_warnings_are_errors(FuseLibrary)
add_library(Fuse::Fuse ALIAS FuseLibrary)
