set(FUSE_LINK_BASE_INCLUDE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(FUSE_LINK_INCLUDE_DIRECTORY "${FUSE_LINK_BASE_INCLUDE_DIRECTORY}/fuse/link")

###[ sources ]#####################################################################################

set(FUSE_LINK_BASE_SOURCES
    ${FUSE_LINK_INCLUDE_DIRECTORY}/SegmentEncoding.h
    SegmentEncoding.cc
)

source_group("Base" FILES ${FUSE_LINK_BASE_SOURCES})

###[ library ]#####################################################################################

add_library(FuseLink
    ${FUSE_LINK_BASE_SOURCES}
)
target_include_directories(FuseLink
    PUBLIC
        $<BUILD_INTERFACE:${FUSE_LINK_BASE_INCLUDE_DIRECTORY}>
)
target_compile_features(FuseLink PUBLIC cxx_std_17)
target_all_warnings(FuseLink)
target_warnings_are_errors(FuseLink)
add_library(Fuse::Link ALIAS FuseLink)