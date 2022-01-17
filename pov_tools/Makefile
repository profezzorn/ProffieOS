ALL: image.h image_pgm.h image_8bit.h preview.png

IMAGE=1024px-Star_Wars_Logo.svg.png
OPTIONS=

image.h: pnmtorle pnmwindshieldwiper $(IMAGE)
	anytopnm $(IMAGE) | ./pnmwindshieldwiper $(OPTIONS) | pnmscale -reduce 10 | ./pnmtorle >image.h

image_pgm.h: pgmtorle pnmwindshieldwiper $(IMAGE)
	anytopnm $(IMAGE) | ./pnmwindshieldwiper  $(OPTIONS) | pnmscale -reduce 10 | ./pgmtorle >image_pgm.h

image_8bit.h: pnmquantizedtorle pnmwindshieldwiper $(IMAGE)
	anytopnm $(IMAGE) | ./pnmwindshieldwiper  $(OPTIONS) | pnmscale -reduce 10 | pnmquant 256 | ./pnmquantizedtorle >image_8bit.h

preview.png: pnmwindshieldwiper $(IMAGE)
	anytopnm $(IMAGE) | ./pnmwindshieldwiper  $(OPTIONS) | pnmscale -reduce 10 | pnmquant 256 | pnmtopng >preview.png

pnmtorle: pnmtorle.cc common.h rle.h
	g++ -O2 pnmtorle.cc -o pnmtorle -g -lm

pgmtorle: pgmtorle.cc common.h rle.h
	g++ -O2 pgmtorle.cc -o pgmtorle -g -lm

pnmquantizedtorle: pnmquantizedtorle.cc  common.h rle.h
	g++ -O2 pnmquantizedtorle.cc -o pnmquantizedtorle -g -lm

pnmwindshieldwiper: pnmwindshieldwiper.cc common.h
	g++ -O2 pnmwindshieldwiper.cc -o pnmwindshieldwiper -g -lm

1024px-Star_Wars_Logo.svg.png:
	wget https://upload.wikimedia.org/wikipedia/commons/thumb/6/6c/Star_Wars_Logo.svg/1024px-Star_Wars_Logo.svg.png


clean:
	rm preview.png image*.h
