file(GLOB SRC *.cpp *.h)

add_subdirectory(toml)

add_library(config STATIC ${SRC})

target_include_directories(config
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(config PRIVATE config_toml)
set_target_properties(config PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)
