DESTDIR=libxenomai
XENOCONFIG=$(DESTDIR)/usr/xenomai/bin/xeno-config
CC=$(shell DESTDIR=$(DESTDIR) $(XENOCONFIG) --cc)

CFLAGS=-std=c99 $(shell DESTDIR=$(DESTDIR) $(XENOCONFIG) --posix --alchemy --cflags) -Wall -Iutils -g 
LDFLAGS=$(shell DESTDIR=$(DESTDIR) $(XENOCONFIG) --posix --alchemy --ldflags --auto-init-solib) -g

# CFLAGS=-std=c99 -Wall
# LDFLAGS=

# LDLIBS+=-ljpeg

SRCDIR = .
UTILSDIR = utils

SOURCES = $(wildcard $(SRCDIR)/*.c)
UTILS_SOURCES = $(wildcard $(UTILSDIR)/*.c)

OBJECTS = $(SOURCES:.c=.o)
UTILS_OBJECTS = $(UTILS_SOURCES:.c=.o)

TARGET=xenomai_timer
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
