run:
	cd build/bin && ./client

init:
	@echo "Copying base files to build/bin..."
	@mkdir -p build/bin/logs
	@-unlink build/bin/cfg 2>/dev/null || true
	@cd build/bin && ln -s ../../base/cfg cfg
	@-unlink build/bin/data 2>/dev/null || true
	@cd base && unzip -qq -o data.zip
	@cd build/bin && ln -s ../../base/data data
	@echo "Done"



run-client: run

gdb:
	cd build/bin && gdb -q -ex run ./client

.PHONY: run-%
run-%:
	@mkdir -p build/bin/logs
	@printf '\033]2;$*\007'
	@cd build/bin && exec ./$*

gdb-%:
	@mkdir -p build/bin/logs
	@cd build/bin && gdb -q -ex run ./$* || exit 1


format:
	@find . -type f \( -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.c' \) -print0 | xargs -0 clang-format -i