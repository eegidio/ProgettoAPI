#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
valgrind ./ProgettoAPI --leak-check=full --show-leak-kinds=all [--track-origins=yes]
valgrind --tool=callgrind ./ProgettoAPI | kcachegrind callgrind.out.*
valgrind --tool=massif ./ProgettoAPI | massif-visualizer massif.out.*
*/

//--------------------------------------------------------------------

typedef struct station{
    int dist;
    int max_car;
    int n_cars;
    struct car *parking;
    struct station *next, *prev;
}station;

typedef struct car{
    int autonomy;
    struct car *next;
}car;




//init
void parse_input(char *input);
//aggiungi-stazione
void add_station(int distance, int n_parked, int *parking);
int filter_sort_parking(int n_cars, int *cars);
//demolsci_stazione
//aggiungi_auto
//rottama_auto
//perscorso

static station *zero;

//--------------------------------------------------------------------

/* 
int main(){

    char buffer[100];

    zero = NULL;

    while(fgets(buffer, 100, stdin) != NULL){   
        parse_input(buffer);
    }
    
    return 0;
}
*/
///*

//TEST main
int main(){
    int ex[5]={1,2,3,4,5};
    add_station(5, 5, ex);

    return 0;
}

//*/


void parse_input(char *input){
    //non esegue controlli di correttezza sulla stringa di input
    char *token, delim[2]=" ";
    int n1;

    token = strtok(input, delim);
    n1 = atoi(strtok(NULL, delim));

    if(strcmp(token, "aggiungi-stazione")==0){
        int n_cars;

        n_cars = atoi(strtok(NULL, delim));
        
        int cars[n_cars];

        for(int i=0; i<n_cars; i++)
            cars[i] = atoi(strtok(NULL, delim));

        //da rimuovere duplicati, trovare max ed ordinare
        
        //control print
        fprintf(stderr, "adding station %d with %d cars : ", n1, n_cars);
        for(int i=0; i<n_cars; i++)
            fprintf(stderr, "%d, ", cars[i]);
        fprintf(stderr, "\n");

    } else if(strcmp(token, "demolisci-stazione")==0){
        //control print
        fprintf(stderr, "deleting station %d\n", n1);

    }else if(   strcmp(token, "aggiungi-auto")==0 ||
                strcmp(token, "rottama-auto")==0 ||
                strcmp(token, "pianifica-percorso")==0 ){
        int n2;

        n2 = atoi(strtok(NULL, delim));

        if(strcmp(token, "aggiungi-auto")==0){
            //control print
            fprintf(stderr, "adding car to %d with autonomy %d\n", n1, n2);

        }else if(strcmp(token, "rottama-auto")==0){
            //control print
            fprintf(stderr, "deleting car to %d with autonomy %d\n", n1, n2);

        }else if(strcmp(token, "pianifica-percorso")==0){
            //control print
            fprintf(stderr, "planning route from %d to %d\n", n1, n2);
        }
    }
    
    return;
}

void add_station(int distance, int n_parked, int *parking){
    station *current = zero, *new;

    new = (station*)malloc(sizeof(station));
    new->dist = distance;
    new->n_cars = n_parked;

    
}

int filter_sort_parking(int n_cars, int *cars){
    
    

}
