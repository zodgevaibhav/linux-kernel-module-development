#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int fd;
	char buf[36];
	if(argc != 2) {
		fprintf(stderr, "device file path is missing.\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if(fd < 0) {
		perror("open() failed");
		exit(2);
	}

	// f_pos = 0
	strcpy(buf, "sunbeam infotech");
	write(fd, buf, strlen(buf));
	printf("data written: %s\n", buf);
	// f_pos = 16

	lseek(fd, -8L, SEEK_CUR);
	// f_pos = 8
	memset(buf, 0, sizeof(buf));
	read(fd, buf, 8); 
	// f_pos = 16
	printf("data read: %s\n", buf); // infotech

	strcpy(buf, " linux device drivers");
	write(fd, buf, strlen(buf));
	// f_pos = 32 (MAXLEN)
	printf("data written: %s\n", buf);
	
	lseek(fd, 0, SEEK_SET);
	// f_pos = 0
	memset(buf, 0, sizeof(buf));
	read(fd, buf, sizeof(buf)); 
	// f_pos = 32
	printf("data read: %s\n", buf); // sunbeam infotech linux device dr

	close(fd);
	return 0;
}

// terminal> gcc pchar_test.c -o pchar_test.out
// terminal> sudo ./pchar_test.out /dev/pchar