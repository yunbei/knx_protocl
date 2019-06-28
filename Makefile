CC      = gcc
CXX     = g++
LINK    = g++
# CFLAGS  = -g -Wall -O2
CFLAGS  = -g -Wall
TARGET  = knx_stack
SRCS    = $(wildcard *.cpp)
SRCS    += $(wildcard *.c)

CXX_OBJS    = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
C_OBJS  = $(patsubst %.c, %.o, $(wildcard *.c))

all:$(TARGET)

$(TARGET):$(CXX_OBJS) $(C_OBJS)
	$(LINK) $(CFLAGS) -o $@ $^

%.o:%.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY:clean
clean:
	rm -rf *.o $(TARGET) $(CXX_OBJS) $(C_OBJS)
