file(GLOB SRC *.cpp *.h)

add_subdirectory(storage)


add_library(login_service STATIC ${SRC})
target_include_directories(login_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/storage
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(login_service
    PRIVATE
        storage
)
set_target_properties(login_service PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES)
