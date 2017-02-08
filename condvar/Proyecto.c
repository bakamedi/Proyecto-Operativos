#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "generic.h"
#include "list.h"
#include "nodelist.h"
#include "queue.h"


//variables globales para la suspencion de hilos
int globalContC;
int globalContB;
int globalContA;

int contA = 0;
int contB = 0;
int contC = 0;

pthread_mutex_t count_mutex;  //mutex for count increase critical section
pthread_cond_t count_threshold_cv; //condition variable

void *imprimePrioridadC(void *t);
void *imprimePrioridadB(void *t);
void *imprimePrioridadA(void *t);

NodeList *nodoCaracter;
Queue *colaPrioridad;

int main (int argc, char *argv[]){
  colaPrioridad = queueNew();
  char seq[6]="ABCCCC";//dato de entrada
  int i, rc, numHilos = 0,numHilosC = 0,numHilosB = 0,numHilosA = 0,numHilosTotal = 0;


  //llena la cola con los caracteres CBA
  for(i = 5; i >= 0 ; i --){
    nodoCaracter = nodeListNew(seq[i]);
    if(nodoCaracter!=NULL){
      nodoCaracter = nodeListNew(seq[i]);
      queueEnqueue(colaPrioridad,nodoCaracter);
      printf("  Datos agregado a la COLA : %c\n",(char) nodeListGetCont(nodoCaracter));
    }
  }

  printf("ingrese el numero de hilos\n");
  scanf("%i", &numHilos);

  numHilosC = numHilos;
  numHilosB = numHilos/2;
  numHilosA = numHilos/4;
  numHilosTotal = numHilosA+numHilosB+numHilosC;

  globalContC = numHilosC;
  globalContB = numHilosB;


  if(numHilosA == 0){
    numHilosA = 1;
    globalContA = numHilosA;
  }

  pthread_attr_t attr;
  pthread_t threadsCBA[numHilosTotal];
  /* Initialize mutex and condition variable objects */
  pthread_mutex_init(&count_mutex, NULL);
  pthread_cond_init (&count_threshold_cv, NULL);

  /* Create threads in a joinable state */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  
  for(i = 0 ; i < numHilosC ; i++){
    pthread_create(&threadsCBA[i], &attr, imprimePrioridadC, (void *) i);
    pthread_join(threadsCBA[i], NULL);
  }

  for(i = numHilosC ; i < numHilosC+numHilosB ;i++){
    pthread_create(&threadsCBA[i], &attr, imprimePrioridadB, (void *) i);
    pthread_join(threadsCBA[i], NULL);
  }

  for(i = numHilosC+numHilosB ; i < numHilosC+numHilosB+numHilosA ; i++){
    pthread_create(&threadsCBA[i], &attr, imprimePrioridadA, (void *) i);
    pthread_join(threadsCBA[i], NULL);
  }
  

  /* Clean up and exit */
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit(NULL);

}


//critic section
void *imprimePrioridadC(void *t){
  int id = (int) t;
  printf("\n-----------------------------------------------------------------\n");
  printf("HILO C , ID: %i\n", id);
  printf("veo el primer elemento de la cola es %c\n",(char)(nodeListGetCont(queuePeekFront(colaPrioridad))));
  sleep(2);
  printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
  imprimeCola(colaPrioridad);
  printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
  while (!queueIsEmpty(colaPrioridad)) {
    printf("%c  ---- contC =  %i\n",(char)(nodeListGetCont(queuePeekFront(colaPrioridad))),contC);
    sleep(2);
    if((char)(nodeListGetCont(queuePeekFront(colaPrioridad))) == 'C' && contC < globalContC){
      printf("%i\n",contC);
      contC++;
      pthread_mutex_lock(&count_mutex);
      nodoCaracter = queueDequeue(colaPrioridad);
      printf("Saco de la cola el elemento %c\n", (char)nodeListGetCont(nodoCaracter));
      imprimeCola(colaPrioridad);
      contC--;
      pthread_mutex_unlock(&count_mutex);
      break;
    }
  }
  printf("\n-----------------------------------------------------------------\n");
}


void *imprimePrioridadB(void *t){
  int id = (int) t;
  printf("\n-----------------------------------------------------------------\n");
  printf("HILO B , ID: %i\n", id);
  printf("veo el primer elemento de la cola es %c\n",(char)(nodeListGetCont(queuePeekFront(colaPrioridad))));
  sleep(2);
  printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
  imprimeCola(colaPrioridad);
  printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
  while (!queueIsEmpty(colaPrioridad)) {
    printf("%c  ---- contC =  %i\n",(char)(nodeListGetCont(queuePeekFront(colaPrioridad))),contC);
    sleep(2);
    if((char)(nodeListGetCont(queuePeekFront(colaPrioridad))) == 'C' && contC == 0){
      printf("%i\n",contC);
      contC++;
      pthread_mutex_lock(&count_mutex);
      nodoCaracter = queueDequeue(colaPrioridad);
      printf("Saco de la cola el elemento %c\n", (char)nodeListGetCont(nodoCaracter));
      imprimeCola(colaPrioridad);
      contC--;
      pthread_mutex_unlock(&count_mutex);
      break;
    }
  }
  printf("\n-----------------------------------------------------------------\n");
}


void *imprimePrioridadA(void *t){
  int id = (int) t;
  printf("\n-----------------------------------------------------------------\n");
  printf("HILO A , ID: %i\n", id);
  printf("veo el primer elemento de la cola es %c\n",(char)(nodeListGetCont(queuePeekFront(colaPrioridad))));
  sleep(2);
  while (!queueIsEmpty(colaPrioridad)) {
    printf("%c  ---- contC =  %i\n",(char)(nodeListGetCont(queuePeekFront(colaPrioridad))),contC);
    sleep(2);
    printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
    imprimeCola(colaPrioridad);
    printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
    if((char)(nodeListGetCont(queuePeekFront(colaPrioridad))) == 'A' && contB == 0){
      printf("%i\n",contC);
      contC++;
      pthread_mutex_lock(&count_mutex);
      nodoCaracter = queueDequeue(colaPrioridad);
      printf("Saco de la cola el elemento %c\n", (char)nodeListGetCont(nodoCaracter));
      printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
      imprimeCola(colaPrioridad);
      printf("EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
      contC--;
      pthread_mutex_unlock(&count_mutex);
      break;
    }
  }
  printf("\n-----------------------------------------------------------------\n");
}

void imprimeCola(Queue *cola){
  NodeList* i;
  printf("COLA: ");
  for(i = colaPrioridad->header;i !=NULL ;i = i->next){
    printf(" %c ", (char)(nodeListGetCont(i)));
  }
}