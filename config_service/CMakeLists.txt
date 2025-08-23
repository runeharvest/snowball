file(GLOB SRC *.cpp *.h)

add_subdirectory(config)


add_library(config_service STATIC ${SRC})
target_include_directories(config_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/config
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(config_service
    PRIVATE
        config
)
set_target_properties(config_service PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)
