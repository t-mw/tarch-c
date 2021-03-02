debug:
	cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
	cmake --build build

release:
	cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
	cmake --build build

clean:
	rm -r build
