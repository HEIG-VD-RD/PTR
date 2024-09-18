#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NB_MESURES 30

int main()
{
	struct timespec tp_res, tp_time;

	int res;

	//Formatting the csv file with the header
	printf("iteration : Temps en sec.nanosec : Résolution en sec.nanosec\n");

	if ((res = clock_getres(CLOCK_REALTIME, &tp_res)) == -1) {
		perror("clock_getres error");
		exit(EXIT_FAILURE);
	}
	//getres is used to know the resolution of the clock
	printf("Nan: Nan : %ld.%09ld\n", tp_res.tv_sec, tp_res.tv_nsec);

	for (int i = 0; i < NB_MESURES; ++i) {
		if (clock_gettime(CLOCK_REALTIME, &tp_time) == -1) {
			perror("clock_gettime error");
			exit(EXIT_FAILURE);
		}

		printf("%d: %ld.%09ld : Nan\n", i, tp_time.tv_sec, tp_time.tv_nsec);
	}
	return EXIT_SUCCESS;
}

/*int main(int argc, char **argv)
{
    struct timeval tv;
    int i;

    for (i = 0; i < NB_MESURES; ++i)
    {
        gettimeofday(&tv, NULL);
        printf("%2d : %ld.%06ld\n", i, tv.tv_sec, tv.tv_usec);
    }

    return EXIT_SUCCESS;
}*/
