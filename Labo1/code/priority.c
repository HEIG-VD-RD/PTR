#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdint.h>

#define EXECUTION_TIME 10 /* In seconds */

/* Barrier variable */
pthread_barrier_t barr;

void *f_thread(void *arg)
{
	//Wait for threads to be created
	pthread_barrier_wait(&barr);

	time_t start_time = time(NULL);
	uint64_t nb_iterations = 0;
	while (time(NULL) < (start_time + EXECUTION_TIME)) {
		++nb_iterations;
	}
	return (void *)nb_iterations;
}

int main(int argc, char **argv)
{
	/* ... */
	int nb_threads;
	int min_prio;
	int max_prio;
	int i;
	srand(time(NULL));

	/* Parse input */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s NB_THREADS\n", argv[0]);
		return EXIT_FAILURE;
	}
	nb_threads = strtoimax(argv[1], (char **)NULL, 10);
	if (nb_threads <= 0) {
		fprintf(stderr, "NB_THREADS must be > 0 (actual: %d)\n",
			nb_threads);
		return EXIT_FAILURE;
	}

	/* ... */

	/* Why did we add these following lines? */

	/* Set thread attributes necessary to use priorities */
	struct sched_param schedparam;
	pthread_attr_t thread_attr;
	schedparam.sched_priority = 1;
	pthread_attr_init(&thread_attr);
	pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
	if (pthread_attr_setschedparam(&thread_attr, &schedparam) != 0) {
		fprintf(stderr, "Could not set scheduler parameters!\n");
		return EXIT_FAILURE;
	}

	/* Get minimal and maximal priority values */
	min_prio = sched_get_priority_min(SCHED_FIFO);
	max_prio = sched_get_priority_max(SCHED_FIFO);
	max_prio -= min_prio;

	/* Initialize barrier */
	if (pthread_barrier_init(&barr, NULL, nb_threads)) {
		fprintf(stderr, "Could not initialize barrier!\n");
		return EXIT_FAILURE;
	}

	/* Set priorities and create threads */
	/* ... */
	pthread_t threads[nb_threads];
	int prio_value[nb_threads];

	for (i = 0; i < nb_threads; i++) {
		if (pthread_create(&threads[i], NULL, f_thread, NULL)) {
			fprintf(stderr, "Could not create thread %d!\n", i);
			return EXIT_FAILURE;
		}
		prio_value[i] = min_prio + (rand() % max_prio);
		pthread_setschedprio(threads[i], prio_value[i]);
	}

	/* Wait for the threads to complete and set the results */
	/* ... */
	uint64_t nb_iterations[nb_threads];
	uint64_t total_iterations = 0;

	for (i = 0; i < nb_threads; i++) {
		pthread_join(threads[i], &nb_iterations[i]);
		total_iterations += nb_iterations[i];
	}

	for (i = 0; i < nb_threads; ++i) {
		fprintf(stdout, "[%02d] %ld (%2.0f%%)\n", prio_value[i],
			nb_iterations[i],
			100.0 * nb_iterations[i] / total_iterations);
	}

	pthread_barrier_destroy(&barr);

	return EXIT_SUCCESS;
}
