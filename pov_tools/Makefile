ALL: image_rgb.h image_pgm.h image_8bit.h preview.png

IMAGE=1024px-Star_Wars_Logo.svg.png
OPTIONS=

image_rgb.h: pnmtorle pnmwindshieldwiper $(IMAGE)
	anytopnm $(IMAGE) | ./pnmwindshieldwiper $(OPTIONS) | pnmscale -reduce 10 | ./pnmtorle >image_rgb.h

pov_renderer_rgb: pov_renderer.cc image_rgb.h  common.h rle.h
	g++ -O2 pov_renderer.cc -DIMAGE_FILE='"image_rgb.h"' -o pov_renderer_rgb -g -lm

preview_rgb.png: pov_renderer_rgb
	./pov_renderer_rgb | pnmtopng >preview_rgb.png


image_pgm.h: pgmtorle pnmwindshieldwiper $(IMAGE)
	anytopnm $(IMAGE) | ./pnmwindshieldwiper  $(OPTIONS) | pnmscale -reduce 10 | ./pgmtorle >image_pgm.h


pov_renderer_pgm: pov_renderer.cc image_pgm.h  common.h rle.h
	g++ -O2 pov_renderer.cc -DIMAGE_FILE='"image_pgm.h"' -o pov_renderer_pgm -g -lm

preview_pgm.png: pov_renderer_pgm
	./pov_renderer_pgm | pnmtopng >preview_pgm.png

image_8bit.h: pnmquantizedtorle pnmwindshieldwiper $(IMAGE)
	anytopnm $(IMAGE) | ./pnmwindshieldwiper  $(OPTIONS) | pnmscale -reduce 10 | pnmquant 256 | ./pnmquantizedtorle >image_8bit.h

pov_renderer_8bit: pov_renderer.cc image_8bit.h
	g++ -O2 pov_renderer.cc -DIMAGE_FILE='"image_8bit.h"' -o pov_renderer_8bit -g -lm

preview_8bit.png: pov_renderer_8bit common.h rle.h
	./pov_renderer_8bit | pnmtopng >preview_8bit.png

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
	rm preview.png || :
	for x in rgb 8bit pgm ; do rm image_$$x.h preview_$$x.png pov_renderer_$$x || : ; done
