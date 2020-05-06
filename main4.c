#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#define var 9;
int len;
union sigval val;

void my_signal_interrupt(int i, siginfo_t *info, void *extra){
    printf ("Parent got %d signal\n",i);
    len = info->si_value.sival_int;
}

void child0(int i, siginfo_t *info, void *extra){
    printf("Child 0 got %d signal\n",i);
    char *ptr_val = info->si_value.sival_ptr;
    len = strlen(ptr_val);
}

void child1(int i, siginfo_t *info, void *extra){
    printf("Child 1 got %d\n",i);
    len = info->si_value.sival_int;
}


void child2(int i) {
    printf("Child 2 got %d signal and still alive :D\n", i);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = my_signal_interrupt;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, 0);

    printf("Hello, User!\n");
    printf("Enter string\n");

    char str[256];
    gets(str);

    pid_t pids[50] = {1},par=0;
    int i;
    for (i=0; i<3; i++){
        pids[i] = fork();
        switch (pids[i]){
            case 0:
                //children code below
                if (i == 2){
                    struct sigaction sa2;
                    sa2.sa_handler = child2;
                    sigaction(SIGINT, &sa2, 0);
                    printf("Child %d| pid %d| parent %d\n",i,getpid(),getppid());
                }
                else if (i == 1){
                    struct sigaction sa1;
                    sa1.sa_handler = child1;
                    sa1.sa_flags = SA_SIGINFO;
                    sigaction(SIGUSR1, &sa1, 0);
                    printf("Child %d| pid %d| parent %d\n",i,getpid(),getppid());
                }
                else if (i == 0){
                    struct sigaction sa0;
                    sa0.sa_handler = child0;
                    sa0.sa_flags = SA_SIGINFO;
                    sigaction(SIGUSR1, &sa0, 0);
                    printf("Child %d| pid %d| parent %d\n",i,getpid(),getppid());
                }
                break;
            default:
                //parent code below
                if (i == 0){
                    sigset_t parset;
                    sigemptyset(&parset);
                    sigaddset(&parset, SIGTERM);
                    sigprocmask(SIG_BLOCK, &parset, 0);
                    val.sival_ptr = str;
                    //printf("%p\n",str);
                    par = getpid();
                }
                sleep(1);
                break;
        }
        //common code below
        if (getpid() != par) break;
    }
    if (getpid() != par){
        sleep(100);
    }

    //printf("%d here\n",getpid());
/*
    for(int i=0;i<3;i++){
        printf("%d\n",pids[i]);
    }
*/
    for (int stage = 0; stage < 3; stage++){
        switch (stage) {
            case 0:
                //send and recieve line from 0
                if (getpid() == par){
                    //parent
                    sigqueue(pids[0],SIGUSR1,val);
                    sleep(100);
                }
                else if (pids[0]==0) {
                    //child0
                    printf("Child 0 got str\n");
                    printf("Length is %d bytes.\n",len);
                    val.sival_int = len;
                    sigqueue(getppid(),SIGUSR1,val);
                    sleep(100);
                }
                break;
            case 1:
                //send number to 1
                if (getpid() == par){
                    //parent
                    int bytes = len - var;
                    val.sival_int = bytes;
                    sigqueue(pids[1],SIGUSR1,val);
                    sleep(10);
                    printf("Hey, you, you're finally awake...\n");
                } else if (pids[1]==0) {
                    //child1
                    pid_t pid;
                    printf("Child1 got number %d\n",len);
                    for (int i=0;i<len;i++){
                        pid = fork();
                        if (pid != 0) {
                            printf("Child 2.%d| pid %d| parent %d\n",i,pid,getpid());
                        }
                        else break;
                    }
                    sleep(100);
                }
                break;
            case 2:
                //kill everyone
                if (getpid() == par){
                    //parent
                    signal(SIGINT,SIG_IGN);
                    printf("Daddy kills everyone right now.\n");
                    kill(0,SIGINT);
                    while(wait(0) != -1)
                    {
                    }
                }
                break;
        }
    }
    return 0;
}

