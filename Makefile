.PHONY: debug release run clean

debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
	cmake --build build
	cp build/compile_commands.json .

release:
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
	cmake --build build
	cp build/compile_commands.json .

release-static:
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DENABLE_HOTRELOAD=OFF -S . -B build
	cmake --build build
	cp build/compile_commands.json .

run: debug
	LSAN_OPTIONS=suppressions=sanitizer_suppressions.txt ./build/runner

clean:
	rm -r build
