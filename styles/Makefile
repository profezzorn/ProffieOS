test: tests
	./tests

tests: tests.cpp style_parser.h
	# clang++ -std=c++11 -g -fsanitize=address -fno-omit-frame-pointer -MD -MP -o tests tests.cpp -lm
	g++ -O -g -std=c++14 -MD -MP -o tests tests.cpp -lm

-include *.d

