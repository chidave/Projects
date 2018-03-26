#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define DONE 3


typedef struct {
    
    int num;
    
    int eatTimes;
    
    int total;
    
} Philosopher;

int numOfTimes;
sem_t *sema;
int *state;
sem_t mutex;
Philosopher **table;
int totalNum;
int numDone;

void* process_philospher(void* philosopher);
void pick_chopstick(Philosopher *phil);
void attemptToEat(Philosopher* phil);
void drop_chopstick(Philosopher *phil);

int main(int argc, char** argv) {
	
	if(argc != 3) {
		printf("Usage: %s numOfPhilosophers numOfEatTimes\n", argv[0]);
		return -1;
	}
	
	int i;
	int num = atoi(argv[1]);
	numOfTimes = atoi(argv[2]);
	totalNum = num;
	numDone = 0;
	//sem_t mutex;
	//sem_t sema[num];
	sema = malloc(sizeof(sem_t) * num);
	state = malloc(sizeof(int) * num);
	pthread_t thread_id[num];
	table = malloc(sizeof(Philosopher*) * num);
	
	for(i = 0; i < num; i++) {
		table[i] = malloc(sizeof(Philosopher));
		table[i]->num = i+1;
		table[i]->eatTimes = 0;
		table[i]->total = num;
		state[i] = THINKING;
	}
	
	//initialize semaphores
	for (i = 0; i < num; i++) {  //create semaphore for each philospher
        sem_init(&sema[i], 0, 0);
	}
	
	sem_init(&mutex, 0, 1);  //contols access to picking and replacing of chopsticks
	
	//create phiosopher processes
	for (i = 0; i < num; i++) {
        pthread_create(&thread_id[i], NULL, process_philospher, table[i]);
        printf("Philosopher %d thinking\n", i+1);
	}
	
	for (i = 0; i < num; i++) {
		pthread_join(thread_id[i], NULL);
	}
	
	return 0;
	
}

void* process_philospher(void* philosopher) {
	
	while(1) {
		
		Philosopher *phil = (Philosopher*)philosopher;
		int num = phil->num;
		num = num - 1;
		
		if(state[num] != DONE) {
			
			sleep(1);
			pick_chopstick(phil);
			sleep(0);
			drop_chopstick(phil);
		}
		
	}
	
}

void pick_chopstick(Philosopher *phil) {
	
	int num = phil->num;
	num = num - 1;
    sem_wait(&mutex);
 
    //change state to hungry
    if(state[num] != DONE) {
		state[num] = HUNGRY;
	}
 
    printf("Philosopher %d Hungry\n", num + 1);
 
    // eat if neighbours are not eating
    attemptToEat(phil);
 
    sem_post(&mutex);
 
    // if unable to eat wait to be signalled
    sem_wait(&sema[num]);
 
    sleep(1);
}

void drop_chopstick(Philosopher *phil) {
	
	int num = phil->num;
	num = num - 1;
	int leftNum = phil->total - 1;
	int left = (num + leftNum) % phil->total;
	int right = (num + 1) % phil->total;
    sem_wait(&mutex);
 
    //change state to thinking
    if(state[num] != DONE) {
		state[num] = THINKING;
	}
	
    printf("Philosopher %d thinking\n", num + 1);
 
    attemptToEat(table[left]);
    attemptToEat(table[right]);
 
    sem_post(&mutex);
}

void attemptToEat(Philosopher* phil) {
	
	int num = phil->num;
	num = num - 1;
	int leftNum = phil->total - 1;
	int left = (num + leftNum) % phil->total;
	int right = (num + 1) % phil->total;
	
    if (state[num] == HUNGRY && state[left] != EATING && state[right] != EATING  && state[num] != DONE) {
        //change state to eating
        state[num] = EATING;
 
        sleep(2);
 
        printf("Philosopher %d Eating\n", num + 1);
        phil->eatTimes++;
        if(phil->eatTimes == numOfTimes) {
			printf("Philosopher %d is done\n", num + 1);
			state[num] = DONE;
			numDone++;
			if(numDone == totalNum) {
				printf("Everyone is done eating\n");
				exit(0);
			}
		}
 
        // sem_post(&S[phnum]) has no effect
        // during pick
        // used to wake up hungry philosophers
        // during drop
        sem_post(&sema[num]);
    }
}

