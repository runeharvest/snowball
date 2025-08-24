file(GLOB SRC *.cpp *.h)

add_subdirectory(message)

add_library(network_service STATIC ${SRC})
target_include_directories(network_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/message
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(network_service
    PRIVATE
        message
)
set_target_properties(network_service PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)
