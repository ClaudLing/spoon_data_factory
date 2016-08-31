INCS = $(addprefix -I, . inc) 
SRCS = $(wildcard *.c main/*.c src/*.c) 
OBJS = $(patsubst %c, %o, $(SRCS))  
TARGET = $(addsuffix .exe, $(addprefix exe/, $(basename $(notdir $(wildcard main/*.c)))))  

CC = gcc
LD = gcc
CFLAGS =
LDFLAGS =

.PHONY: all clean  
  
all: $(TARGET)
  
$(TARGET): $(OBJS)  
	@mkdir -p exe
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -o $(patsubst %c, %o, $<) -c $<

clean:  
	rm -f $(OBJS) $(TARGET)

