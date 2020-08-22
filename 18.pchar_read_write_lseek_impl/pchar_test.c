//user space program to test linux pchar driver

#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

int main(int argc, char * argv[])
{

int fd;
char buf[36];
    if(argc !=2)
    {
        fprintf(stderr, "device file path is missing");
        exeit(1);
    }

    fd = open(argv[1], O_RDWR);
    if(fd<0)
    {
        perror("Open file failed");
        exit(2);
    }

    strcpy(buf,"sunbeam infotech");
    write(fd,buf, strlen(buf));
    printf("1. Data return - %s", buf);

    lseek(fd, -8L, SEEK_CUR);

    memset(fd, 0, sizeof(buf));
    printf("2. Data return - %s", buf);

    read(fd, buf, 8);
    printf("3. Data return - %s", buf);

    write(fd, "Linux device driver", strlen("Linux device driver"));
    printf("4. Data return - %s", buf);

    lseek(fd, 0, SEEK_CUR);
        memset(fd, 0, sizeof(buf));
    printf("5. Data return - %s", buf);

    read(fd, buf, sizeof(buf));
    printf("6. Data return - %s", buf);
    
    close(fd);
    return 0;
}

// terminal> gcc pchar_test.c -o pchar_test.out
// terminal> sudo ./pchar_test.out /dev/pchar