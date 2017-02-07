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
void *imprimePrioridadC(void *t);
void *imprimePrioridadB(void *t);
void *imprimePrioridadA(void *t);

int main (int argc, char *argv[]){

  Elemento* datoEntrada;
  indiceCola = extraer();
  char seq[6]="ABCCCC";//dato de entrada
  int i, rc, numHilos = 0,numHilosC = 0,numHilosB = 0,numHilosA = 0,numHilosTotal = 0;

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
  char id = (int) t;
  printf("HILO C-----> %i <-----", id);
  printf("veo el primer elemento de la cola es %c\n",verDatoPrimerElelemento());
  while (verDatoPrimerElelemento()!=NULL) {
    printf("%c  ---- contC =  %i\n",verDatoPrimerElelemento(),contC);
    sleep(2);
    if(verDatoPrimerElelemento() == 'C' && contC < globalContC){
      printf("%i\n",contC);
      contC++;
      pthread_mutex_lock(&count_mutex);
      indiceCola = extraer();
      printf("Saco de la cola el elemento %c\n", indiceCola->nombre);
      contC--;
      pthread_mutex_unlock(&count_mutex);
    }
  }
}


void *imprimePrioridadB(void *t){
  char id = (int) t;
  sleep(2);
  printf("HILO B-----> %i <-----", id);
  printf("veo el primer elemento de la cola es %c\n",verDatoPrimerElelemento());
  sleep(2);
  while (verDatoPrimerElelemento()!=NULL) {
    printf("%c  ---- contC =  %i\n",verDatoPrimerElelemento(),contC);
    sleep(2);
    if(verDatoPrimerElelemento() == 'B' && contC == 0){
      contB++;
      pthread_mutex_lock(&count_mutex);
      indiceCola = extraer();
      printf("Saco de la cola el elemento %c\n", indiceCola->nombre);
      contB--;
      pthread_mutex_unlock(&count_mutex);
    }
  }
}


void *imprimePrioridadA(void *t){
  char id = (int) t;
  printf("HILO A-----> %i <-----", id);
  printf("veo el primer elemento de la cola es %c\n",verDatoPrimerElelemento());
  sleep(2);
  while (verDatoPrimerElelemento()!=NULL) {
    printf("%c  ---- contB =  %i\n",verDatoPrimerElelemento(),contB);
    sleep(2);
    if(verDatoPrimerElelemento() == 'A' && contB == 0){
      contA++;
      pthread_mutex_lock(&count_mutex);
      indiceCola = extraer();
      printf("Saco de la cola el elemento %c\n", indiceCola->nombre);
      contA--;
      pthread_mutex_unlock(&count_mutex);
    }
  }
}

//////////function of the QUEUE////////////
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

