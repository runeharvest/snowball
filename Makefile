
init:
	mkdir -p build/bin/config
	mkdir -p build/bin/logs
	cp -r base/*.cfg build/bin/config

# valid options include: naming_service, login_server, welcome_service, position_service, chat_service, collision_service, frontend_service, snowballs_client
.PHONY: run-%
run-%:
	@mkdir -p build/bin/logs
	@mkdir -p build/bin/config
	@cd build/bin && ./$* -L logs/ -C config/ || exit 1

format:
	@find . -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.c' \) -print0 | xargs -0 clang-format -i