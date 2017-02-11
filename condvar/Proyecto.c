#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generic.h"
#include "list.h"
#include "nodelist.h"
#include "queue.h"

char seq[]="BABCAC"; /* paquete 1 */
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
void asignarPqte(char seq[], char val[], Queue *cola, NodeList *nodo,int tamPaquete);
int fn_Comparar(void const *a, void const *b);

int main(int argc, char *argv[]) {
  colaC = queueNew(); /*cola c*/
  colaB = queueNew(); /*cola b*/
  colaA = queueNew(); /*cola a*/
  
  //ordena la seq
  qsort(seq, strlen(seq), 1, fn_Comparar);
  printf("Cadena ordenada: \"%s\"\n", seq);
  
  pthread_t th1, thEscritor, th3, th4;
  pthread_mutex_init(&mutexC, NULL);
  pthread_mutex_init(&mlectores, NULL);
  pthread_create(&th1, NULL, (void*)Lector, NULL);
  pthread_create(&thEscritor, NULL, (void*)Escritor, NULL);
  //pthread_create(&th3, NULL, (void*)Lector, NULL);
  //pthread_create(&th4, NULL, (void*)Escritor, NULL);
  //pthread_create(&th5, NULL, (void*)Lector, NULL);
   
  pthread_join(th1, NULL);
  //pthread_join(th2, NULL);
  //pthread_join(th3, NULL);
  //pthread_join(th4, NULL);
  //pthread_join(th5, NULL);
   
  pthread_mutex_destroy(&mutexC);
  pthread_mutex_destroy(&mlectores);
   
  exit(0);
}
 
/*
 * Código del escritor
 */
void Escritor(void) { 
 pthread_mutex_lock(&mutexC);
  
 //realizar funcion que reciba siempre un solo valor y lo escriba en la cola correspondiente
 printf("%d\n",sizeof(seq));
 int tamPaquete = sizeof(seq);
 asignarPqte(seq,valA, colaC, nodoC,tamPaquete);
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
  //imprimirBuffer(colaB);
  //imprimirBuffer(colaA);
 pthread_mutex_lock(&mlectores);
 lectores--;
 if (lectores == 0) 
  pthread_mutex_unlock(&mutexC);
 pthread_mutex_unlock(&mlectores);
 pthread_exit(0);
}

/*
 * Destinar datos a paquetes
 */
void asignarPqte(char seq[], char val[], Queue *cola, NodeList *nodo,int tamPaquete){
 int i;
 printf("%i\n", tamPaquete);
 for(i = 0; i < tamPaquete; i++){  
  printf("%c\n", seq[i]);
  /*Escribe en C*/
  /*
  if(seq[i]==val[0]){   
   nodo = nodeListNew(seq[i]);
   if(nodo!=NULL){
   nodo = nodeListNew(seq[i]);
   queueEnqueue(cola,nodo);  
   printf("\nCola: %c\n",(char)nodeListGetCont(nodo));   
   }
  }*/
 }
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

/*
  *compara 2 valores
*/
int fn_Comparar(void const *pa, void const *pb){
  char a = *((char *) pa);
  char b = *((char *) pb);   
  
  return b - a;
}
     
