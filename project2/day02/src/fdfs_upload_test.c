#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "make_log.h"

#define FDFS_TEST_MODULE "test"
#define FDFS_TEST_PROC  "fdfs"

#define MAX_FILE_LEN 1024

int main(int argc, char *argv[])
{
    int         ret = 0;
    int         pfd[2] = {0};
    pid_t       pid = 0;
    char        fileid[MAX_FILE_LEN] = {0};

    //创建管道
    ret = pipe(pfd);
    if (ret < 0)
    {
        perror("pipe error");
        exit(-1);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork error");
        exit(-1);
    }

    if (0 == pid)
    {
        //chlid
        //关闭读端
        close(pfd[0]);
        //重定向 stdout =====> pfd[1]
        dup2(pfd[1], STDOUT_FILENO);
        //exec ===> fdfs_upload_file
        execlp("fdfs_upload_file", "fdfs_upload_file", "./conf/client.conf", argv[1], NULL);
        perror("execlp fdfs_upload_file error");
    }
    else
    {
        //parent
        //关闭写端
        close(pfd[1]);
        //wait
        wait(NULL);
        //读取fileid
        read(pfd[0], fileid, sizeof(fileid));
        //写入日志
        LOG(FDFS_TEST_MODULE, FDFS_TEST_PROC, "filename = [%s], fileid=[%s]", argv[1], fileid);
    }

    return ret;
}
