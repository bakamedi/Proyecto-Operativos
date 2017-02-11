#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "generic.h"
#include "list.h"
#include "nodelist.h"
#include "queue.h"

int datoC = 6; /* recurso C */
int datoB = 0; /* recurso B */
char seq[6]="BABCAC"; /* paquete 1 */
char valC[1]="C";
char valB[1]="B";
char valA[1]="A";
NodeList *nodoC, *nodoB, *nodoA;
Queue *colaC, *colaB, *colaA;

int lectores = 0; /* numero de lectores, recurso*/
pthread_mutex_t mutexC; /* controla acceso al dato C */
pthread_mutex_t mutexB; /* controla acceso al dato B */
pthread_mutex_t mlectores; /* controla acceso de m lectores */
void Escritor(void);
void Lector(void);
void imprimirBuffer(Queue *cola);

int main(int argc, char *argv[]) {
 colaC = queueNew(); /*cola c*/
 colaB = queueNew(); /*cola b*/
 colaA = queueNew(); /*cola a*/
 pthread_t th1, th2, th3, th4;
 pthread_mutex_init(&mutexC, NULL);
 pthread_mutex_init(&mlectores, NULL);
 pthread_create(&th1, NULL, (void*)Lector, NULL);
 pthread_create(&th2, NULL, (void*)Escritor, NULL);
 pthread_create(&th3, NULL, (void*)Lector, NULL);
 //pthread_create(&th4, NULL, (void*)Escritor, NULL);
 pthread_create(&th5, NULL, (void*)Lector, NULL);
  
 pthread_join(th1, NULL);
 pthread_join(th2, NULL);
 pthread_join(th3, NULL);
 //pthread_join(th4, NULL);
 pthread_join(th5, NULL);
  
 pthread_mutex_destroy(&mutexC);
 pthread_mutex_destroy(&mlectores);
  
 exit(0);
}
 
/*
 * Código del escritor
 */
void Escritor(void) { 
 int i;
 pthread_mutex_lock(&mutexC);
  
 //datoC = datoC + 2; /* aumenta el dato*/
 //realizar funcion que reciba siempre un solo valor y lo escriba en la cola correspondiente
 for(i = 0; i < sizeof(seq); i++){  
  /*Escribe en C*/
  if(seq[i]==valC[0]){   
   nodoC = nodeListNew(seq[i]);
    if(nodoC!=NULL){
     nodoC = nodeListNew(seq[i]);
     queueEnqueue(colaC,nodoC);  
     //printf("Cola C: %c\n",(char)nodeListGetCont(nodoC));   
    }
   }
   /*Escribe en B*/
   if(seq[i]==valB[0]){
   nodoB = nodeListNew(seq[i]);
    if(nodoB!=NULL){
     nodoB = nodeListNew(seq[i]);
     queueEnqueue(colaB,nodoB);
     //printf("Cola B: %c\n",(char)nodeListGetCont(nodoB));
    }
   }
   /*Escribe en A*/
   if(seq[i]==valA[0]){
   nodoA = nodeListNew(seq[i]);
    if(nodoA!=NULL){
     nodoA = nodeListNew(seq[i]);
     queueEnqueue(colaA,nodoA);
     //printf("Cola A: %c\n",(char)nodeListGetCont(nodoA));
    }
   }
 }
  
 pthread_mutex_unlock(&mutexC);
 pthread_exit(0);
}
 
/*
 * Código del lector
 */
void Lector(void) { 
 pthread_mutex_lock(&mlectores);
 lectores++;
 if (lectores == 1) 
  pthread_mutex_lock(&mutexC);
 pthread_mutex_unlock(&mlectores);
  /*leer dato buffer C*/
  imprimirBuffer(colaC);
  imprimirBuffer(colaB);
  imprimirBuffer(colaA);
 pthread_mutex_lock(&mlectores);
 lectores--;
 if (lectores == 0) 
  pthread_mutex_unlock(&mutexC);
 pthread_mutex_unlock(&mlectores);
 pthread_exit(0);
}

/*
 * Imprimir Buffer
 */
void imprimirBuffer(Queue *cola){
  NodeList* i;
  printf("\nCOLA: ");
  for(i = cola->header;i !=NULL ;i = i->next){
    printf(" %c ", (char)(nodeListGetCont(i)));
  }
  printf("\n");
}
