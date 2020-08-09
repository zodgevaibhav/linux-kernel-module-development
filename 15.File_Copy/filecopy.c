#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

//Compile >> gcc filecopy.c -o filecopy
int main(int argc, char* argv[])
{

    FILE* fp;
    int fs, fd, cnt;
    char buf[512];
    if(argc !=3)
    {
        fprintf(stderr,"%s <src patrh> <dest path> ",argv[0]);
        _exit(1);
    }

    fs = open(argv[1],O_RDONLY);
    if(fs < 0 )
    {
        perror("Open file failed");
        _exit(2);
    }

    //Get mode of source file using fstat system call and use it in below instruction
    fd = open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,0644); 
    if(fd < 0 )
    {
        perror("Open failed for destination");
        _exit(2);
    }

    while((cnt = read(fs, buf, sizeof(buf)))>0)
        write(fd,buf,cnt);

    close(fd);
    close(fs);

    printf("File copy successful");

    return 0;

}