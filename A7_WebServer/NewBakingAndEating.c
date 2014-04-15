// Let's talk about this code (grades) 
// This original code had bugs and allows one to test one's 
// understanding about threads, mutexes, conditionals 
// see COMMENTS BELOW and experiment with this code. 
// 3/28/14

// Example demonstrates Mutex control over Pies with a
// Pie baker and a conditional 
// R. Trenary, 2/1/12
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 5
int Done [NUM_EATERS];     // A flag for each eater. AND them 
			// and one unfinished means we still have work

pthread_mutex_t MyMutex= PTHREAD_MUTEX_INITIALIZER; 
// A mutex and a cond (itional) 
pthread_cond_t PiesToEat= PTHREAD_COND_INITIALIZER;  
// Initialized 

int PieceOfPies; // Important shared variable

//==================================
//    BAKER THREAD
//==================================
void *PieBaking(void *threadid)  // THE BAKER
{int i, AllDone;
int rc ; // Return Code 

while (1)   // Forever until all eaters Done
{ AllDone=1; 
	for (i=0;i<NUM_EATERS;i++) AllDone = ( Done[i] && AllDone);  
				// Think about the above and logicals

	if  (AllDone) {printf("All Eaters are done \n"); break;}

	do {rc = pthread_mutex_trylock(&MyMutex); 		
	      } while (rc !=0);
// does this lock if successful ? 

	if (PieceOfPies == 0) {PieceOfPies +=2;
			printf("Baked a couple of of pies\n");
		} 
			// make a couple of pies if needed
			// and alert those who are waiting 
	
	pthread_cond_broadcast(&PiesToEat);
                   pthread_mutex_unlock(&MyMutex);
}
	printf("Done Baking\n");
 	pthread_exit(NULL);
}

// ==================================
//   EATER THREAD
// ==================================
void *PieEating(void *eaterid)
{
   long tid;
   int rc = -1; // Initialize to not acquired
   tid = (long)eaterid;
   int PiecesEaten = 0;	 // Control Pieces Eaten by eater
	Done[tid] = 0; // Not Done 
     while (PiecesEaten < 2)	
     {	
	while ( rc !=0 )
	{   rc = pthread_mutex_trylock(&MyMutex);  
				 // will return non zero if not acquired 	
	    usleep(100); // sleep and try again
	}	
	printf(" eater %lu acquired lock \n", tid);
	// pthread_mutex_lock(&MyMutex);	
	// THIS DOES NOT WORK -- TRY IT 
      // YOU NEED ABOVE LOOP. WHY ? (grade)

if (PieceOfPies == 0) (pthread_cond_wait(&PiesToEat, &MyMutex));
	// DOES cond_wait UNLOCK THE mutex while waiting ? (grades)
	// AND DOES IT LOCK WHEN cond_wait succeeds ? 	(grades)
	// THE ABOVE if DOES NOT WORK -- YOU NEED while 
 printf("Eating pie for Dennis.  It's me, eater #%ld!\n", tid);
		PieceOfPies--; // critical section 
	 pthread_mutex_unlock(&MyMutex);

	PiecesEaten++; // Ate a piece of pie 	
	// now sleep some of it off 
	usleep(100);
	rc = -1;	
	printf("%u available piece of pies \n",PieceOfPies);
	// THIS SHOULD BE UNSTABLE REPORT ? (grade) 
 }
	Done[tid] = 1; // All done 
	printf("eater %lu is done and said so \n",tid);	
         pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t eaters[NUM_EATERS];
   pthread_t PieBaker;
   int rc;
   long t;
	PieceOfPies = 0;
   for(t=0; t<NUM_EATERS; t++)
    {
      printf("In main: creating eater %lu\n", t);
      rc = pthread_create(&eaters[t], NULL, PieEating, (void *)t);
      if (rc){
         printf("ERROR; return code from pthread_create() for eaters is %d\n", rc);
         exit(-1); }
     }
    	rc = pthread_create(&PieBaker, NULL, PieBaking, (void *)t);
       	if (rc){
         	printf("ERROR; return code from pthread_create() for baker is %d\n", rc);
        	 exit(-1);}
     
	printf("looks like pies are done cooking\n");
   		/* Last thing that main() should do */
// ==> WE NEED TO JOIN THREADS BEFORE DESTROY <===
	pthread_mutex_destroy(&MyMutex);
	pthread_cond_destroy(&PiesToEat);
   	pthread_exit(NULL);
	printf("Done in main\n");
	exit(0);
}


