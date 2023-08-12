CFLAGS= -pthread -MD -ggdb  -O2
CXXFLAGS=$(CFLAGS)
LDFLAGS=-lm -lpthread

COMMON=pqoiml.h pamstream.h stb_image_write.h
HOST_COMMON=
BINARIES=cpqoi dpqoi

all: $(BINARIES)

clean:
	rm $(BINARIES) *.o *.d

$(BINARIES) : % : %.o $(COMMON) $(HOST_COMMON)
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

-include *.d

