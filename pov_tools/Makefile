ALL: image.h image_pgm.h image_8bit.h preview.png

IMAGE=1024px-Star_Wars_Logo.svg.png

image.h: pnmtorle pnmwindshieldwiper $(IMAGE)
	pngtopnm $(IMAGE) | ./pnmwindshieldwiper | pnmscale -reduce 10 | ./pnmtorle >image.h

image_pgm.h: pgmtorle pnmwindshieldwiper $(IMAGE)
	pngtopnm $(IMAGE) | ./pnmwindshieldwiper | pnmscale -reduce 10 | ./pgmtorle >image_pgm.h

image_8bit.h: pnmquantizedtorle pnmwindshieldwiper $(IMAGE)
	pngtopnm $(IMAGE) | ./pnmwindshieldwiper | pnmscale -reduce 10 | pnmquant 256 | ./pnmquantizedtorle >image_8bit.h

preview.png: pnmwindshieldwiper $(IMAGE)
	pngtopnm $(IMAGE) | ./pnmwindshieldwiper | pnmscale -reduce 10 | pnmtopng >preview.png

pnmtorle: pnmtorle.cc
	g++ -O2 pnmtorle.cc -o pnmtorle -g -lm

pgmtorle: pgmtorle.cc
	g++ -O2 pgmtorle.cc -o pgmtorle -g -lm

pnmquantizedtorle: pnmquantizedtorle.cc
	g++ -O2 pnmquantizedtorle.cc -o pnmquantizedtorle -g -lm

pnmwindshieldwiper: pnmwindshieldwiper.cc
	g++ -O2 pnmwindshieldwiper.cc -o pnmwindshieldwiper -g -lm

1024px-Star_Wars_Logo.svg.png:
	wget https://upload.wikimedia.org/wikipedia/commons/thumb/6/6c/Star_Wars_Logo.svg/1024px-Star_Wars_Logo.svg.png


