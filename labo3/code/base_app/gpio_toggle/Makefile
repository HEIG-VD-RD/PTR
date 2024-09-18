CC=arm-buildroot-linux-gnueabihf-gcc

CFLAGS=-std=gnu11 -Wall -I../utils -pthread  
LDFLAGS=-pthread  

LDLIBS+=-ljpeg

SRCDIR = .
UTILSDIR = ../utils

SOURCES = $(wildcard $(SRCDIR)/*.c)
UTILS_SOURCES = $(wildcard $(UTILSDIR)/*.c)

OBJECTS = $(SOURCES:.c=.o)
UTILS_OBJECTS = $(UTILS_SOURCES:.c=.o)

TARGET=gpio_toggle
USER=root
HOSTNAME=192.168.0.2

all: $(TARGET)

$(TARGET): $(OBJECTS) $(UTILS_OBJECTS)
	@echo "\033[0;36mCompiling $(TARGET)...\033[0m"
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS) 
	

%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) 

deploy: $(TARGET)
	@echo "\033[0;36mDeploying $(TARGET)...\033[0m"
	scp $(TARGET) $(USER)@$(HOSTNAME):/root

clean:
	rm -f $(OBJECTS) $(UTILS_OBJECTS) $(TARGET)

.PHONY: clean
