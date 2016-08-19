CC = gcc  
LD = gcc  
CFLAGS = 
LDFLAGS = 
  
SRCS = $(wildcard *.c source/*.c)  
OBJS = $(patsubst %c, %o, $(SRCS))  
TARGET = datetimetest  
  
.PHONY: all clean  
  
all: $(TARGET)  
  
$(TARGET): $(OBJS)  
	$(LD) $(LDFLAGS) -o $@ $^
  
%o: %c
	$(CC) $(CFLAGS) -o $@ $<  
  
clean:  
	rm -f *.o $(TARGET)  

