CC=arm-buildroot-linux-gnueabihf-gcc
CFLAGS=-I. -O2 -lrt
objects = gettimeofday gettimeofday2 signal_timer signal_timer2
TARGET = gettimeofday
USER = root
HOSTNAME = 192.168.0.2

samples = 1000
interval = 500

all: $(TARGET)

$(objects): %: %.c
	@echo "\033[0;36mCompiling $(TARGET)...\033[0m"
	$(CC) -o $@ $< $(CFLAGS)

deploy: $(TARGET)
	@echo "\033[0;36mDeploying $(TARGET)...\033[0m"
	scp $(TARGET) $(USER)@$(HOSTNAME):/root

clean:
	rm *.o $(objects)
