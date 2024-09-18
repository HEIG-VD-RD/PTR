#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#define n_sec 3
#define pi 3.14f

int main(void)
{

    time_t start_time = time(NULL);
    // For addition
    int x = 0;

    // For multiplication and division
    // float x = 42.42f;

    uint32_t nb_iterations = 0;
    while (time(NULL) < (start_time + n_sec))
    {
        //  printf("start:time = %ld\n", start_time);

        // For addition
        x += 1;

        // For multiplication
        // x *= pi;

        // For division
        // x /= pi;

        ++nb_iterations;
    }
    printf("nb_iterations = %d\n", nb_iterations);

    return EXIT_SUCCESS;
}
