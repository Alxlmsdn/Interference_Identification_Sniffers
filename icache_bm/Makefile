TARGET=icache
SOURCES:=icache.c ../common/cctimer.c ../common/threadaffinity.c
DEPS := ../common/cctimer.h ../common/thread/affinity.h

OBJECTS ?= $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(patsubst %.cc,%.o,$(SOURCES) ) ) )

CC=gcc
CFLAGS=-std=c99 -mtune=native  -funroll-loops -I../common -Wall
LD_LIBS=-lpthread -lm

# Build targets
all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJECTS) functions.o
	#if $(foreach source,$(OBJECTS),[ $(source) -nt $(TARGET) ] ||) 1; \
	#then $(CC) $(CFLAGS) $^ -o $@ $(LD_LIBS); \
	#fi
	$(CC) $(CFLAGS) $^ -o $@ $(LD_LIBS)

functions: functions.s
	$(CC) -c $^ -o functions.o

.PHONY: clean
clean:
	-rm -f *.o *.d *.pyc ../common/*.o $(TARGET)