#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include "pchar_ioctl.h"

int main(int argc, char *argv[]) {
	int fd, ret;
	struct pchar_info info;
	if(argc != 3) {
		fprintf(stderr, "%s <operation> <device>\n", argv[0]);
		fprintf(stderr, "supported operations\n1. 'reset' fifo\n2. get fifo 'info'\n");
		exit(1);
	}

	fd = open(argv[2], O_RDWR);
	if(fd < 0) {
		perror("open() failed to open device");
		exit(2);
	}

	if(strcmp(argv[1], "reset") == 0) {
		ret = ioctl(fd, FIFO_CLEAR);
		if(ret != 0) {
			perror("failed to clear fifo");
			exit(3);
		}
		printf("fifo cleared.\n");
	} else if(strcmp(argv[1], "info") == 0) {
		ret = ioctl(fd, FIFO_GET_INFO, &info);
		if(ret != 0) {
			perror("failed to get fifo info");
			exit(3);
		}
		printf("fifo info: size=%d, len=%d, avail=%d.\n", info.size, info.len, info.avail);
	} else
		printf("unsupported operation!\n");
	return 0;
}

// terminal> gcc pchar_test.c -o pchar_test.out
// terminal> sudo ./pchar_test.out reset /dev/pchar0
// terminal> sudo ./pchar_test.out info /dev/pchar0
 
