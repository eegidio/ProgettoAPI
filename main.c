#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include  <math.h>

#define inf INT32_MAX //potrebbe richiedere u_int_32

//------------------------------  ------------------------------//

typedef struct car{
    int autonomy;
    struct car *next;
}car;

typedef enum rb_color{
    RB_RED , RB_BLACK
}rb_color;

typedef struct rb_node{
    int distance;
    int max_autonomy;
    car *parking;

    struct rb_node *p;
    struct rb_node *right;
    struct rb_node *left;
    rb_color c;
}rb_node;

typedef struct rb_tree{
    rb_node *root;
    rb_node *nil;
}rb_tree;

typedef struct heap_node{
    int32_t es_dist;
    int key;
    int max_autonomy;
    struct heap_node *prev;
}heap_node;

typedef struct heap{
    heap_node *min_h;
    int heap_size;
    int heap_lenght;
}heap;

static const char *ns = "aggiungi-stazione";
static const char *ds = "demolisci-stazione";
static const char *nc = "aggiungi-auto";
static const char *dc = "rottama-auto";
static const char *pr = "pianifica-percorso";

//------------------------------ PROTOTIPI ------------------------------//

void parse_input(rb_tree *t, char *input);
void add_station(rb_tree *t, int distance, int n_cars, int *parking);
void remove_station(rb_tree *t, int distance);
void add_car(rb_tree *t,int station, int car);
void remove_car(rb_tree *t, int station, int car);
void plan(rb_tree *t, int start, int finish);
void print_out(heap_node *v, int len, int dest);
void print_out_rec(heap_node *x);
//--- Heap ---
void h_init(heap *x, int lenght);
void h_deinit(heap *x);
int h_parent(int i);
int h_left(int i);
int h_right(int i);
void h_fill(heap *h, rb_tree *t, int start, int finish);
bool h_isALessThanB(heap *h, int a, int b);
void h_swap(heap *h, int a, int  b);
void h_build_min_heap(heap *h);
void h_min_heapify(heap *h, int n);
heap_node h_exctract_min(heap *h);
void h_decrement_prio(heap *h, int i, int prio);
//--- List ---
car *parking_new_node(int aut);
void parking_delete_node(car *del);
void parking_delete_all(rb_node *s);
void parking_delete_all_rec(car *c);
bool parking_insert(rb_node *s, int aut);
bool parking_delete(rb_node *s, int aut);
//--- RB tree ---
rb_node *rb_new_node(rb_tree *t, rb_color c, int key);
void rb_delete_node(rb_node *n);
void rb_init(rb_tree *t);
void rb_delete_all(rb_tree *t, rb_node *n);
void rb_deinit(rb_tree *t);
void rb_left_rotate(rb_tree *t, rb_node *x);
void rb_right_rotate(rb_tree *t, rb_node *x);
rb_node *rb_insert(rb_tree *t, int key);
rb_node *rb_insert_node(rb_tree *t, rb_node *z);
void rb_insert_fixup(rb_tree *t, rb_node *z);
void rb_transplant(rb_tree *t, rb_node *u, rb_node *v);
rb_node *rb_subtree_min(rb_tree *t, rb_node *x);
rb_node *rb_find(rb_tree *t, int key);
bool rb_remove(rb_tree *t, int key);
void rb_remove_node(rb_tree *t, rb_node *z);
void rb_remove_fixup(rb_tree *t, rb_node *x);
rb_node *rb_successor(rb_tree *t, rb_node *x);
//testing
void rb_print(rb_tree *t, rb_node *n);
void h_print(heap *h);
void h_print_visited(heap_node *v, int len);

//------------------------------ MAIN ------------------------------//

int main()
{
    rb_tree a;
    char buffer[500];

    rb_init(&a);
    
    while(fgets(buffer, 500, stdin) != NULL){
        parse_input(&a, buffer);
        //rb_print(&a, a.root);
    }
    
    rb_deinit(&a);
    
    return 0;
}

//------------------------------ DICHIARAZIONI ------------------------------//

