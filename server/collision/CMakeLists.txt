FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(collision WIN32 ${SRC})

TARGET_LINK_LIBRARIES(collision
						nelmisc
						nelnet
						nelpacs
						nel3d)

NL_DEFAULT_PROPS(collision "Snowballs, Services: Collision")
NL_ADD_RUNTIME_FLAGS(collision)

SET_TARGET_PROPERTIES(collision PROPERTIES
  INSTALL_RPATH "$ORIGIN/lib:$ORIGIN/../lib"
  BUILD_RPATH   "$ORIGIN/lib:$ORIGIN/../lib")

INSTALL(TARGETS collision RUNTIME DESTINATION sbin COMPONENT services)
