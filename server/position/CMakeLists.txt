FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(position WIN32 ${SRC})

TARGET_LINK_LIBRARIES(position
						nelmisc
						nelnet)

NL_DEFAULT_PROPS(position "Snowballs, Services: Position Service")
NL_ADD_RUNTIME_FLAGS(position)

SET_TARGET_PROPERTIES(position PROPERTIES
  INSTALL_RPATH "$ORIGIN/lib:$ORIGIN/../lib"
  BUILD_RPATH   "$ORIGIN/lib:$ORIGIN/../lib")

INSTALL(TARGETS position RUNTIME DESTINATION sbin COMPONENT services)
