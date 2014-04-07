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

#define STR_SIZE 100
#define RT_PRIORITY 99

int fd = -1;

int sleep_usec = -1, min_usec = -1, max_usec = -1, step_usec = -1, loop_count = -1;
char out_file[500];
int realtime = 0;
struct sched_param schedParam;

void set_sleep_time(int p_usec, struct timespec* p_timespec);
double get_time_diff(struct timespec* const begin, struct timespec* const end);
int parse_args(int argc, char **argv);
int check_args();
void openFile(char *file_name);
void closeFile();
int writeToFile(char *file_name, int period, int delay);
void setRealtimePrio();

int main(int argc, char **argv)
{
	struct timespec sleep_time, remain_time, begin, end;
	int ret, i, delay, max_delay;
	double meassured_time;
	
	if(parse_args(argc, argv))
	{
		fprintf(stderr, "parsing error\n");
		return -1;	
	}
	
	if(check_args())
		return -1;
	if(realtime)
		setRealtimePrio();
	
	for(sleep_usec = min_usec; sleep_usec <= max_usec; sleep_usec += step_usec) {
		
		max_delay = 0;
		set_sleep_time(sleep_usec, &sleep_time);
	
		printf("starting meassurement: sleep time = %d usec, loops = %d\n", sleep_usec, loop_count);
		printf("============================\n");
		
		for(i = 0; i < loop_count; ++i)
		{
			clock_gettime(CLOCK_MONOTONIC, &begin);
			
			ret = clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep_time, &remain_time);
			if(ret)
			{
				fprintf(stderr, "Sleep returned error: %d\n", ret);
				return -1;
			}
			
			clock_gettime(CLOCK_MONOTONIC, &end);
			
			meassured_time = get_time_diff(&begin, &end);
			
			delay = abs((int) (meassured_time - sleep_usec));
			if(delay > max_delay)
				max_delay = delay;
			
			printf("value: %ld usec, delay: %d usec, raw_data: %.6f usec\n",
				   (long) meassured_time,
				   delay,
				   meassured_time);
		}
		
		printf("============================\n");
		printf("Max delay = %d usec\n", max_delay);
		printf("============================\n");
		
		writeToFile(out_file, sleep_usec, max_delay);
	}
	
	return 0;
}

void set_sleep_time(int p_usec, struct timespec* p_timespec)
{
	p_timespec->tv_sec = p_usec / 1000000;
	p_timespec->tv_nsec = (p_usec % 1000000) * 1000;
}

double get_time_diff(struct timespec* const begin, struct timespec* const end)
{
	return (double) (end->tv_sec - begin->tv_sec) * 1000000 + (double) (end->tv_nsec - begin->tv_nsec) / 1000;
}

int parse_args(int argc, char **argv)
{
	char c;
	struct option long_opts[] =
	{
		{"min", required_argument, 0, 'a'},
		{"max", required_argument, 0, 'b'},
		{"loop", required_argument, 0, 'c'},
		{"step", required_argument, 0, 'd'},
		{"out", required_argument, 0, 'e'},
		{"rt", no_argument, 0, 'f'}
		};
	int idx;
	
	while((c = getopt_long_only(argc, argv, "", long_opts, &idx)) != -1)
	{
		switch(c)
		{
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
			default:
				break;
		}
	}
	
	return 0;
}

int check_args()
{
	int result = 0;
	
	if(min_usec < 0)
	{
		fprintf(stderr, "argument \'-min\' needs value >= 0\n");
		result = -1;
	}
	
	if(max_usec <= 0)
	{
		fprintf(stderr, "argument \'-max\' needs value > 0\n");
		result = -1;
	}
	
	if(loop_count <= 0)
	{
		fprintf(stderr, "argument \'-loop\' needs value > 0\n");
		result = -1;
	}
	
	if(step_usec <= 0)
	{
		fprintf(stderr, "argument \'-step\' needs value > 0\n");
		result = -1;
	}
	
	return result;
}

void openFile(char *file_name)
{
	fd = open(file_name, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
	
	if (fd == -1)  {
		perror("Error opening the file.");
		exit(-1);
	}
}

void closeFile()
{
	if (close(fd) == -1) {
		perror("Error closing the file.");
		exit(-1);
	}
	fd = -1;
}

int writeToFile(char *file_name, int period, int delay)
{
	char str[STR_SIZE];
	
	if (fd == -1)
		openFile(file_name);
		
	sprintf(str, "%d %d\n", period, delay);
	
	if (write(fd, str, strlen(str)) == -1) {
		perror("Error writing in file.");
		return -1;
	}
	
	return 0;
}

void setRealtimePrio()
{
	schedParam.sched_priority = RT_PRIORITY;
	if(sched_setscheduler(0, SCHED_RR, &schedParam) == -1)
		perror("Could not set RT_prio");
}
