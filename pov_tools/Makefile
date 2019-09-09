ALL: image.h image_pgm.h

image.h: pnmtorle pnmwindshieldwiper 1024px-Star_Wars_Logo.svg.png
	pngtopnm 1024px-Star_Wars_Logo.svg.png | ./pnmwindshieldwiper | pnmscale -height 144 | ./pnmtorle >image.h

image_pgm.h: pgmtorle pnmwindshieldwiper 1024px-Star_Wars_Logo.svg.png
	pngtopnm 1024px-Star_Wars_Logo.svg.png | ./pnmwindshieldwiper | pnmscale -height 144 | ./pgmtorle >image_pgm.h

preview.png: pnmwindshieldwiper 1024px-Star_Wars_Logo.svg.png
	pngtopnm 1024px-Star_Wars_Logo.svg.png | ./pnmwindshieldwiper | pnmscale -height 144 | pnmtopng >preview.png

pnmtorle: pnmtorle.cc
	g++ -O2 pnmtorle.cc -o pnmtorle -g -lm

pgmtorle: pgmtorle.cc
	g++ -O2 pgmtorle.cc -o pgmtorle -g -lm

pnmwindshieldwiper: pnmwindshieldwiper.cc
	g++ -O2 pnmwindshieldwiper.cc -o pnmwindshieldwiper -g -lm

pnmtoblc: pnmtoblc.cc
	g++ -O2 pnmtoblc.cc -o pnmtoblc -g -lm

1024px-Star_Wars_Logo.svg.png:
	wget https://upload.wikimedia.org/wikipedia/commons/thumb/6/6c/Star_Wars_Logo.svg/1024px-Star_Wars_Logo.svg.png


