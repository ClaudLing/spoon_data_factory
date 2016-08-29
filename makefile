CC = gcc
LD = gcc
CFLAGS =
LDFLAGS =

SRCS = $(wildcard *.c main/*.c src/*.c) 
INCS = $(addprefix -I, . inc) 
OBJS = $(patsubst %c, %o, $(SRCS))  
TARGET = $(addsuffix .exe, $(addprefix exe/, $(basename $(notdir $(wildcard main/*.c)))))  

.PHONY: all clean  
  
all: $(TARGET)
  
$(TARGET): $(OBJS)  
	@mkdir -p exe
	$(LD) $(LDFLAGS) -o $@ $^
  
%o: %c
	$(CC) $(CFLAGS) $(INCS) -o $(patsubst %c, %o, $<) -c $<
  
clean:  
	rm -f $(OBJS) $(TARGET)

test:
	echo $(INCS)