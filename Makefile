debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
	cmake --build build

release:
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
	cmake --build build

run:
	LSAN_OPTIONS=suppressions=sanitizer_suppressions.txt ./build/game

clean:
	rm -r build
