#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <cobalt/stdio.h>
#include <sys/mman.h>

#include <alchemy/task.h>
#include <alchemy/timer.h>

#include "io_utils.h"

#define PERIOD 1e7 // 10ms

int nbMeasures = 10;

RTIME diffsGPIO[10];
RTIME diff[10];
int value = 0x0;
volatile int count = 0;

void xenomai_timer(void *arg)
{
	RTIME now, previous;
	RTIME nowGPIO, previousGPIO;

	rt_task_set_periodic(NULL, TM_NOW, PERIOD);
	previous = rt_timer_read();

	while (count < nbMeasures) {
		rt_task_wait_period(NULL);

		value = value ? 0x0 : 0x2;
		previousGPIO = rt_timer_read();
		write_gpio_val(MMAP, 0, REG_LOW, value);
		nowGPIO = rt_timer_read();
		now = rt_timer_read();
		diff[count] = now - previous;
		diffsGPIO[count] = nowGPIO - previousGPIO;
		previous = now;

		count++;
	}
}

int main(int argc, char *argv[])
{
	RT_TASK timer;

	init_ioctl(0);
	write_gpio_en(MMAP, 0, REG_LOW, 0x2);
	mlockall(MCL_CURRENT | MCL_FUTURE);

	if (rt_task_create(&timer, "timer", 0, 99, T_JOINABLE) != 0) {
		rt_printf("Error creating task\n");
		exit(EXIT_FAILURE);
	}

	if (rt_task_start(&timer, &xenomai_timer, NULL) != 0) {
		rt_printf("Error starting task\n");
		exit(EXIT_FAILURE);
	}

	if (rt_task_join(&timer) != 0) {
		rt_printf("Error joining task\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < 10; i++) {
		rt_printf("Temps %.3f\n", diff[i] / 1000000.0);
	}
	for (int i = 0; i < 10; i++) {
		rt_printf("Temps GPIO %.3f\n", diffsGPIO[i] / 1000000.0);
	}
	write_gpio_val(MMAP, 0, REG_LOW, 0x0);
	rt_task_delete(&timer);

	munlockall();

	clear_ioctl();

	return EXIT_SUCCESS;
}
