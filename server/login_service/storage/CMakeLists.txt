file(GLOB SRC *.cpp *.h)

add_subdirectory(memory)
# add_subdirectory(sql)

add_library(storage STATIC ${SRC})

target_include_directories(storage
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(storage PRIVATE storage_memory)
set_target_properties(storage PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)
