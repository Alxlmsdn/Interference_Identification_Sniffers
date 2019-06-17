TARGET=caches
SOURCES:=caches.c cclfsr.c cctimer.c 

OBJECTS ?= $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SOURCES) ) ) )

CC=gcc
CFLAGS=-std=c99 -mtune=native -O3 -funroll-loops -Wall
LD_LIBS=-lpthread -lm

# Build targets
all: $(TARGET)

$(TARGET): $(OBJECTS)
	if $(foreach source,$(OBJECTS),[ $(source) -nt $(TARGET) ] ||) 1; \
	then $(CC) $(CFLAGS) $^ -o $@ $(LD_LIBS); \
	fi

clean:
	-rm -f *.o *.d *.pyc $(TARGET)