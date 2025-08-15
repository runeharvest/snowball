
init:
	@echo "Copying base files to build/bin..."
	@mkdir -p build/bin/cfg
	@mkdir -p build/bin/logs
	@mkdir -p build/bin/data
	@cp -r base/*.cfg build/bin/cfg/
	@unzip -qq -o base/data.zip -d build/bin
	@echo "Done"


run:
	cd build/bin && ./client

run-client: run

gdb:
	cd build/bin && gdb -q -ex run ./client

# valid options include: naming_service, login_server, welcome_service, position_service, chat_service, collision_service, frontend_service, snowballs_client
.PHONY: run-%
run-%:
	@mkdir -p build/bin/logs
	@mkdir -p build/bin/config
	@cd build/bin && ./$* || exit 1

gdb-%:
	@mkdir -p build/bin/logs
	@mkdir -p build/bin/config
	@cd build/bin && gdb -q -ex run ./$* || exit 1


format:
	@find . -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.c' \) -print0 | xargs -0 clang-format -i