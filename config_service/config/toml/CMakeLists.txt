file(GLOB SRC *.cpp *.h)

include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

add_library(config_toml STATIC ${SRC})
target_include_directories(config_toml
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${CMAKE_SOURCE_DIR}/include
        ${tomlplusplus_SOURCE_DIR}/include
)
target_compile_definitions(config_toml PUBLIC TOML_EXCEPTIONS=0)


set_target_properties(config_toml PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)