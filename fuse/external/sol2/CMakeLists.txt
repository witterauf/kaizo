add_library(sol2 INTERFACE)
target_include_directories(sol2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sol2 INTERFACE lua)