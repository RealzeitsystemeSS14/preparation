#include "file_writer.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STR_SIZE 100

int fd = -1;

static void openFile(char *file_name)
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
		exit(-1);
	}
	
	return 0;
}