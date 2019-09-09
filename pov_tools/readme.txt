Ths directory contains programs that can help create POV data from images.
They were written and tested on Linux, but should work fine on Macs as well.
There are three programs:

pnmwindshieldwiper
scales the image and bends it so that it will work on a swinging blade

pnmtorle
Converts a full-color image to hex data to be included in the code.

pgmtorle
Converts a single-color image to hex data to be included in the code.

In addition, there is a Makefile that shows how to use all of these
programs. In fact, if you run "make" in this directory, the makefile
will download a star wars logo and convert it to color data.
