proyecto: Proyecto.o generic.o list.o nodelist.o queue.o
	gcc -o proyecto Proyecto.o generic.o list.o nodelist.o queue.o -l pthread 
main.o: Proyecto.c
	gcc -c Proyecto.c

list.o: list.c list.h
	gcc -c list.c

generic.o: generic.c generic.h
	gcc -c generic.c

nodelist.o: nodelist.c nodelist.h
	gcc -c nodelist.c

queue.o: queue.c queue.h
	gcc -c queue.c

clean:
	rm proyecto \
		Proyecto.o