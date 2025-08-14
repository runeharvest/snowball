
init:
	@mkdir -p build/bin/cfg
	@mkdir -p build/bin/logs
	@cp -r base/*.cfg build/bin/cfg/
	@echo "Initialized"


run:
	cd build/bin && LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../lib ./client

run-client: run

run-gdb:
	cd build/bin && LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:../lib gdb -q -ex run ./client

# valid options include: naming_service, login_server, welcome_service, position_service, chat_service, collision_service, frontend_service, snowballs_client
.PHONY: run-%
run-%:
	@mkdir -p build/bin/logs
	@mkdir -p build/bin/config
	@cd build/bin && ./$* -L logs/ -C config/ || exit 1

format:
	@find . -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.c' \) -print0 | xargs -0 clang-format -i