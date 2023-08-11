#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
valgrind ./ProgettoAPI --leak-check=full --show-leak-kinds=all [--track-origins=yes]
valgrind --tool=callgrind ./ProgettoAPI | kcachegrind callgrind.out.*
valgrind --tool=massif ./ProgettoAPI | massif-visualizer massif.out.*
*/

typedef struct station
{
    int distance;
    int max_autonomy;
    struct station *prev;
    struct station *next;
}
station;





//---------------------------- PROTOTIPI ----------------------------//


void parse_input(char *input);

void add_station(station *highway, int dist, int max_car);
void T_print_stations(station *highway);


//------------------------------------------------------------------//

 
int main(){
    station **highway=NULL;
    
    add_station(highway, 5, 5);
    add_station(highway, 6, 5);
    add_station(highway, 7, 5);
    T_print_stations(highway);
    //_____________

    /*
    char buffer[100];

    while(fgets(buffer, 100, stdin) != NULL){   
        parse_input(buffer);
    }
    
    return 0;
    */
}

//---------------------------- DICHIARAZIONI ----------------------------//

void parse_input(char *input)
{
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

void add_station(station *highway, int dist, int max_car)
{
    station *new, *curr=highway;

    new=malloc(sizeof(station));
    new->distance=dist;
    new->max_autonomy=max_car;
    
    if(curr == NULL){
        
    }
    
}

void T_print_stations(station *highway)
{
    station *curr=highway;

    while(curr != NULL){
        fprintf(stderr, "%d | %d\n", curr->distance, curr->max_autonomy);
        curr=curr->next;
    }
}
