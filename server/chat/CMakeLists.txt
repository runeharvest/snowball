FILE(GLOB SRC *.cpp *.h)

ADD_EXECUTABLE(chat_service WIN32 ${SRC})
 
TARGET_LINK_LIBRARIES(chat_service
						nelmisc
						nelnet)

NL_DEFAULT_PROPS(chat_service "Snowballs, Services: Chat")
NL_ADD_RUNTIME_FLAGS(chat_service)

INSTALL(TARGETS chat_service RUNTIME DESTINATION sbin COMPONENT services)