void parse_input(rb_tree *t, char *input)
{
    //non esegue controlli di correttezza sulla stringa di input
    char *token, delim[2] = " ";
    int n1;

    token = strtok(input, delim);
    n1 = atoi(strtok(NULL, delim));
    if(strcmp(token, ns) == 0){
        int n_cars;
        n_cars = atoi(strtok(NULL, delim));
        int cars[n_cars];
        for(int i=0; i<n_cars; i++)
            cars[i] = atoi(strtok(NULL, delim));
        add_station(t, n1, n_cars, cars);
        /*
        fprintf(stderr, "adding station %d with %d cars : ", n1, n_cars);
        for(int i=0; i<n_cars; i++)
            fprintf(stderr, "%d, ", cars[i]);
        fprintf(stderr, "\n");
        */
    } else if(strcmp(token, ds)==0){
        remove_station(t, n1);
        //fprintf(stderr, "deleting station %d\n", n1);
    }else if(strcmp(token, nc)==0 ||
                strcmp(token, dc)==0 ||
                strcmp(token, pr)==0 ){
        int n2 = atoi(strtok(NULL, delim));
        if(strcmp(token, nc)==0){
            add_car(t, n1, n2);
            //fprintf(stderr, "adding car to %d with autonomy %d\n", n1, n2);
        }else if(strcmp(token, dc)==0){
            remove_car(t, n1, n2);
            //fprintf(stderr, "deleting car to %d with autonomy %d\n", n1, n2);
        }else if(strcmp(token, pr)==0){
            //fprintf(stderr, "planning route from %d to %d\n", n1, n2);
            plan(t, n1, n2);
        }
    }
}

void add_station(rb_tree *t, int distance, int n_cars, int *parking)
{
    rb_node *new_station = rb_insert(t, distance);
    
    if(new_station == NULL){
        printf("non aggiunta\n");
        return;
    }else{
        for(int i=0; i<n_cars; i++)
            parking_insert(new_station, parking[i]);
        printf("aggiunta\n");
    }
}

void remove_station(rb_tree *t, int distance)
{
    if(rb_remove(t, distance))
        printf("demolita\n");
    else
        printf("non demolita\n");
}

void add_car(rb_tree *t,int station, int car)
{      
    rb_node *s = rb_find(t, station);
    if(s && parking_insert(s, car))
        printf("aggiunta\n");
    else printf("non aggiunta\n");
}

void remove_car(rb_tree *t, int station, int car)
{
    rb_node *s = rb_find(t, station);
    if(s && parking_delete(s, car))
        printf("rottamata\n");
    else printf("non rottamata\n");
}

void plan(rb_tree *t, int start, int finish)
{
    heap h;
    bool LtoR;
    if(start < finish)
        LtoR = true;
    else LtoR = false;

    h_init(&h, fabs(start - finish));
    h_fill(&h, t, start, finish);
    h_build_min_heap(&h);
    
    int j=0, ndis, reach;
    heap_node *u, *v, *visited;
    visited = calloc(h.heap_size, sizeof(heap_node));

    while(h.heap_size > 0){
        visited[j] = h_exctract_min(&h);
        u = visited + j;
        j++;
        if(LtoR)
            reach = u->key + u->max_autonomy;
        else 
            reach = fabs(u->key - u->max_autonomy);
        for(int i = 0 ; i < h.heap_size ; i++){
            if( (LtoR && h.min_h[i].key <= reach) ||
                (!LtoR && h.min_h[i].key >= reach)){
                v = h.min_h + i;
                ndis = u->es_dist +1;
                if(v->es_dist > ndis){
                    v->prev = u;
                    h_decrement_prio(&h, i, ndis);
                }
            }
        }
    }    
    print_out(visited, j, finish);

    free(visited);
    h_deinit(&h);
}

void print_out(heap_node *v, int len, int dest)
{
    heap_node *curr = v;

    int i = 1;
    while(curr->key != dest && i<len){
        if(v[i].key == dest)
            curr = v + i;
        i++;
    }
    if(curr->key == v[0].key){
        printf("nessun percorso\n");
        return;
    }
    print_out_rec(curr);
    printf("\n");
}

void print_out_rec(heap_node *x)
{
    if(x == NULL){
        return;
    }else{
        print_out_rec(x->prev);
        printf("%d ", x->key);
    } 
}


//___________________________________________________
void h_init(heap *x, int lenght)
{
    x->heap_lenght = lenght;
    x->heap_size = 0;
    x->min_h = calloc(lenght, sizeof(heap_node));
}

void h_deinit(heap *x)
{
    free(x->min_h);
}

int h_parent(int i)
{
    return (i-1)/2;
}

int h_left(int i)
{
    return (2*i)+1;
}

int h_right(int i)
{
    return (2*i)+2;
}

