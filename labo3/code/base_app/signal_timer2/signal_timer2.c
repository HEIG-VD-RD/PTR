#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <string.h>

#include "io_utils.h"

int nbMeasures;
int count = 0;
timer_t timer;
long *val, *valGPIO;

int value;
// volatile cause apparently the programme never ends if not volatile
volatile int running = 1;
struct timespec currentTime, lastTime, lastTimeGPIO, currenTimeGPIO;
long diff, diffGPIO;

void handler_signal(int sig)
{
	clock_gettime(CLOCK_REALTIME, &currentTime);
	if (count < nbMeasures) {
		value = value ? 0x0 : 0x2;

		// Set the value of GPIO_0_D1
		clock_gettime(CLOCK_REALTIME, &lastTimeGPIO);
		write_gpio_val(MMAP, 0, REG_LOW, value);
		clock_gettime(CLOCK_REALTIME, &currenTimeGPIO);
		diffGPIO = currenTimeGPIO.tv_nsec - lastTimeGPIO.tv_nsec;

		long diff = currentTime.tv_nsec - lastTime.tv_nsec;
		val[count] = diff;
		valGPIO[count] = diffGPIO;

		lastTime = currentTime;
		count++;
	} else {
		timer_delete(timer);
		running = 0;
	}
}

int main(int argc, char *argv[])
{
	init_ioctl(0);
	long usec = 10000; //10 millisecondes -> 10 000 microsecondes
	long nsec;

	struct sigaction sa;
	struct sigevent sev;
	struct itimerspec its;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handler_signal;
	sigaction(SIGRTMIN, &sa, NULL);

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = &timer;

	/* Test the parameters */
	if (argc != 2) {
		fprintf(stderr, "Wrong parameter(s)\n");
		return EXIT_FAILURE;
	}

	/* Set the measures and duration */
	nbMeasures = strtoimax(argv[1], (char **)NULL, 10);
	
	if (nbMeasures <= 0 ) {
		fprintf(stderr,
			"Both the number of measures and the time (in us) must be > 0\n");
		return EXIT_FAILURE;
	}

	val = (long *)calloc(nbMeasures, sizeof(long));
	valGPIO = (long *)calloc(nbMeasures, sizeof(long));

	if (val == NULL) {
		printf("Erreur d'allocation mémoire\n");
		return EXIT_FAILURE;
	}

	nsec = usec *
	       1000; // 10 millisecondes -> 10 000 microsecondes -> 10 000 000 nanosecondes

	if (timer_create(CLOCK_REALTIME, &sev, &timer) == -1) {
		perror("timer_create");
		return EXIT_FAILURE;
	}

	its.it_value.tv_sec = 0;
	its.it_value.tv_nsec = nsec;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	if (timer_settime(timer, 0, &its, NULL) == -1) {
		perror("timer_settime");
		return EXIT_FAILURE;
	}

	clock_gettime(CLOCK_REALTIME, &lastTime);
	write_gpio_en(MMAP, 0, REG_LOW, 0x2);
	while (running)
		;

	for (int i = 0; i < nbMeasures; i++) {
		printf("Temps %.3f ms\n", val[i] / 1000000.0);
	}
	for (int i = 0; i < nbMeasures; i++) {
		printf("Temps GPIO %.3f ms\n", valGPIO[i] / 1000000.0);
	}
	free(val);
	free(valGPIO);
	write_gpio_val(MMAP, 0, REG_LOW, 0x0);

	return EXIT_SUCCESS;
}
