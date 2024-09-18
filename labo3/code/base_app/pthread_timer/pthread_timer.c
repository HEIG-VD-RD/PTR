#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/time.h>

#include "io_utils.h"

#define _GNU_SOURCE

int value;
int count = 0;
int nbMeasures;
timer_t timer;
struct timespec currentTime, lastTime, lastTimeGPIO, currenTimeGPIO;
long diffGPIO, diff;
long *val, *valGPIO;


void *thread_timer(void *arg)
{
	write_gpio_en(MMAP, 0, REG_LOW, 0x2);
	struct timespec delay;
	delay.tv_sec = 0; // 0 seconds delay
	delay.tv_nsec = 10000000; // 10 milliseconds delay

	clock_gettime(CLOCK_REALTIME, &lastTime);
	while (count < nbMeasures) {
		clock_gettime(CLOCK_REALTIME, &currentTime);

		value = value ? 0x0 : 0x2;

		// Set the value of GPIO_0_D1
		clock_gettime(CLOCK_REALTIME, &lastTimeGPIO);
		write_gpio_val(MMAP, 0, REG_LOW, value);
		clock_gettime(CLOCK_REALTIME, &currenTimeGPIO);
		diffGPIO = currenTimeGPIO.tv_nsec - lastTimeGPIO.tv_nsec;

		diff = currentTime.tv_nsec - lastTime.tv_nsec;

		val[count] = diff;
		valGPIO[count] = diffGPIO;

		lastTime = currentTime;
		count++;
		nanosleep(&delay, NULL);
	}
	
}

int main(int argc, char *argv[])
{
	init_ioctl(0);
	/* Test the parameters */
	if (argc != 2) {
		fprintf(stderr, "Wrong parameter(s)\n");
		return EXIT_FAILURE;
	}

	/* Set the measures and duration */
	nbMeasures = strtoimax(argv[1], (char **)NULL, 10);

	if (nbMeasures <= 0) {
		fprintf(stderr,
			"Both the number of measures and the time (in us) must be > 0\n");
		return EXIT_FAILURE;
	}

	val = (long *)calloc(nbMeasures, sizeof(long));
	valGPIO = (long *)calloc(nbMeasures, sizeof(long));

	if (val == NULL) {
		perror("Tableau de valeur erreur");
		return EXIT_FAILURE;
	}

	if (valGPIO == NULL) {
		perror("Tableau de valeur GPIO erreur");

		return EXIT_FAILURE;
	}

	pthread_t thread;
	pthread_attr_t attr;
	struct sched_param schedparam;

	pthread_attr_init(&attr);
	int max_prio = sched_get_priority_max(SCHED_FIFO);
	if (max_prio == -1) {
		perror("sched_get_priority_max failed");
		return EXIT_FAILURE;
	}

	schedparam.sched_priority = max_prio;

	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	if (pthread_attr_setschedpolicy(&attr, SCHED_FIFO) != 0) {
		perror("Unable to set scheduling policy\n");
	}
	if (pthread_attr_setschedparam(&attr, &schedparam) != 0) {
		fprintf(stderr, "Could not set scheduler parameters!\n");
		return EXIT_FAILURE;
	}

	if (pthread_create(&thread, NULL, thread_timer, NULL) != 0) {
		perror("pthread_create error\n");
		return EXIT_FAILURE;
	}

	pthread_join(thread, NULL);

	for (int i = 0; i < nbMeasures; i++) {
		printf("Temps %.3f ms\n", val[i] / 1000000.0);
	}
	for (int i = 0; i < nbMeasures; i++) {
		printf("Temps GPIO %.3f ms\n", valGPIO[i] / 1000000.0);
	}
	write_gpio_val(MMAP, 0, REG_LOW, 0x0);
	free(val);
	free(valGPIO);

	return EXIT_SUCCESS;
}
