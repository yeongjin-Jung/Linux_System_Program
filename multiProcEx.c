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
	pid_t pid;
	int endValue;
	long long result;
	int msqid;
	clock_t startTime, endTime;
	struct timeval UTCtime_s, UTCtime_e;
	Value value;
	
	if(argc<2)
	{
		printf("Argument Error : ./multiProc %s %s\n",argv[1], argv[2]);
		return 0;
	}

	endValue = atoi(argv[1]);
	printf("endValue=%d\n",endValue);

	procNum = atoi(argv[2]);
	printf("procNum=%d\n",procNum);

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
		        // create message queue
				if (( msqid = msgget( (key_t)1234, IPC_CREAT | 0666))==-1)
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
