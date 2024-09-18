#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include "io_utils.h"

#define NB_LOOP

bool running = true;

void *toggle_gpio(void *arg) {
    int value;
    struct timespec delay;
    struct timespec lastTime, currenTime;
    struct timespec lastTimeGPIO, currenTimeGPIO;
    long diff;
    long diffGPIO;

    delay.tv_sec = 0; // 0 seconds delay
    delay.tv_nsec = 10000000; // 10 milliseconds delay
    
    // Enable the GPIO_0_D1. It is the FPGA pin PIN_Y17 of the 2x20 GPIO 
    // Expansion Headers
    write_gpio_en(MMAP, 0, REG_LOW, 0x2);

    while (running) {
        // Toggle the value
        value = value ? 0x0 : 0x2;
        clock_gettime(CLOCK_REALTIME, &currenTime);
        clock_gettime(CLOCK_REALTIME, &lastTimeGPIO);
		
        // Set the value of GPIO_0_D1
        write_gpio_val(MMAP, 0, REG_LOW, value);
        clock_gettime(CLOCK_REALTIME, &currenTimeGPIO);
        diffGPIO = currenTimeGPIO.tv_nsec - lastTimeGPIO.tv_nsec;
        printf("GPIO %.3f ms\n", diffGPIO / 1000000.0 );

        diff = currenTime.tv_nsec - lastTime.tv_nsec;
        printf("time %.3f ms\n", diff / 1000000.0);

        nanosleep(&delay, NULL);
        lastTime = currenTime;
    }
    
    write_gpio_en(MMAP, 0, REG_LOW, 0x0);
    
    return NULL;
}

// Signal handler used to end the infinite loop
void sigint_handler(int signum) {
    printf("\nCtrl+C received. Exiting...\n");
    running = false;
}


int main(int argc, char *argv[]) { 
    // Setup of the ioctl 
    init_ioctl(0);

    // Register Ctrl+C handler
    signal(SIGINT, sigint_handler);

    // Thread setup to handle GPIO value toggle
    pthread_t thread;
    if (pthread_create(&thread, NULL, toggle_gpio, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    
    // Wait for the thread to end
    pthread_join(thread, NULL);

    printf("Goodbye!\n");

    return 0;
}
