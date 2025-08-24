file(GLOB SRC *.cpp *.h)

add_subdirectory(nel)
# add_subdirectory(sql)

add_library(message STATIC ${SRC})

target_include_directories(message
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(message PRIVATE message_nel)
set_target_properties(message PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)