void h_fill(heap *h, rb_tree *t, int start, int finish)
{
    int min_val, max_val;
    if(start < finish){
        min_val = start;
        max_val = finish;        
    }else{
        min_val = finish;
        max_val = start;
    }

    rb_node *s = rb_find(t, min_val);
    while(s != t->nil && s->distance <=max_val){
        h->min_h[h->heap_size].key = s->distance;
        h->min_h[h->heap_size].max_autonomy = s->max_autonomy;
        h->min_h[h->heap_size].prev = NULL;
        if(s->distance == start)
            h->min_h[h->heap_size].es_dist = 0;
        else
            h->min_h[h->heap_size].es_dist = inf;
        
        s = rb_successor(t, s);
        h->heap_size++;
    }    

    
}

bool h_isALessThanB(heap *h, int a, int b)
{
    assert(a != b);
    if(h->min_h[a].es_dist < h->min_h[b].es_dist)
        return  true;
    else if(h->min_h[a].es_dist > h->min_h[b].es_dist)
        return false;
    else{
        if(h->min_h[a].key < h->min_h[b].key)
            return true;
        else return false;
    }
}

void h_swap(heap *h, int a, int  b)
{
    heap_node tmp = h->min_h[a];
    h->min_h[a] = h->min_h[b];
    h->min_h[b] = tmp;
}

void h_build_min_heap(heap *h)
{
    for(int i=(h->heap_size/2); i>=0; i--)
        h_min_heapify(h, i);
}

void h_min_heapify(heap *h, int n)
{
    int posmin;
    int l = h_left(n);
    int r = h_right(n);
    if(l < h->heap_size && h_isALessThanB(h, l, n))
        posmin = l;
    else posmin = n;
    if(r < h->heap_size && h_isALessThanB(h, r, posmin))
        posmin = r;
    if(posmin != n){
        h_swap(h, n, posmin);
        h_min_heapify(h, posmin);
    }
}

heap_node h_exctract_min(heap *h)
{
    if(h->heap_size == 0)
        return (heap_node){-1,-1,-1,NULL};

    heap_node min = h->min_h[0];
    h_swap(h, 0, h->heap_size-1);
    h->heap_size--;
    h_min_heapify(h, 0);
    return min;
}

void h_decrement_prio(heap *h, int i, int prio)
{
    if(prio > h->min_h[i].es_dist){
        fprintf(stderr, "Error : h_decrement_prio, new key not valid");
        return;
    }
    h->min_h[i].es_dist = prio;
    while(i > 0 && h_isALessThanB(h, i, h_parent(i))){
        h_swap(h, i, h_parent(i));
        i = h_parent(i);
    }
}

void h_print(heap *h)
{
    fprintf(stderr, "lenght: %d size: %3d\n", h->heap_lenght, h->heap_size);
    for(int i=0; i<h->heap_size; i++)
        fprintf(stderr, "%10d | %3d | %3d\n", h->min_h[i].es_dist, h->min_h[i].key, h->min_h[i].max_autonomy);
}

//___________________________________________________
car *parking_new_node(int aut)
{
    car *new_car = calloc(1, sizeof(car));
    new_car->autonomy = aut;
    new_car->next = NULL;
    return new_car;
}

void parking_delete_node(car *del)
{
    free(del);
}

void parking_delete_all(rb_node *s)
{
    parking_delete_all_rec(s->parking);
}

void parking_delete_all_rec(car *c)
{
    if(c == NULL)
        return;
    else{
        parking_delete_all_rec(c->next);
        free(c);
    }
}

bool parking_insert(rb_node *s, int aut)
{
    car *new = parking_new_node(aut);
    car *curr = s->parking;
    
    if(curr == NULL || curr->autonomy < aut){
        s->max_autonomy = aut;
        s->parking = new;
        new->next = curr;
    }else{
        car *prev = curr;
        while(curr != NULL && curr->autonomy >= aut){
            if(curr->autonomy == aut){
                parking_delete_node(new);   
                return 0;
            }
            prev = curr;
            curr = curr->next;
        }
        prev->next = new;
        new->next = curr;
    }
    return true;
}

bool parking_delete(rb_node *s, int aut)
{
    car *curr = s->parking;

    if(curr == NULL)
        return false;
    else if(curr->autonomy == aut){
        if(curr->next == NULL)
            s->max_autonomy = 0;
        else 
            s->max_autonomy = curr->next->autonomy;
        s->parking = curr->next;
    }else{
        car *prev = curr;
        while(curr != NULL && curr->autonomy != aut){
            prev = curr;
            curr = curr->next;
        }
        if(curr == NULL)
            return false;
        else
            prev->next = curr->next;
    }
    free(curr);
    return true;

}

