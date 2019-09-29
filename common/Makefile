test: tests
	./tests

tests: tests.cpp linked_ptr.h current_preset.h preset.h file_reader.h blade_config.h color.h fuse.h quat.h vec3.h
	g++ -O -ggdb -std=c++11 -o tests tests.cpp -lm
