CFLAGS= -pthread -MD -ggdb -O2
CXXFLAGS=$(CFLAGS)
LDFLAGS=-lm -lpthread

COMMON=
HOST_COMMON=
BINARIES=videotoblc

all: $(BINARIES)

clean:
	rm $(BINARIES) *.o *.d

$(BINARIES) : % : %.o $(COMMON) $(HOST_COMMON)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

spectrum.blc: videotoblc /home/hubbe/hack/teensy/chaosdisplay/video/the_spectrum_song.mp4
	./videotoblc /home/hubbe/hack/teensy/chaosdisplay/video/the_spectrum_song.mp4 >spectrum.blc

-include *.d