//___________________________________________________
rb_node *rb_new_node(rb_tree *t, rb_color c, int key)
{
    rb_node *new = calloc(1, sizeof(rb_node));
    new->distance = key;
    new->max_autonomy = 0;
    new->parking = NULL;
    new->p = t->nil;
    new->left = t->nil;
    new->right = t->nil;
    new->p = t->nil;
    new->c = c;
    return new;
}

void rb_delete_node(rb_node *n)
{
    parking_delete_all(n);
    free(n);
}

void rb_init(rb_tree *t)
{
    rb_node *nil = calloc(1, sizeof(rb_node));
    nil->c = RB_BLACK;
    //il valore distance=0 potrebbe dare problemi
    t->nil = nil;
    t->root = t->nil;
}

void rb_delete_all(rb_tree *t, rb_node *n)
{   
    if(n == t->nil)
        return;

    rb_delete_all(t, n->left);
    rb_delete_all(t, n->right);
    rb_delete_node(n);    
}

void rb_deinit(rb_tree *t)
{
    rb_delete_all(t, t->root);
    rb_delete_node(t->nil);
}

void rb_left_rotate(rb_tree *t, rb_node *x)
{
    rb_node *y = x->right;
    x->right = y->left;
    if(y->left != t->nil)
        y->left->p = x;
    y->p = x->p;
    if(x->p == t->nil)
        t->root = y;
    else if(x == x->p->left)
        x->p->left = y;
    else 
        x->p->right = y;
    y->left = x;
    x->p = y;
}

void rb_right_rotate(rb_tree *t, rb_node *x)
{
    rb_node *y = x->left;
    x->left = y->right;
    if(y->right != t->nil)
        y->right->p = x;
    y->p = x->p;
    if(x->p == t->nil)
        t->root = y;
    else if(x == x->p->left)
        x->p->left = y;
    else 
        x->p->right = y;
    y->right = x;
    x->p = y;
}

rb_node *rb_insert(rb_tree *t, int key)
{
    rb_node *new = rb_new_node(t, RB_RED, key);
    rb_node *inserted = rb_insert_node(t, new);
    if(inserted)
        return inserted;
    else{
        rb_delete_node(new);
        return NULL;
    }
}

rb_node *rb_insert_node(rb_tree *t, rb_node *z)
{
    rb_node *y = t->nil;
    rb_node *x = t->root;

    while(x != t->nil){
        y = x;
        if(z->distance == y->distance){
            return NULL;
        }else if(z->distance < y->distance){
            x = x->left;
        }else{
            x = x->right;
        }
    }
    
    z->p = y;
    if(y == t->nil){
        t->root = z;
    }else if(z->distance < y->distance){
        y->left = z;
    }else{
        y->right = z;
    }
    
    z->left = t->nil;
    z->right = t->nil;
    z->c = RB_RED;
    rb_insert_fixup(t, z);
    return z;
}

void rb_insert_fixup(rb_tree *t, rb_node *z)
{
    while(z->p->c == RB_RED){
        if(z->p == z->p->p->left){
            rb_node *y = z->p->p->right;
            if(y->c == RB_RED){
                z->p->c = RB_BLACK;
                y->c  =  RB_BLACK;
                z->p->p->c = RB_RED;
                z = z->p->p;
            }else {
                if(z == z->p->right){
                    z = z->p;
                    rb_left_rotate(t, z);
                }
                z->p->c = RB_BLACK;
                z->p->p->c =  RB_RED;
                rb_right_rotate(t, z->p->p);
            }
        }else{
            rb_node *y = z->p->p->left;
            if(y->c == RB_RED){
                z->p->c = RB_BLACK;
                y->c  =  RB_BLACK;
                z->p->p->c = RB_RED;
                z = z->p->p;
            }else {
                if(z == z->p->left){
                    z = z->p;
                    rb_right_rotate(t, z);
                }
                z->p->c = RB_BLACK;
                z->p->p->c =  RB_RED;
                rb_left_rotate(t, z->p->p);
            }
        }
    }
    t->root->c = RB_BLACK;
}

