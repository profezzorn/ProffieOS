test: tests
	./tests

tests: tests.cpp stdout.h
	g++ -O -ggdb -std=c++11 -MD -MP -o tests tests.cpp -lm

-include *.d
