#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>

typedef int sem_t;
union semun {
 int val;
 struct semid_ds *buf;
 unsigned short *array;
} arg;
sem_t CreateSem(key_t key, int value)
{
	union semun sem;
	sem_t semid;
	sem.val = value;
	
	semid = semget(key,value,IPC_CREAT|0666);
	if (-1 == semid)
	{
		printf("create semaphore error\n");
		return -1;
	}
	
	semctl(semid,0,SETVAL,sem);
	
	return semid;
}
/*
struct sembuf{
	ushort sem_num;
	short  sem_op;
	short  sem_flg;
};
*/
void SetvalueSem(sem_t semid, int value)
{
	union semun sem;
	sem.val = value;
	
	semctl(semid,0,SETVAL,sem);
	
	return ;
}

int GetvalueSem(sem_t semid)
{
	union semun sem;
	
	return semctl(semid,0,GETVAL,sem);
	
	return sem.val;
}

void DestroySem(sem_t semid)
{
	union semun sem;
	sem.val = 0;
	
	semctl(semid,0,IPC_RMID,sem);
}

int Sem_P(sem_t semid)
{
	struct sembuf sops={0,+1,IPC_NOWAIT};
	
	return (semop(semid,&sops,1));
}

int Sem_V(sem_t semid)
{
	struct sembuf sops={0,-1,IPC_NOWAIT};
	
	return (semop(semid,&sops,1));
}

static char msg[]="你好，共享内存\n";

int main(void)
{
	key_t key;
	int semid,shmid;
	char i,*shms,*shmc;
	struct semid_ds buf;
	int value = 0;
	char buffer[80];
	pid_t p;
	
	key = ftok("/ipc/sem",'a');
	shmid = shmget(key,1024,IPC_CREAT|0604);
	
	semid = CreateSem(key,0);
	
	p = fork();
	if(p > 0)
	{
		/* 父进程 */
		/* 建立共享内存 */
		shms = (char *)shmat(shmid,0,0);
		
		memcpy(shms, msg, strlen(msg)+1);	
		sleep(10);
		Sem_P(semid);
		shmdt(shms);
				
		DestroySem(semid);
	}
	else if(p == 0)
	{
		shmc = (char *)shmat(shmid,0,0);
		Sem_V(semid);
		printf("共享内存的值为:%s\n",shmc);
		shmdt(shmc);
	}
	return 0;
//gcc -o shm shm.c -g
}
