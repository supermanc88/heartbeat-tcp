#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>


#include "hb_manually.h"


int main(int argc, char * argv[])
{
    int sfd;
    char buf[BUFSIZ];

    if (argc != 4)
        exit(1);

    char * ip = argv[1];
    char * port = argv[2];
    char * operation = argv[3];

    printf("ip = %s, port = %s, operation = %s\n", ip, port, operation);

    sfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(atoi(port));

    sendto(sfd, operation, strlen(operation), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    return 0;
}
