FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(collision_service WIN32 ${SRC})

TARGET_LINK_LIBRARIES(collision_service
						nelmisc
						nelnet
						nelpacs
						nel3d)

NL_DEFAULT_PROPS(collision_service "Snowballs, Services: Collision")
NL_ADD_RUNTIME_FLAGS(collision_service)

INSTALL(TARGETS collision_service RUNTIME DESTINATION sbin COMPONENT services)
