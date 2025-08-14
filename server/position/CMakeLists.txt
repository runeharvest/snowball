FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(position_service WIN32 ${SRC})

TARGET_LINK_LIBRARIES(position_service
						nelmisc
						nelnet)

NL_DEFAULT_PROPS(position_service "Snowballs, Services: Position Service")
NL_ADD_RUNTIME_FLAGS(position_service)

INSTALL(TARGETS position_service RUNTIME DESTINATION sbin COMPONENT services)
