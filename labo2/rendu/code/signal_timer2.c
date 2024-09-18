#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <inttypes.h>
#include <string.h>

int nbMeasures;
volatile int count = -1;
timer_t timer;
struct timespec lastTime;
long *val;

void handler_signal(int sig)
{
	struct timespec currentTime;
	clock_gettime(CLOCK_REALTIME, &currentTime);
	if (count <= nbMeasures) {
		if (lastTime.tv_sec != 0 ||
		    lastTime.tv_nsec !=
			    0) { // Si ce n'est pas la première mesure

			long diffSec = currentTime.tv_sec - lastTime.tv_sec;
			long diffNsec = currentTime.tv_nsec - lastTime.tv_nsec;
			long diff = diffSec * 1000000000 + diffNsec;

			val[count] = diff;
		}

		lastTime = currentTime;
		count++;
	} else {
		timer_delete(timer);
	}
}

int main(int argc, char *argv[])
{
	long usec;
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
	if (argc != 3) {
		fprintf(stderr, "Wrong parameter(s)\n");
		return EXIT_FAILURE;
	}

	/* Set the measures and duration */
	nbMeasures = strtoimax(argv[1], (char **)NULL, 10);
	usec = (long)strtol(argv[2], (char **)NULL, 10);

	if (nbMeasures <= 0 || usec <= 0) {
		fprintf(stderr,
			"Both the number of measures and the time (in us) must be > 0\n");
		return EXIT_FAILURE;
	}
	/* ... */
	val = (long *)calloc(nbMeasures, sizeof(long));

	if (val == NULL) {
		printf("Erreur d'allocation mémoire\n");
		return 1;
	}

	nsec = usec * 1000;

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

	while (count < nbMeasures)
		;

	for (int i = 0; i < nbMeasures; i++) {
		printf("%ld\n", val[i]);
	}
	free(val);

	return EXIT_SUCCESS;
}
