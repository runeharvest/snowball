FILE(GLOB SRC *.cpp *.h)

ADD_LIBRARY(storage_memory STATIC ${SRC})
TARGET_INCLUDE_DIRECTORIES(storage_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${CMAKE_CURRENT_SOURCE_DIR}/../../entity
)