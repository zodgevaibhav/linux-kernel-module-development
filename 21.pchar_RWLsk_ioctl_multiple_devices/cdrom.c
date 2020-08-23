#include<sys/ioctl.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<linux/cdrom.h>
#include<unistd.h>

#define CDROM_DEV "\\dev\\sr0"

int main(int argc, char* argv[])
{
	int ret;
	int fd = open(argv[1],O_RDONLY);
	if(fd<0)
	{
		perror("\n Open file failed");
		exit(1);
	}
	printf("\n File opened successfuly %s %d", argv[1], fd);
	ret = ioctl(fd,CDROMEJECT,0);
	if(ret!=0)
	{
		perror("\n CD ROM FAILED");
		exit(1);
	}
	close(fd);
}
