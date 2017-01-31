#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

//estrucutra de la cola
typedef struct Elemento{
    struct elemento *proximo;
    char *nombre;
}Elemento;

Elemento *primer = NULL;
Elemento *ultimo = NULL;
Elemento* indiceCola;


//variables globales para la suspencion de hilos
int globalContC;
int globalContB;
int globalContA;

int contA = 0;
int contB = 0;
int contC = 0;

pthread_mutex_t count_mutex;  //mutex for count increase critical section
pthread_cond_t count_threshold_cv; //condition variable

void *inc_count(void *t); //thread function that increases a counter
void *watch_count(void *t); //thread function that wakes up when the counter reaches COUNT_LIMIT
void *imprime(void *t);
void agregar(Elemento* elemento);
Elemento* extraer();
char verDatoPrimerElelemento();

int main (int argc, char *argv[]){

  Elemento* datoEntrada;
  indiceCola = extraer();
  char seq[6]="ABCCCC";//dato de entrada
  int i, rc, numHilos = 0,numHilosC = 0,numHilosB = 0,numHilosA = 0,numHilosTotal = 0;
  char prioridadC = 'c', prioridadB = 'b', prioridadA = 'a';


  //llena la cola con los caracteres CBA
  for(i = 5; i >= 0 ; i --){
    datoEntrada = malloc(sizeof(Elemento));
    datoEntrada->nombre = seq[i];
    if(datoEntrada!=NULL){
      printf("  Datos agregado a la COLA : %c\n", datoEntrada->nombre);
      agregar(datoEntrada);
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
    pthread_create(&threadsCBA[i], &attr, imprime, (void *) prioridadC);
    pthread_join(threadsCBA[i], NULL);
  }

  for(i = numHilosC ; i < numHilosC+numHilosB ;i++){
    pthread_create(&threadsCBA[i], &attr, imprime, (void *) prioridadB);
    pthread_join(threadsCBA[i], NULL);
  }

  for(i = numHilosC+numHilosB ; i < numHilosC+numHilosB+numHilosA ; i++){
    pthread_create(&threadsCBA[i], &attr, imprime, (void *) prioridadA);
    pthread_join(threadsCBA[i], NULL);
  }


  /*
  Elemento* indice = extraer();
  printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");

  while(indice != NULL){
      printf("%c\n", indice->nombre);
      indice = extraer();
  }
  */

  /* Clean up and exit */
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&count_mutex);
  pthread_cond_destroy(&count_threshold_cv);
  pthread_exit(NULL);

}


//seccion critica de los hilos
void *imprime(void *t){
  char c = (char) t;
  printf("-----> %c <-----\n", verDatoPrimerElelemento());
  printf("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ\n");
  //printf("%c\n", indiceCola->nombre);
  while(verDatoPrimerElelemento() != NULL){
    printf("---------------------------------------------------------\n");
    printf("-----> %c <----- |||| -----> %i <-----\n", verDatoPrimerElelemento(), globalContC);
    if(verDatoPrimerElelemento() == 'C' && contC < globalContC){
      contC++;
      pthread_mutex_lock(&count_mutex);
      indiceCola = extraer();
      printf("%c\n", indiceCola->nombre);
      contC--;
      pthread_mutex_unlock(&count_mutex);
    }
    if(verDatoPrimerElelemento() == 'B' && contB < globalContB){
      contB++;
      pthread_mutex_lock(&count_mutex);
      indiceCola = extraer();
      printf("%c\n", indiceCola->nombre);
      contB--;
      pthread_mutex_unlock(&count_mutex);
    }
    if(verDatoPrimerElelemento() == 'A' && contA < globalContA){
      contA++;
      pthread_mutex_lock(&count_mutex);
      indiceCola = extraer();
      printf("%c\n", indiceCola->nombre);
      contA--;
      pthread_mutex_unlock(&count_mutex);
    }
  }

}
//////////funciones de la COLA////////////
void agregar(Elemento* elemento){
    //comprobando si la cola esta vacia
    elemento->proximo = NULL;
    if(primer == NULL){
        primer = elemento;
        ultimo = elemento;
    }
    else{
        ultimo->proximo = elemento;
        ultimo = elemento;
    }
}

Elemento* extraer(){
    if(primer == NULL){
        return NULL;
    }
    Elemento* elementoRetorno = primer;
    primer = primer->proximo;

    return elementoRetorno;
}

char verDatoPrimerElelemento(){
  if(primer == NULL){
      return NULL;
  }
  Elemento* elemento = primer;
  char c = (char) elemento->nombre;
  return c;
}

////////////////////////////////////////

/*
void *inc_count(void *t)
{
  int i;
  long my_id = (long)t;

  printf("Starting thread %ld\n", my_id);
  for (i=0; i<TCOUNT; i++) {
    pthread_mutex_lock(&count_mutex);
    count++;

    if (count == COUNT_LIMIT) {
      pthread_cond_signal(&count_threshold_cv);
      printf("thread %ld, count = %d  Threshold reached.\n", my_id, count);
      }
    printf("thread %ld, count = %d, unlocking mutex\n",  my_id, count);
    pthread_mutex_unlock(&count_mutex);

    sleep(1);
    }
  pthread_exit(NULL);
}

void *watch_count(void *t)
{
  long my_id = (long)t;

  printf("Starting thread %ld\n", my_id);

  pthread_mutex_lock(&count_mutex);
  while (count<COUNT_LIMIT) {
    printf("thread %ld suspended.\n", my_id);
    pthread_cond_wait(&count_threshold_cv, &count_mutex);
    printf("thread %ld Condition signal received.\n", my_id);
    count += 125;
    printf("thread %ld count now = %d.\n", my_id, count);
    }
  pthread_mutex_unlock(&count_mutex);
  pthread_exit(NULL);
}
*/
