FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(client WIN32 ${SRC})

TARGET_LINK_LIBRARIES(client
						nelmisc
						nelpacs
						nelnet
						nel3d)

NL_DEFAULT_PROPS(client "Snowball Client")
NL_ADD_RUNTIME_FLAGS(client)

# If sound is enabled then add the definitions and link the libraries.
IF(WITH_SOUND)
  ADD_DEFINITIONS(-DSBCLIENT_WITH_SOUND)
  TARGET_LINK_LIBRARIES(client nelsound)
ENDIF()

TARGET_LINK_OPTIONS(client PRIVATE -static-libstdc++ -static-libgcc)

SET_TARGET_PROPERTIES(client PROPERTIES
  INSTALL_RPATH "$ORIGIN/lib:$ORIGIN/../lib"
  BUILD_RPATH   "$ORIGIN/lib:$ORIGIN/../lib")

INSTALL(TARGETS client RUNTIME DESTINATION ${NL_BIN_PREFIX} COMPONENT client)
