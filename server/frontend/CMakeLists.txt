FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(frontend WIN32 ${SRC})

TARGET_LINK_LIBRARIES(frontend
						nelmisc
						nelnet)

NL_DEFAULT_PROPS(frontend "Snowballs, Services: Frontend Service")
NL_ADD_RUNTIME_FLAGS(frontend)

SET_TARGET_PROPERTIES(frontend PROPERTIES
  INSTALL_RPATH "$ORIGIN/lib:$ORIGIN/../lib"
  BUILD_RPATH   "$ORIGIN/lib:$ORIGIN/../lib")

INSTALL(TARGETS frontend RUNTIME DESTINATION sbin COMPONENT services)
