file(GLOB SRC *.cpp *.h)

add_library(message_nel STATIC ${SRC})
target_include_directories(message_nel
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(message_nel PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)