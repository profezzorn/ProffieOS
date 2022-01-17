OUTPUT ?= $(wildcard *.png)

ALL: image.h image_pgm.h image_8bit.h preview.png

image.h: pnmtorle pnmwindshieldwiper *.png
	pngtopnm *.png | ./pnmwindshieldwiper | pnmscale -height $(HEIGHT) | ./pnmtorle > ../styles/$(basename $(OUTPUT))_$(HEIGHT)_FC_POV_data.h

image_pgm.h: pgmtorle pnmwindshieldwiper *.png
	pngtopnm *.png | ./pnmwindshieldwiper | pnmscale -height $(HEIGHT) | ./pgmtorle > ../styles/$(basename $(OUTPUT))_$(HEIGHT)_SC_POV_data.h

image_8bit.h: pnmquantizedtorle pnmwindshieldwiper *.png
	pngtopnm *.png | ./pnmwindshieldwiper | pnmscale -height $(HEIGHT) | pnmquant 256 | ./pnmquantizedtorle > ../styles/$(basename $(OUTPUT))_$(HEIGHT)_8b_POV_data.h

preview.png: pnmwindshieldwiper *.png
	pngtopnm *.png | ./pnmwindshieldwiper | pnmscale -height $(HEIGHT) | pnmtopng > $(basename $(OUTPUT))_$(HEIGHT)_preview.png

pnmtorle: pnmtorle.cc
	g++ -O2 pnmtorle.cc -o pnmtorle -lm

pgmtorle: pgmtorle.cc
	g++ -O2 pgmtorle.cc -o pgmtorle -lm

pnmquantizedtorle: pnmquantizedtorle.cc
	g++ -O2 pnmquantizedtorle.cc -o pnmquantizedtorle -lm

pnmwindshieldwiper: pnmwindshieldwiper.cc
	g++ -O2 pnmwindshieldwiper.cc -o pnmwindshieldwiper -lm


