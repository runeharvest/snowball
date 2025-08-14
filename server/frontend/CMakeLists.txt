FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(frontend_service WIN32 ${SRC})

TARGET_LINK_LIBRARIES(frontend_service
						nelmisc
						nelnet)

NL_DEFAULT_PROPS(frontend_service "Snowballs, Services: Frontend Service")
NL_ADD_RUNTIME_FLAGS(frontend_service)

INSTALL(TARGETS frontend_service RUNTIME DESTINATION sbin COMPONENT services)
