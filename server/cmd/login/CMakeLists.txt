FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(login WIN32 ${SRC})

INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})

TARGET_INCLUDE_DIRECTORIES(login
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/../../../login_service
		${CMAKE_CURRENT_SOURCE_DIR}/../../../login_service/storage
		${CMAKE_CURRENT_SOURCE_DIR}/../../../login_service/storage/memory
)

TARGET_LINK_LIBRARIES(login
						login_service
						${MYSQL_LIBRARIES}
						${ZLIB_LIBRARIES}
						nelmisc
						nelnet)

NL_DEFAULT_PROPS(login "NelNS, Services: Login Service")
NL_ADD_RUNTIME_FLAGS(login)


SET_TARGET_PROPERTIES(login PROPERTIES
  INSTALL_RPATH "$ORIGIN/lib:$ORIGIN/../lib"
  BUILD_RPATH   "$ORIGIN/lib:$ORIGIN/../lib")

INSTALL(TARGETS login RUNTIME DESTINATION sbin COMPONENT ls)
