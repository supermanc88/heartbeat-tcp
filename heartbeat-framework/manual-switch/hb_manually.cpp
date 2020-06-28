#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>


#include "hb_manually.h"


int main(int argc, char * argv[])
{

    int ret = 0;
    int fd;
    char buf[BUFSIZ];
    int n;

    ret = access(PIPE_NAME, F_OK);

    if(ret == 0) {
        printf("exist pipe file\n");
        system(RM_PIPE);
    }

    ret = mkfifo(PIPE_NAME, 0666);

    fd = open(PIPE_NAME, O_RDONLY | O_NONBLOCK);
    printf("fd = %d\n", fd);

    if (fd > 0) {
        n = read(fd, buf, BUFSIZ);

        printf("read %s, n = %d\n", buf, n);
    }

    int sfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(694);

    sfd = socket(AF_INET, SOCK_DGRAM, 0);

    bind(sfd, (struct sockaddr*) &server_addr, sizeof(server_addr));

    client_len = sizeof(client_addr);

    while (1) {
        bzero(buf, BUFSIZ);
        int n = recvfrom(sfd, buf, BUFSIZ, 0, (struct sockaddr *)&client_addr, &client_len);
        if (n == -1) {
            perror("recvfrom error");
        }

        printf("recvfrom buf = %s\n", buf);
    }


    return 0;
}
