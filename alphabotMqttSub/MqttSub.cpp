#include "Subscriber.h"
#include <pthread.h>

int main( int argc, char *argv[]) 
{
    MosquittoSub *ptrRobot = new MosquittoSub;

    pthread_t thread_id[2];

    if(pthread_create(&thread_id[0], NULL, &MosquittoSub::WrapperSubscribe, ptrRobot))
    {
        printf("FAILED......");
        exit(1);
    }

    if(pthread_create(&thread_id[1], NULL, &MosquittoSub::WrapperCommand, ptrRobot))
    {
        printf("FAILED......");
        exit(1);
    }
    pthread_join(thread_id[0],NULL);
    pthread_join(thread_id[1],NULL);

    printf("STOP ALL PROCESS\n");
    printf("EXIT PROGRAM....\n");
	return 0;
}