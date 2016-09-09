CC = gcc
LD = gcc
CFLAGS =
LDFLAGS =-lz -L/usr/local/lib

OS_NAME = $(shell uname -o)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')
ifeq ($(LC_OS_NAME), cygwin)
	PLATFORM = x86_64
else
	PLATFORM = linux
endif


INCS = $(addprefix -I, . inc) 
SRCS = $(wildcard *.c main/*.c src/*.c) 
OBJS = $(patsubst %.c, %.o, $(SRCS))  
TARGET = $(addsuffix .exe, $(addprefix exe/$(PLATFORM)/, $(basename $(notdir $(wildcard main/*.c)))))  


.PHONY: all clean  
  
all: $(TARGET)
  
$(TARGET): $(OBJS)  
	@mkdir -p exe/x86_64
	@mkdir -p exe/linux
	$(LD) -o $@ $^ $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -o $(patsubst %c, %o, $<) -c $<

clean:  
	rm -f $(OBJS) $(TARGET)
	
	


