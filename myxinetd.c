#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT 9527

int main(int argc, char *argv[])
{
    int lfd, cfd;
    struct sockaddr_in addr_serv, addr_clie;
    socklen_t addrlen_clie;
    pid_t pid;

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_serv.sin_port = htons(PORT);
    bind(lfd, (struct sockaddr *)&addr_serv, sizeof(addr_serv));

    listen(lfd, 128);

    while(1)
    {
        addrlen_clie = sizeof(addr_clie);
        cfd = accept(AF_INET,(struct sockaddr *)&addr_clie, &addrlen_clie );

        //接受到链接请求创建多进程
        pid = fork();
        if(-1 == pid)       //fork出错
        {
            perror("fork");
            exit(1);            
        }
        else if(0 == pid)   //子进程
        {
            close(lfd);

            dup2(cfd, STDIN_FILENO);
            dup2(cfd, STDOUT_FILENO);
            execl("/home/itheima/test/httpd/project/xhttpd", "./xhttpd", "/home/itheima/test", NULL);
            perror("execl");
            exit(2);
        }
        else                //父进程
        {
            close(cfd);
        }
    }

    return 0;
}
