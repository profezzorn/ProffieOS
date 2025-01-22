# Tools for creating and modifying PQOI (*.pqf) files.

PQOI is a image/video/animation file format created specifically
for RGB565 images and video. In this directory, you'll find:

* cpqoi - creates pqoi files
* dpqoi - disassembles pqoi files
* pqoi.h - all the functions needed to add pqoi support in other programs

PQOI files genrally use a .pqf file extension.

## Installation and setup

These programs are written to be used from command lines and scripts in a POSIX-like environment.
If you're on Windows, I suggest installing a linux image to use them.

In addition to the code in this directory, you'll need:

* a compiler (to compile these tools)
* make
* ffmpeg
* mjpegtools
* netpbm

Just run "make" in this directory to compile cpqoi and dpqoi.

### A note about netpbm
The NetPBM that comes with ubuntu and debian is from 2002. It is missing many features and has many bugs.
If you are on Debian, Ubuntu or one of it's derivatives, install NetPBM from the NetPBM site, or these tools will not work.

# Creating your first PQOI file.

Just run:
```
./cpqoi someimage.png >someimage.pqf
```

This will create a PQOI file from the image, the resolution will be the same as the input.
If this is too big, we can specify a scaling pipeline, something like:

```
./cpqoi someinage.png "pamscale -xysize 128 80" >someimage.pqf
```

This will create a PQF with a maximum size of 128x80 pixels. Any pipeline of pam commands can be used for the scaling.
For single images, this might not seem very helpful, but scaling pipelines become more important later....

If we want to check the generated PQF file, we can do his:
```sh
mkdir tmpdir
cd tmpdir
../dpqoi ../someimage.pqf
```

This will create an image called "IMG00000.png" in the current directory which will contain the image from the PQF file.
Please note that this will NOT be identical to the original image, as the image data will have been converted to RGB565 and back.
dpqoi will also print out this:

```
file IMG00000.png
```

This is our first hint at PQOIML, the PQOI markup language. More about this below.

# Creating an animated PQOI file.

Just run:
```sh
./cpqoi somemovie.mp4 "pamscale -xysize 128 80" >animation.pqf
```

This will create an animated and looped PQOI file.
Animated PQF files are basically just concatenated PQF files, with a header which specifies the frame rate.

As before, we can also do the opposite:

```sh
mkdir tmpdir2
cd tmpdir2
../dpqoi ../animation.pqf
```

This time, many PNG images will be generated, and dpqoi will write out a lot more pqoiml:

```
label L0
fps 25:1
file IMG00000.png
file IMG00001.png
file IMG00002.png
file IMG00003.png
file IMG00004.png
.... etc.
goto L0
```

This pqoiml is a little more interesting. This uses "label" and "goto" to create a loop, it has several images, and it specifies a frame rate.
We can save the pqoiml text into a file like this:

```sh
../dpqoi ../animation.pqf >animation.pqoiml
```

If we give this pqoiml file as input to cpqoi, it will re-assemble the animation from it's components:

```sh
../cpqoi animation.pqoiml >animation.pqf
```

# PQOIML

The PQOI markup language is very simple, here are all the possible commands:

### fps A:B

Specifies the frame rate as a fraction, here are some examples:

```
fps 25:1  # 25 fps
fps 24000:1001 # common movie frame rate (23.976 fps)
fps 30000:1001 # NTSC frame rate (29.97 fps)
```

### file FILENAME

Imports the image or movie into the PQF.
Uses the scaling pipeline to scale each image.

### label LABEL

Generate a label. Labels don't actually exist in the output file, as GOTO and IF statements
just use positions in the file instead.

### goto LABEL

Generates an unconditional jump to the label.
Looped animations have goto to the beginning at the end.

### if COND goto LABEL

In pqoiml, if statements are very simple, they work just like a goto, but with
a condition. The conditions are application defined, but are always exactly
four characters long. The list of conditions supported by ProffieOS has not
yet been finalized, but this is what is supported so far:

* ison - do the jump if the saber is ignited
* lock - if we're in normal lockup mode
* drag - drag lockup mode
* melt - melt lockup mode
* A<30 - if A is less than 30
* A>30 - if A is greather than 30
* A=30 - if A is equal to 30

Note that "A" is a generic variable, and the meaning of A can be assigned at
runtime by using a .SCR file. There are 5 possible variables: A, B, C, D, E
and their values can be battery percent, volume, and other things.

### scaling SCALING PIPELINE

This sets the commands that scales the images. The scaling command is
not saved in the output file, it's only used when a "file" statement
is encountered. The scaling pipeline can also be specified on the command line,
which makes it possible to use the same pqoiml file to generate multiple
PQF files in different resolutions.

## An example pqoiml file:

This is a pqoiml file for a simple battery overlay:

```
scaling pamcut -pad 0 0 174 250 | pamflip -r180 | pamcut -pad 0 0 1280 640 | pamflip -r180 | pamscale -xysize 160 80

fps 1:2

label empty
file 0.png
label twenty
file 20.png
if A<10 goto blink
if A<40 goto twenty

label forty
file 40.png
if A<40 goto twenty
if A<60 goto forty

label sixty
file 60.png
if A<60 goto forty
if A<80 goto sixty

label eighty
file 80.png
if A<80 goto sixty

label full
if A<90 goto eighty
file 100.png
goto full
```
