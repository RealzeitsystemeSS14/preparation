#include "file_writer.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/time.h>

#define MICROSECONDS_PER_SECOND 1000000
#define NANOSECONDS_PER_SECOND 1000000000
#define RT_PRIORITY 99

typedef struct time_measure {
	int sleep_usec;
	int max_delay;
} time_measure;

// Prototypes
void set_sleep_time(int p_usec, struct timespec* p_timespec);
void setRealtimePrio();
int parse_args(int argc, char **argv);
int check_args();
struct timeval get_time_diff(struct timeval *begin, struct timeval *end);
struct timespec get_time_diff_precise(struct timespec *begin, struct timespec *end);
int get_time_diff_usec(struct timeval *begin, struct timeval *end);
double get_time_diff_usec_precise(struct timespec *begin, struct timespec *end);

// Options (getopt)
int sleep_usec = -1, min_usec = -1, max_usec = -1, step_usec = -1, loop_count = -1;
int realtime = 0, verbose = 0;
char out_file[100];

// Other variables
struct sched_param scheduler_options;

/*
 * Measurement function. Where the most interesting stuff happens.
 */
int doMeasurement()
{
	struct timespec sleep_time, remain_time;
	//struct timeval begin, end;
	struct timespec begin, end;
	int diff_time, delay, max_delay;
	int i, j = 0, data_length;
	time_measure *data;
	double diff_time_precise;

	data_length = ((max_usec - min_usec) / step_usec) + 1;
	data = malloc(sizeof(time_measure) * 10000);
	if (data == NULL) {
		perror("Error allocating memory.");
		return -1;
	}

	for (sleep_usec = min_usec; sleep_usec <= max_usec; sleep_usec += step_usec) {

		max_delay = 0;
		set_sleep_time(sleep_usec, &sleep_time);

		if (verbose) {
			printf("============================\n");
			printf("Starting measurement: Sleep time = %d usec, loops = %d\n", sleep_usec, loop_count);
			printf("============================\n");
			printf("tv_sec = %ld, tv_nsec = %ld\n", sleep_time.tv_sec, sleep_time.tv_nsec);
		}

		for (i = 0; i < loop_count; ++i) {
			int ret;

			//!!gettimeofday(&begin, NULL);
			clock_gettime(CLOCK_MONOTONIC, &begin);

			ret = clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, &remain_time);
			if (ret) {
				fprintf(stderr, "Sleep returned error: %d.\n", ret);
				return -1;
			}

			//!!gettimeofday(&end, NULL);
			clock_gettime(CLOCK_MONOTONIC, &end);

			diff_time_precise = get_time_diff_usec_precise(&begin, &end);
			//!!diff_time = get_time_diff_usec(&begin, &end);
			//!!!delay = abs(diff_time - sleep_usec);
			delay = abs((int) diff_time_precise - sleep_usec);
			if (delay > max_delay)
				max_delay = delay;

			if (verbose) {
				//!!printf("value: %d usec, delay: %d usec, raw_data: %d usec\n",
				//!!   diff_time, delay, diff_time);

				printf("value: %d usec, delay: %d usec, raw_data: %f usec\n",
				       (int) diff_time_precise, delay, diff_time_precise);
			}
		}

		if (verbose) {
			printf("============================\n");
			printf("Max delay = %d usec\n", max_delay);
			printf("============================\n");
		}

		data[j].sleep_usec = sleep_usec;
		data[j].max_delay = max_delay;
		++j;
	}

	for (i = 0; i < data_length; ++i)
		writeToFile(out_file, data[i].sleep_usec, data[i].max_delay);

	free(data);

	return 0;
}

/*
 * Time conversion functions.
 */
int get_time_diff_usec(struct timeval *begin, struct timeval *end)
{
	int time_in_usec = ((end->tv_sec * 1000000) + end->tv_usec) -
	                   ((begin->tv_sec * 1000000) + begin->tv_usec);

	return time_in_usec;
}

struct timeval get_time_diff(struct timeval *begin, struct timeval *end)
{
	struct timeval tmp;

	tmp.tv_sec = end->tv_sec - begin->tv_sec;
	tmp.tv_usec = end->tv_usec - begin->tv_usec;

	if (tmp.tv_usec < 0) {
		tmp.tv_sec--;
		tmp.tv_usec = MICROSECONDS_PER_SECOND + tmp.tv_usec;
	}

	return tmp;
}

double get_time_diff_usec_precise(struct timespec *begin, struct timespec *end)
{
	double end_usec = (end->tv_sec * 1000000.0) + (end->tv_nsec / 1000.0);
	double start_usec = (begin->tv_sec * 1000000.0) + (begin->tv_nsec / 1000.0);

	return end_usec - start_usec;
}

struct timespec get_time_diff_precise(struct timespec *begin, struct timespec *end)
{
	struct timespec tmp;

	tmp.tv_sec = end->tv_sec - begin->tv_sec;
	tmp.tv_nsec = end->tv_nsec - begin->tv_nsec;

	if (tmp.tv_nsec < 0) {
		tmp.tv_sec--;
		tmp.tv_nsec = NANOSECONDS_PER_SECOND + tmp.tv_nsec;
	}

	return tmp;
}

/*
 * Main and helper functions ...
 */
int main(int argc, char **argv)
{
	if (parse_args(argc, argv)) {
		perror("Parsing error.");
		return -1;
	}

	if (check_args()) {
		perror("Invalid arguments.");
		return -1;
	}

	if (realtime)
		setRealtimePrio();

	if (doMeasurement() == -1)
		perror("Measurement was not successful.\n");

	closeFile();
	return 0;
}

void set_sleep_time(int p_usec, struct timespec* p_timespec)
{
	p_timespec->tv_sec = p_usec / 1000000.0;
	p_timespec->tv_nsec = (p_usec % 1000000) * 1000.0;
}

void setRealtimePrio()
{
	scheduler_options.sched_priority = RT_PRIORITY;

	if (sched_setscheduler(0, SCHED_RR, &scheduler_options) == -1)
		perror("Could not set RT_prio");
}

int parse_args(int argc, char **argv)
{
	char c;
	int idx;

	struct option long_opts[] = {
		{"min", required_argument, 0, 'a'},
		{"max", required_argument, 0, 'b'},
		{"loop", required_argument, 0, 'c'},
		{"step", required_argument, 0, 'd'},
		{"out", required_argument, 0, 'e'},
		{"rt", no_argument, 0, 'f'},
		{"v", no_argument, 0, 'v'}
	};

	while (1) {
		c = getopt_long_only(argc, argv, "", long_opts, &idx);

		// 255 for pi
		if (c < 0 || c == 255)
			break;

		switch (c) {
			case 'a':
				min_usec = atoi(optarg);
				break;
			case 'b':
				max_usec = atoi(optarg);
				break;
			case 'c':
				loop_count = atoi(optarg);
				break;
			case 'd':
				step_usec = atoi(optarg);
				break;
			case 'e':
				strcpy(out_file, optarg);
				break;
			case 'f':
				realtime = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				break;
		}
	}

	return 0;
}

int check_args()
{
	int result = 0;

	if(min_usec < 0) {
		fprintf(stderr, "argument \'-min\' needs value >= 0\n");
		result = -1;
	}

	if(max_usec <= 0) {
		fprintf(stderr, "argument \'-max\' needs value > 0\n");
		result = -1;
	}

	if(loop_count <= 0) {
		fprintf(stderr, "argument \'-loop\' needs value > 0\n");
		result = -1;
	}

	if(step_usec <= 0) {
		fprintf(stderr, "argument \'-step\' needs value > 0\n");
		result = -1;
	}

	return result;
}