void rb_transplant(rb_tree *t, rb_node *u, rb_node *v)
{
    if(u->p == t->nil)
        t->root = v;
    else if(u == u->p->left)
        u->p->left = v;
    else u->p->right = v;
    v->p = u->p;
}

rb_node *rb_subtree_min(rb_tree *t, rb_node *x)
{
    while(x->left != t->nil)
        x = x->left;
    return x;
}

rb_node *rb_find(rb_tree *t, int key)
{
    rb_node *x = t->root;
    while(x != t->nil){
        if(x->distance == key)
            return x;
        else if(x->distance > key)
            x = x->left;
        else
            x = x->right;
    }
    return NULL;
}

bool rb_remove(rb_tree *t, int key)
{
    rb_node *x = rb_find(t, key);
    if(x){
        rb_remove_node(t, x);
        rb_delete_node(x);
        return true;
    }else return false;
}

void rb_remove_node(rb_tree *t, rb_node *z)
{
    rb_node *x = NULL;
    rb_node *y = z;
    enum rb_color y_original = y->c;

    if(z->left == t->nil){
        x = z->right;
        rb_transplant(t, z, z->right);
    }else if(z->right == t->nil){
        x = z->left;
        rb_transplant(t, z, z->left);
    }else{
        y = rb_subtree_min(t, z->right);
        y_original = y->c;
        x = y->right;
        if(y->p == z){
            x->p = y;
        }else{
            rb_transplant(t, y, y->right);
            y->right = z->right;
            y->right->p = y;
        }
        rb_transplant(t, z, y);
        y->left = z->left;
        y->left->p = y;
        y->c = z->c;
    }
    if(y_original == RB_BLACK)
        rb_remove_fixup(t, x);
}

void rb_remove_fixup(rb_tree *t, rb_node *x)
{
    rb_node *w = NULL;
    while(x != t->root && x->c == RB_BLACK){
        if(x == x->p->left){
            w = x->p->right;
            if(w->c == RB_RED){
                w->c = RB_BLACK;
                x->p->c = RB_RED;
                rb_left_rotate(t, x->p);
                w = x->p->right;
            }
            if(w->left->c == RB_BLACK && w->right->c == RB_BLACK){
                w->c = RB_RED;
                x = x->p;
            }else{
                if(w->right->c == RB_BLACK){
                    w->left->c = RB_BLACK;
                    w->c = RB_RED;
                    rb_right_rotate(t, w);
                    w = x->p->right;
                }
                w->c = x->p->c;
                x->p->c = RB_BLACK;
                w->right->c = RB_BLACK;
                rb_left_rotate(t, x->p);
                x = t->root;
            }
        }else{
            w = x->p->left;
            if(w->c == RB_RED){
                w->c = RB_BLACK;
                x->p->c = RB_RED;
                rb_right_rotate(t, x->p);
                w = x->p->left;
            }
            if(w->right->c == RB_BLACK && w->left->c == RB_BLACK){
                w->c = RB_RED;
                x = x->p;
            }else{
                if(w->left->c == RB_BLACK){
                    w->right->c = RB_BLACK;
                    w->c = RB_RED;
                    rb_left_rotate(t, w);
                    w = x->p->left;
                }
                w->c = x->p->c;
                x->p->c = RB_BLACK;
                w->left->c = RB_BLACK;
                rb_right_rotate(t, x->p);
                x = t->root;
            }
        }
    }
    x->c = RB_BLACK;
}

rb_node *rb_successor(rb_tree *t, rb_node *x)
{
    if(x->right != t->nil)
        return rb_subtree_min(t, x->right);
    rb_node *y = x->p;
    while(y !=  t->nil && x == y->right){
        x = y;
        y = y->p;
    }
    return y;
}

void rb_print(rb_tree *t, rb_node *n)
{
    if(n == t->nil)
        return;
    else{
        rb_print(t, n->left);
        fprintf(stderr, "%3d [%3d] | ", n->distance, n->max_autonomy);
        car *c = n->parking;
        while(c != NULL){
            fprintf(stderr, "%3d ", c->autonomy);
            c = c->next;
        }
        fprintf(stderr, "\n");
        rb_print(t, n->right);
    }
}

void h_print_visited(heap_node *v, int len)
{
    fprintf(stderr, "%4d |        SRC\n", v[0].key);
    for(int  i=1; i<len; i++)
        fprintf(stderr, "%4d | %4d | %4d\n",  v[i].key,  v[i].prev->key, v[i].es_dist);
}