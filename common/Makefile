test: tests
	./tests

tests: tests.cpp linked_ptr.h current_preset.h preset.h file_reader.h blade_config.h
	g++ -O -ggdb -std=c++11 -o tests tests.cpp -lm
