test: tests
	./tests
	./unlock_presets_ini.py

presets.ini: tests
	./tests

unlock: presets.ini
	./unlock_presets_ini.py

tests: tests.cpp stdout.h
	g++ -O -ggdb -std=c++11 -MD -MP -o tests tests.cpp -lm

-include *.d
