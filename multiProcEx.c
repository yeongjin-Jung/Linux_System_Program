#include <stdio.h>		 // printf()
#include <unistd.h>		 // fork()
#include <sys/types.h>   // pid_t
#include <stdlib.h>
#include <time.h>
#include <sys/time.h> 	 // getimeofday( ) 함수에서 사용
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>	 
#include <sys/msg.h>	 // msgget(), msgsnd(), msgrcv(), msgctl()


//msg.h 메시지 큐 = 두개(혹은 이상)프로세스들이 일반 시스템 메시지 큐의 접근을 통해 정보를 교환 할 수 있다.

typedef struct {
	long msgType;  
	long long resultValue;
} Value;


long long adder(int start, int end)
{
	int i;
	long long result=0;

	for(i = start ;i<=end; i++)
	{
		result += i;
	}

	return result;
}

/*
timeval 구조체
long tv_sec; 초
long tv_usec; 마이크로초
*/
//UTCtime = 
void disp_runtime(struct timeval UTCtime_s, struct timeval UTCtime_e)
{
	struct timeval UTCtime_r;
	if((UTCtime_e.tv_usec- UTCtime_s.tv_usec)<0)
	{
		UTCtime_r.tv_sec  = UTCtime_e.tv_sec - UTCtime_s.tv_sec - 1;
		UTCtime_r.tv_usec = 1000000 - UTCtime_e.tv_usec - UTCtime_s.tv_usec;
	}
	else
	{
		UTCtime_r.tv_sec = UTCtime_e.tv_sec - UTCtime_s.tv_sec;
		UTCtime_r.tv_usec = UTCtime_e.tv_usec - UTCtime_s.tv_usec;
	}
	printf("runtime : %ld sec %d\n", UTCtime_r.tv_sec, UTCtime_r.tv_usec);
}


int main(int argc, char *argv[])
{
	int procNum;
	pid_t pid;   //pid_t : 프로세스 번호(pid)를 저장하는 타입(t)라는 의미
	int endValue; 
	long long result;
	int msqid;
	clock_t startTime, endTime;
	struct timeval UTCtime_s, UTCtime_e;
	Value value; //Value 구조체 value 변수 선언
	
	if(argc<2)
	{
		printf("Argument Error : ./multiProc %s %s\n",argv[1], argv[2]);
		return 0;
	}

	endValue = atoi(argv[1]);      // atoi:문자 스트링을 정수로 변환
	printf("endValue=%d\n",endValue);   //최종 숫자값

	procNum = atoi(argv[2]);
	printf("procNum=%d\n",procNum);     //프로세스 갯수

	switch(procNum)
	{
		case 1: startTime = clock();
				//gettimeofday(&UTCtime_s, NULL);
					
				result = adder(1, endValue);
					
				endTime = clock();
				printf("result=%lld\n",result);
				//gettimeofday(&UTCtime_e, NULL);
				//disp_runtime(UTCtime_s, UTCtime_e);
				printf("clock() %lf\n",(double)(endTime-startTime)/CLOCKS_PER_SEC);
				break;
				
		case 2: startTime = clock();
		        // create message queue 새로운 메시지 큐 생성 
			// 메시지 큐 : 프로세스 또는 프로그램 인스턴스가 데이터를 서로 교환할때 사용하는 방법. 
				if (( msqid = msgget( (key_t)1234, IPC_CREAT | 0666))==-1) 
					//msqid : 메시지 큐 식별자 , key : 시스템에서 식별하는 메시지 큐 변호
					//IPC_CREAT : key에 해당하는 큐가 있다면 큐의 식별자를 반환하며, 없으면 생성합니다.
					//IPC_EXCL : key에 해당하는 큐가 없다면 생성하지만 있다면 -1을 반환하고 복귀합니다.
   				{
      				perror( "Error : msgget()");
      				return -1;
   				}
   				
   				// create child process
				pid=fork();		
				if(pid==-1)   	// 자식 프로세스 생성에 실패한 경우
					perror("pid error");
				else if(pid==0) // 자식 프로세스인 경우
				{
					value.resultValue = adder(1, endValue>>1);	
				    if (msgsnd( msqid, &value, sizeof(value) - sizeof(long), 0)==-1)
					    //msgsnd는 message queue로 데이터를 전송하는 함수
					    //
                    {
                        perror( "Error : msgsnd()");
                        return -2;
                    }	
				}
				else
				{	// 부모 프로세스인 경우
					result = adder((endValue>>1)+1, endValue);
					if(msgrcv( msqid, &value, sizeof(value) - sizeof(long), 0, 0)==-1)
                    {
                        perror( "Error : msgrcv()");
                        return -3;
                    }
                    printf("value.resultValue=%lld\n",value.resultValue);
                    result+=value.resultValue;
                    printf("result=%lld\n",result);
                }                
                endTime = clock();
                printf("clock() %lf\n",(double)(endTime-startTime)/CLOCKS_PER_SEC);
				break;	
				
		default:
				printf("parameter error\n");
	}

	return 0;
}
