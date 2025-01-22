test: tests test2
	./tests
	./test2
	./unlock_presets_ini.py

presets.ini: tests
	./tests

unlock: presets.ini
	./unlock_presets_ini.py

# clang++ -std=c++11 -O1 -g -fsanitize=address -fno-omit-frame-pointer -MD -MP -o tests tests.cpp -lm

tests: tests.cpp stdout.h Makefile
	g++ -O -ggdb -std=c++11 -MD -MP -o tests tests.cpp -lm

test2: test2.cpp stdout.h
	g++ -O -ggdb -std=c++11 -MD -MP -o test2 test2.cpp -lm

-include *.d
