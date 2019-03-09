CFLAGS = -Wall -g -O0 -std=gnu++14
CC = g++

source=$(wildcard *.cpp)

obj = $(source:.cpp=)

all: $(obj)

$(obj): %: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(obj)