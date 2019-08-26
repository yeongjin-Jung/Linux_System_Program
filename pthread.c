#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>    // 스레드 사용하기 위한 헤더파일

//전역 변수 선언
int glob_var = 10;

#define DATACOUNT  4
#define DATASIZE sizeof(int)*DATACOUNT


//전연 변수 포인터

int *addr;


void* t_function(void* data)    //void* 주소값만 넘기겠다 , 
                                //void* data 주소값을 받겠다
{
    int argData;
    pthread_t t_id;
    argData = *((int*)data);
    t_id = pthread_self();
    glob_var +=3;
    printf("getpid =%d,t_id=%lu, argData = %d, glob_var = %d\n"
            ,getpid(), t_id, argData, glob_var);
    sleep(argData);
    for(int i=0; i<DATACOUNT;i++)
    {
        printf("%d", *(addr+i));
    }
    printf("\n");


    return (void*)(argData*argData);

}

int main(void)
{
    // POSIX thread id data type
    pthread_t p_thread[2];
    int ret;
    int a;
    int b;
    int status;

    //프로세스 아이디값
    printf("PID = %d\n", getpid());
    //
    addr= malloc(DATASIZE);
    //Main thread : Write data
    for (int i=0; i<DATACOUNT; i++)
    {

    *(addr+i) = i; // 메인 스레드에서 데이터를 쓰고 서브 스레드에서 데이터를 읽는다
    }

    //Thread 1
    ret = pthread_create(&p_thread[0],NULL,t_function, (void*)&a);
    if(ret <0) //pthread_create 정상적으로 수행되지 않은 경우
    {
        perror("Error: pthread_create() : thread1");
        return -1;
    }
    //Thread 2
    ret = pthread_create(&p_thread[1],NULL,t_function, (void*)&b);
    if(ret <0) //pthread_create 정상적으로 수행되지 않은 경우
    {
          perror("Error: pthread_create() : thread1");
          return -1;
    }


    pthread_join(p_thread[0],(void**)&status);
    printf("thread_join1:%d\n",status);

    pthread_join(p_thread[1],(void**)&status);
    printf("thread_join2:%d\n",status);


}


