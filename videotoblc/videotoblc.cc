#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include "matrix.h"
#include <map>
#include <math.h>
#include <deque>
#include <algorithm>

#define XSIZE 170
#define YSIZE 178

#define STR2(X) #X
#define STR(X) STR2(X)

const double squish_factor = 1.2375;

class YuvStream {
public:
  YuvStream(const std::string &filename) {
    cdiv = 2;
    setenv("FILE", filename.c_str(), 1);
    
    std::string cmd = "ffmpeg -i \"$FILE\" -an -f yuv4mpegpipe -";
    fprintf(stderr, "Executing: %s\n", cmd.c_str());
    f = popen(cmd.c_str(), "r");
    out = NULL;
    eof_reached = 0;
    read_header();
  }

  int c() {
    int c = getc(f);
    if (out) putc(c, out);
    return c;
  }

  int getnum() {
    int x = 0;
    while (1) {
      int y = c();
      y -= '0';
      if (y < 0 || y > 9) return x;
      x *= 10;
      x += y;
    }
  }

  void read_header() {
    int k;
    if((k=c()) != 'Y' ||
       (k=c()) != 'U' ||
       (k=c()) != 'V' ||
       (k=c()) != '4' ||
       (k=c()) != 'M' ||
       (k=c()) != 'P' ||
       (k=c()) != 'E' ||
       (k=c()) != 'G' ||
       (k=c()) != '2')
      goto headerr;

    while(1)
    {
      switch(c())
      {
	case ' ':
	  continue;
	case 'W':
	  xsize = getnum();
	  break;
	case 'H':
	  ysize = getnum();
	  break;
	case 'C':
	  cformat = getnum();
	  switch (cformat) {
	    case 444:
	      cdiv = 1;
	      break;
	  }
	  break;

	case '\n': return;
	case EOF: goto headeof;
	default:
	  while(1) {
	    switch(c()) {
	      case '\n':
		return;
	      case ' ':
		break;
	      case EOF:
		goto headeof;
	      default:
		continue;
	    }
	    break;
	  }
      }
    }

  headerr:
    perror("**while reading header");
    exit(1);

  headeof:
    fprintf(stderr,"**End of file in header\n");
    exit(1);
  }

  static float gamma(float x) {
    if (x < 0.0) x = 0.0;
    if (x > 1.0) x = 1.0;
    x = pow(x, 2.2);
    return x;
  }

  bool read_frame(Matrix<float> *r, Matrix<float> *g, Matrix<float> *b)
  {
    int uv_x = xsize / cdiv;
    int uv_y = ysize / cdiv;
    if (xsize != r->columns() || ysize != r->rows()) {
      *r = Matrix<float>(xsize, ysize);
      *g = Matrix<float>(xsize, ysize);
      *b = Matrix<float>(xsize, ysize);
    }
    Matrix<float> y_comp(xsize, ysize);
    Matrix<float> u_comp(uv_x, uv_y);
    Matrix<float> v_comp(uv_x, uv_y);
    
    int k;
    struct frame *ret;
    if(eof_reached) {
      fprintf(stderr, "AT EOF!\n");
      return false;
    }
    switch ((k=c())) {
      case 'F': break;
      case EOF: goto stream_end;
      default: goto frameerr;
    }
    if((k=c()) != 'R' ||
       (k=c()) != 'A' ||
       (k=c()) != 'M' ||
       (k=c()) != 'E')
      goto frameerr;

    while (1) {
      switch(k=c()) {
	case '\n': break;
	case EOF: goto frameeof;
	default: continue;
      }
      break;
    }
    for (int y = 0; y < ysize; y++) {
      for (int x = 0; x < xsize; x++) {
	y_comp.set(x, y, c());
      }
    }
    for (int y = 0; y < uv_y; y++) {
      for (int x = 0; x < uv_x; x++) {
	u_comp.set(x, y, c());
      }
    }
    for (int y = 0; y < uv_y; y++) {
      for (int x = 0; x < uv_x; x++) {
	v_comp.set(x, y, c());
      }
    }
    for (int y = 0; y < ysize; y++) {
      for (int x = 0; x < xsize; x++) {
	float Y = y_comp.get(x, y);
	int ux = x/cdiv;
	int uy = y/cdiv;
	if (ux == u_comp.columns()) ux--;
	if (uy == u_comp.rows()) uy--;
	float U = u_comp.get(ux, uy);
	float V = v_comp.get(ux, uy);
	float R = (1.164*(Y - 16) + 1.596*(V - 128)) / 256;
	float G = (1.164*(Y - 16) - 0.813*(V - 128) - 0.391*(U - 128)) / 256;
	float B = (1.164*(Y - 16) + 2.018*(U - 128)) / 256;


	r->set(x, y, R);
	g->set(x, y, G);
	b->set(x, y, B);
      }
    }
    return true;

  frameeof:
    fprintf(stderr,"**EOF while reading frame\n");
    return false;

  stream_end:
    fprintf(stderr,"**EOF!\n");
    return false;

  frameerr:
    fprintf(stderr,"**Unexpected data while reading frame: %c (%d)\n",k,k);
    for (int i = 0; i < 100; i++) {
      fprintf(stderr, "k = %c ", c());
    }
    return false;
  }

private:
  FILE *f;
  FILE *out;
  int xsize, ysize;
  int cformat;
  int cdiv;
  int eof_reached;
};

int scalecolor(float n) {
  int x = float(n * 256);
  if (x > 255) x = 255;
  if (x < 0) x = 0;
  return x;
}

int main(int argc, char **argv) {
  YuvStream stream(argv[1]);

  Matrix<float> r, g, b;
  std::vector<int> geometry;
  while (stream.read_frame(&r, &g, &b)) {
    for (int i = 0; i < 170; i++) {
      putchar(scalecolor(r.get(r.columns() * i / 170, r.rows() * i / 170)));
      putchar(scalecolor(g.get(g.columns() * i / 170, g.rows() * i / 170)));
      putchar(scalecolor(b.get(b.columns() * i / 170, b.rows() * i / 170)));
    }
    putchar(0);
    putchar(0);
  }
}
