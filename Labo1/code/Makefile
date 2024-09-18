CC= arm-buildroot-linux-gnueabihf-gcc
CFLAGS=-I. -pthread
objects = cpu_loop get_cpu_number priority
TARGET = cpu_loop
USER = root
HOSTNAME = 192.168.0.2

PRIO_EXAMPLE_NB_THREADS = 2

$(objects): %: %.c
	@echo "\033[0;36mCompiling $(TARGET)...\033[0m"
	$(CC) $(CFLAGS) -o $@ $<

deploy: $(TARGET)
	@echo "\033[0;36mDeploying $(TARGET)...\033[0m"
	scp $(TARGET) $(USER)@$(HOSTNAME):/root

run: deploy
	@echo "\033[0;36mRunning $(TARGET)  on remote...\033[0m"
ifeq ($(TARGET), priority)
	ssh $(USER)@$(HOSTNAME) "./$(TARGET) $(PRIO_EXAMPLE_NB_THREADS)"
else
	ssh $(USER)@$(HOSTNAME) "./$(TARGET)"
endif

clean:
	rm *.o $(objects)
