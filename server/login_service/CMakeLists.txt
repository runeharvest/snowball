FILE(GLOB SRC *.cpp *.h)

ADD_SUBDIRECTORY(entity)
ADD_SUBDIRECTORY(storage)

ADD_LIBRARY(login_service STATIC ${SRC})
TARGET_INCLUDE_DIRECTORIES(login_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/entity
        ${CMAKE_CURRENT_SOURCE_DIR}/storage
)
TARGET_LINK_LIBRARIES(login_service
    PUBLIC
        login_entities
    PRIVATE
        storage
)