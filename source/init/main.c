/**
 * 简单的命令行解释器
 *
 * 创建时间：2021年8月5日
 * 作者：李述铜
 * 联系邮箱: 527676163@qq.com
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>

#include "applib/lib_syscall.h"

int main(int argc, char **argv) {
  //  int buf_len = 255;
  // char *buf = (char *)malloc(buf_len);
  // int len = 0;

  // for (int i = 0; i < 10000; ++i) {
  //   FILE *from = fopen("file.c", "rb");
  //   FILE *to = fopen("test.c", "wb");

  //   while ((len = fread(buf, 1, buf_len, from)) > 0) {
  //     len = fwrite(buf, 1, len, to);
  //     if (len < 0) {
  //       printf("write file error\n");
  //     }
  //   }

  //   printf("copy cnt: %d\n", i + 1);
  //   unlink("test.c");
  //   fclose(from);
  //   fclose(to);
  // }

  int pid = -1;
  int i;
  for (i = 0; i < 10; ++i) {
    pid = fork();
    if (pid == 0) {
      break;
    }
  }

  while (1) {
    if (pid > 0) {
      printf("==================== this is parent pid = %d \t================\n",
             getpid());
      msleep(500);
    } else {
      printf("==================== this is child %d, pid = %d \t================\n",
             i, getpid());
      msleep(100);
    }
  }

  return 0;
}
