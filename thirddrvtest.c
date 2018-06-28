
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd,err;
	char msg[2];
	int arg = 0;
	fd = open("/dev/thirddrv", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
	}
	if(argc != 2)
		printf("Please input one number(1~3).\n");
	else if(0 == strcmp(argv[1],"1"))
		 {
			msg[0]=0x01;
			msg[1]=0x01;
			write(fd, msg, 1);
			read(fd, msg, 1);
			ioctl(fd, msg[1], &arg);
		 } 
	else if(0==strcmp(argv[1],"2"))
		 {
			msg[0]=0x02;
			msg[1]=0x02;
			write(fd, msg, 1);
			read(fd, msg, 1);
			ioctl(fd, msg[1], &arg);
		 }
	else if(0==strcmp(argv[1],"3"))
		 {
			msg[0]=0x03;
			msg[1]=0x03;
			write(fd, msg, 1);
			read(fd, msg, 1);
			ioctl(fd, msg[1], &arg);
		 }
	else
		printf("Wrong Number!\n");
	err=close(fd);
	if (err < 0)
	{
		printf("can't close!\n");
	}
	return 0;
}
