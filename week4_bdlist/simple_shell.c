#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 100 /* The maximum length command */

int main(void) {
  char *args[MAX_LEN / 2 + 1]; /* command line arguments */
  char *buf = malloc(sizeof(char) * MAX_LEN);
  char *p;
  pid_t pid;
  int i = 0;
  int should_run = 1;          /* flag to determine when to exit program */
  int background = 0;

  while (should_run) {
    printf("my_shell> ");
    /**
     * 표준입출력으로부터 문자열을 입력 받은 후:
     * (1) fork()를 통해 자식 프로세스를 생성
     * (2) 자식 프로세스가 execvp()를 호출하도록 할 것
     * (3) 만약 입력받은 문자에 &가 포함되어 있으면,
     *     부모 프로세스는 wait() 호출하지 않음
     */
    fgets(buf, MAX_LEN, stdin);
    if(buf[strlen(buf) - 1] == '\n') {
        buf[strlen(buf) - 1] = 0;
    }
    fflush(stdout);

    if (!strcmp(buf, "exit"))  {
        goto exit_tag;
    } else {
        if(buf[strlen(buf)-1] == '&') { background = 1; }
        else { background = 0; }

        i = 0;
        p = strtok(buf, " ");

        while (p != NULL && p != "&") {
            args[i++] = p;
            p = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (i > 0) {
            if ((pid = fork()) < 0) {
                fprintf(stderr, "Fork failed");
                return 1;
            } else if (pid == 0) {
                if (background) { args[i-1] = NULL; }
                execvp(args[0], args);
            } else {
                if(background) { printf("background process\n"); }
                else {
                    printf("Waiting for child, not a background process\n");
                    waitpid(pid, NULL, 0);
                    printf("Child Process complete\n");
                }
            }
        }
    }
  }
  exit_tag:
  return 0;
}
