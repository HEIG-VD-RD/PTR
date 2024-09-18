#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>

int main()
{
    while (1)
    {
        int current_cpu = sched_getcpu();
        time_t timestamp = time(NULL);
        struct tm *timeInfos = localtime(&timestamp);
        printf(" [%02d:%02d:%02d] CPU %d\n", timeInfos->tm_hour, timeInfos->tm_min, timeInfos->tm_sec, current_cpu);
        sleep(1);
    }

    return 0;
}
