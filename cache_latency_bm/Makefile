TARGET=caches
SOURCES:=caches.c cclfsr.c ../common/cctimer.c ../common/threadaffinity.c

OBJECTS ?= $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SOURCES) ) ) )

CC=gcc
CFLAGS=-std=c99 -mtune=native -O3 -funroll-loops -I../common -Wall
LD_LIBS=-lpthread -lm

# Build targets
all: $(TARGET)

$(TARGET): $(OBJECTS)
	if $(foreach source,$(OBJECTS),[ $(source) -nt $(TARGET) ] ||) 1; \
	then $(CC) $(CFLAGS) $^ -o $@ $(LD_LIBS); \
	fi

clean:
	-rm -f *.o *.d *.pyc ../common/*.o ../common/*.pyc $(TARGET)