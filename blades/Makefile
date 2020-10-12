test: tests
	./tests

tests: tests.cpp drive_logic.h
	g++ -O -g -std=c++11 -MD -MP -o tests tests.cpp -lm

-include *.d

