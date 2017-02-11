#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

void sys_err(const char *info)
{
    perror(info);
    exit(1);
}
//创建守护进程
void daemonize(void)
{
    pid_t pid = fork();

    if(pid < 0)
    {
        sys_err("fork");
    }
    //父进程 直接退出
    else if(pid > 0)
    {
        exit(0);
    }
    //子进程 改变会话 变为守护进程 
    //1、改变工作目录
    //2、设置umask
    //3、重定向标准输入、输出、出错到/dev/null
    setsid();
    if(chdir("/") < 0)
    {
        sys_err("chdir");
    }

    umask(0);

    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
}

//业务函数
void write_to_temp(int fd)
{
    time_t current = 0;
    char buf[1024] = {0};
    const char *err_info = "ctime_r is error\n";
    current = time(NULL);
    if(NULL == ctime_r(&current, buf))
    {
        write(fd, err_info, strlen(err_info));
    }
    else
    {
        write(fd, buf, strlen(buf));
    }
}

int main(int argc, const char* argv[])
{
    daemonize();
    //打开文件
    int fd = open("/tmp/daemon.log", O_WRONLY | O_CREAT | O_APPEND, 0664);
    if(fd < 0)
    {
        sys_err("open");
    }
    //    const char *str = "file created";
    //    write(fd, str, strlen(str));
    int i = 10;
    while(i--)
    {
        write_to_temp(fd);
        sleep(10);
    }
    close(fd);

    return 0;
}
