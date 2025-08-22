file(GLOB SRC *.cpp *.h)

add_library(storage_memory STATIC ${SRC})
target_include_directories(storage_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(storage_memory PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)