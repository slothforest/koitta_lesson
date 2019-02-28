#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//mv 명령어
int main(int argc, char **argv)
{
	int nr;
	int sfd, dfd;
	char buf[256] = {0,};

	sfd = open(argv[1], O_RDONLY);
	dfd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);//trunc 다시쓰기

	lseek(dfd,0,SEEK_END);

	while(nr = read(sfd, buf, sizeof(buf)))
		write(dfd, buf, nr);

	close(sfd);
	close(dfd);
	
	unlink(argv[1]);
	return 0;
}
