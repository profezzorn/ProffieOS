test: tests
	./tests

tests: tests.cpp style_parser.h
	g++ -O -g -std=c++11 -MD -MP -o tests tests.cpp -lm

-include *.d

