#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define PRINT_LOG false
#define BUFFER_SIZE 7000
#define HEAP_MAX_LEN 100000
#define INF UINT32_MAX
#define MAT_INIT 10
#define chr_cmp(c) ((c != 32 && c != 0) ? true : false)
#define is_adjacent(LtoR, key, reach) ((LtoR && key <= reach) || (!LtoR && key >= reach))

//------------------------------  ------------------------------//

typedef struct car{
    uint32_t autonomy;
    int count;
}car;

typedef enum rb_color{
    RB_RED , RB_BLACK
}rb_color;

typedef struct rb_node{
    uint32_t distance;

    uint32_t max_autonomy;
    int n_cars;
    int p_len;
    int p_size; 
    car *parking;

    uint32_t es_dist;
    struct rb_node *prev;

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
    uint32_t es_dist;
    uint32_t key;
    //uint32_t max_autonomy;
    //rb_node *tree_node;
    //struct heap_node *prev;
}heap_node;

typedef struct heap{
    heap_node *min_h;
    int heap_size;
    int heap_lenght;
}heap;

static const char *ns = "aggiungi-stazione";    //18
static const char *ds = "demolisci-stazione";   //19
static const char *nc = "aggiungi-auto";        //14
static const char *dc = "rottama-auto";         //13
static const char *pr = "pianifica-percorso";   //19

//------------------------------ PROTOTIPI ------------------------------//
void add_station(rb_tree *t, uint32_t distance, int n_cars, uint32_t *parking);
void remove_station(rb_tree *t, uint32_t distance);
void add_car(rb_tree *t, uint32_t station, uint32_t car);
void remove_car(rb_tree *t, uint32_t station, uint32_t car);
void plan(rb_tree *t, heap *h, uint32_t start, uint32_t finish);
//--- Others ---
void parse_input(rb_tree *t, heap *h, char *input);
int my_strtok(char *input, char *token, int in_len, int start_point);
uint32_t my_atoi(char *s, int s_len);
uint32_t get_number(char *input, char *token, int in_len, int *read);
bool parking_insert(rb_node *s, uint32_t aut);
bool parking_delete(rb_node *s, uint32_t aut);
int dijkstra(heap *h, heap_node *visited, bool LtoR, uint32_t dest);
void print_route(heap_node *v, int len, uint32_t start, uint32_t finish);
void print_route_rec(heap_node *curr, heap_node *src, char *out);
//--- Heap ---
void h_init(heap *x, int lenght);
void h_deinit(heap *x);
int h_parent(int i);
int h_left(int i);
int h_right(int i);
void h_fill(heap *h, rb_tree *t, uint32_t start, uint32_t finish);
bool h_isALessThanB(heap *h, int a, int b);
void h_swap(heap *h, int a, int b);
void h_build_min_heap(heap *h);
void h_min_heapify(heap *h, int n);
heap_node h_exctract_min(heap *h);
void h_decrement_prio(heap *h, int i, uint32_t prio);
//--- Array ---
car *v_init(void);
void v_deinit(car *p);
car *v_expand(car *p, int new_len);
int v_find(car *p, int size, uint32_t key);
void v_insert(car *p, int size, uint32_t key);
void v_insert_sort(rb_node *s, int n_cars, uint32_t *cars);
void v_merge_sort(uint32_t *a, int p, int r);
void v_merge(uint32_t *a, int p, int q, int r);
void v_delete(car *p, int size, int pos);
//--- RB tree ---
rb_node *rb_new_node(rb_tree *t, rb_color c, uint32_t key);
void rb_delete_node(rb_node *n);
void rb_init(rb_tree *t);
void rb_delete_all(rb_tree *t, rb_node *n);
void rb_deinit(rb_tree *t);
void rb_left_rotate(rb_tree *t, rb_node *x);
void rb_right_rotate(rb_tree *t, rb_node *x);
rb_node *rb_insert(rb_tree *t, uint32_t key);
rb_node *rb_insert_node(rb_tree *t, rb_node *z);
void rb_insert_fixup(rb_tree *t, rb_node *z);
void rb_transplant(rb_tree *t, rb_node *u, rb_node *v);
rb_node *rb_subtree_min(rb_tree *t, rb_node *x);
rb_node *rb_subtree_max(rb_tree *t, rb_node *x);
rb_node *rb_find(rb_tree *t, uint32_t key);
bool rb_remove(rb_tree *t, uint32_t key);
void rb_remove_node(rb_tree *t, rb_node *z);
void rb_remove_fixup(rb_tree *t, rb_node *x);
rb_node *rb_successor(rb_tree *t, rb_node *x);
rb_node *rb_predecessor(rb_tree *t, rb_node *x);
//--- Prints ---
void rb_print(rb_tree *t, rb_node *n);
void h_print(heap *h);
void h_print_visited(heap_node *v, int len);

//------------------------------ MAIN ------------------------------//

int main()
{
    rb_tree a;
    heap b;
    char buffer[BUFFER_SIZE];

    rb_init(&a);
    h_init(&b, HEAP_MAX_LEN);

    while(fgets(buffer, BUFFER_SIZE, stdin) != NULL){
        parse_input(&a, &b, buffer);
    }

    h_deinit(&b);
    rb_deinit(&a);

    return 0;
}

//------------------------------ DICHIARAZIONI ------------------------------//
void add_station(rb_tree *t, uint32_t distance, int n_cars, uint32_t *parking)
{
    rb_node *new_station = rb_insert(t, distance);
    
    if(new_station == NULL){
        puts("non aggiunta");
        return;
    }else{
        v_insert_sort(new_station, n_cars, parking);
        puts("aggiunta");
    }
}

void remove_station(rb_tree *t, uint32_t distance)
{
    if(rb_remove(t, distance))
        puts("demolita");
    else
        puts("non demolita");
}

void add_car(rb_tree *t, uint32_t station, uint32_t car)
{      
    rb_node *s = rb_find(t, station);
    if(s && parking_insert(s, car))
        puts("aggiunta");
    else puts("non aggiunta");
}

void remove_car(rb_tree *t, uint32_t station, uint32_t car)
{
    rb_node *s = rb_find(t, station);
    if(s && parking_delete(s, car))
        puts("rottamata");
    else puts("non rottamata");
}

void plan(rb_tree *t, heap *h, uint32_t start, uint32_t finish)
{
    bool LtoR = (start < finish) ? true : false;

    h_fill(h, t, start, finish);
    h_build_min_heap(h);
    
    heap_node *visited;
    visited = calloc(h->heap_size, sizeof(heap_node));
    //rb_print(t, t->root);
    //h_print(h);
    
    int j = dijkstra(h, visited, LtoR, finish);

    //h_print_visited(visited, j);
    print_route(visited, j, start, finish);

    free(visited);
}

//___________________________________________________________________________//
void parse_input(rb_tree *t, heap *h, char *input)
{
    char token[30];
    int in_len = strlen(input), in_read = 0;
    uint32_t n1, n2, *cars;
     
    if(input[in_len - 1] == '\n'){
        input[in_len -1] = '\0';
        in_len--;
    }
    in_read = my_strtok(input, token, in_len, in_read);
    if(memcmp(token, ns, 18) == 0){
        n1 = get_number(input, token, in_len, &in_read);
        n2 = get_number(input, token, in_len, &in_read);
        cars = calloc(n2, sizeof(uint32_t));
        for(int i = 0; i< n2; i++)
            cars[i] = get_number(input, token, in_len, &in_read);
        if(PRINT_LOG){
            fprintf(stderr, "adding station %d with %d cars : ", n1, n2);
            for(int i=0; i<n2; i++)
                fprintf(stderr, "%d, ", cars[i]);
            fprintf(stderr, "\n");
        }
        add_station(t, n1, n2, cars);
        free(cars);
    }else if(memcmp(token, ds, 19) == 0){
        n1 = get_number(input, token, in_len, &in_read);
        if(PRINT_LOG)
            fprintf(stderr, "deleting station %d\n", n1);
        remove_station(t, n1);
    }else if(memcmp(token, nc, 14) == 0){
        n1 = get_number(input, token, in_len, &in_read);
        n2 = get_number(input, token, in_len, &in_read);
        if(PRINT_LOG)
            fprintf(stderr, "adding car to %d with autonomy %d\n", n1, n2);
        add_car(t, n1, n2);
    }else if(memcmp(token, dc, 13) == 0){
        n1 = get_number(input, token, in_len, &in_read);
        n2 = get_number(input, token, in_len, &in_read);
        if(PRINT_LOG)
            fprintf(stderr, "deleting car to %d with autonomy %d\n", n1, n2);
        remove_car(t, n1, n2);
    }else if(memcmp(token, pr, 19) == 0){
        n1 = get_number(input, token, in_len, &in_read);
        n2 = get_number(input, token, in_len, &in_read);
        if(PRINT_LOG)
            fprintf(stderr, "planning route from %d to %d\n", n1, n2);
        plan(t, h, n1, n2);
    }
}

int my_strtok(char *input, char *token, int in_len, int start_point)
{
    if(start_point >= in_len){
        fprintf(stderr, "[ERROR] my_strtok : noting to read\n");
        return -1;
    }
    char c = input[start_point];
    int tok_i = 0;
    
    while(chr_cmp(c)){
        tok_i++;
        c = input[start_point + tok_i];
    }
    memcpy(token, input + start_point, tok_i);
    token[tok_i] = '\0';
    return tok_i + 1;
}

uint32_t my_atoi(char *s, int s_len)
{
    uint32_t res = 0, pos=1;
    for(int i=s_len-1; i>=0; i--){
        res = res + (uint32_t)((s[i] - 48) * pos);
        pos = pos * 10;
    }
    return res;
}

uint32_t get_number(char *input, char *token, int in_len, int *read)
{
    int tok_len = my_strtok(input, token, in_len, *read);
    *read += tok_len;
    return my_atoi(token, tok_len-1);
}

bool parking_insert(rb_node *s, uint32_t aut)
{
    if(s->n_cars == 512)
        return false;
    else s->n_cars++;
    if(s->max_autonomy < aut)
        s->max_autonomy = aut;
    if(s->p_len == s->p_size){
        s->p_len = s->p_len * 4;
        s->parking = v_expand(s->parking, s->p_len);
    }
    int pos = v_find(s->parking, s->p_size, aut);
    if(pos >= 0)
        s->parking[pos].count++;
    else{
        v_insert(s->parking, s->p_size, aut);
        s->p_size++;
    }
    return true;
}

bool parking_delete(rb_node *s, uint32_t aut)
{
    if(s->n_cars == 0 || s->p_size == 0)
        return false;
    else s->n_cars--;
    int pos = v_find(s->parking, s->p_size, aut);
    if(pos < 0)
        return false;
    else s->parking[pos].count--;
    if(s->parking[pos].count == 0){
        v_delete(s->parking, s->p_size, pos);
        s->p_size--;
        if(aut == s->max_autonomy){
            if(s->p_size == 0)
               s->max_autonomy = 0;
            else s->max_autonomy = s->parking[0].autonomy; 
        }
    }
    return true;
}

int dijkstra(heap *h, heap_node *visited, bool LtoR, uint32_t dest)
{
    uint32_t ndis, reach;
    heap_node *u, *v;
    rb_node *tree_u, *tree_v;
    int j=0;
    bool done = false;

    while(h->heap_size > 0){
        visited[j] = h_exctract_min(h);
        u = visited + j;
        j++;
        tree_u = u->tree_node;
        if(LtoR)
            reach = tree_u->distance + tree_u->max_autonomy;
        else
            reach = (tree_u->distance > tree_u->max_autonomy) ? tree_u->distance - tree_u->max_autonomy : 0;
        while(){

            ndis = (u->es_dist != INF) ? u->es_dist + 1 : INF;
            
        }
    }


    /*
    while(h->heap_size > 0 && !done){
        visited[j] = h_exctract_min(h);
        u = visited + j;
        j++;
        if(LtoR)
            reach = u->key + u->max_autonomy;
        else{
            if(u->key < u->max_autonomy)
                reach = 0;
            else    
                reach = u->key - u->max_autonomy;
        }
        for(int i = 0 ; i < h->heap_size ; i++){
            if(is_adjacent(LtoR, h->min_h[i].key, reach)){
                v = h->min_h + i;
                if(u->es_dist != INF)
                    ndis = u->es_dist +1;
                else ndis = INF;
                if(v->es_dist > ndis){
                    v->prev = u;
                    h_decrement_prio(h, i, ndis);
                }
            }
        }
        if(u->key == dest)
            done = true;
    }
    */
    return j;
}

void print_route(heap_node *v, int len, uint32_t start, uint32_t finish)
{
    heap_node *src = v;
    heap_node *dest = v + (len-1);
    char output[len * 10 + 1];
    output[0]=0;
    
    for(int i = len-1; i>=0 && dest->key != finish; i--)
        dest = v + i;
    
    print_route_rec(dest, src, output);
    if(output[0] != 0){
        output[strlen(output)-1]=0;
        puts(output);
    }else puts("nessun percorso");
}

void print_route_rec(heap_node *curr, heap_node *src, char *out)
{
    char b[20];
    sprintf(b, "%"PRIu32" ", curr->key);

    if(curr->prev == NULL){
        if(curr == src)
            strcat(out, b);
        else out[0]=0;
        return;
    }
    print_route_rec(curr->prev, src, out);
    strcat(out, b);
}

//___________________________________________________________________________//
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

void h_fill(heap *h, rb_tree *t, uint32_t start, uint32_t finish)
{
    uint32_t min_val, max_val;
    if(start < finish){
        min_val = start;
        max_val = finish;
    }else{
        min_val = finish;
        max_val = start;
    }
    
    rb_node *s = rb_find(t, min_val);
    int i = 0;
    while(s != t->nil && s->distance <= max_val){
        h->min_h[i].key = s->distance;
        h->min_h->tree_node = s;
        //h->min_h[i].max_autonomy = s->max_autonomy;
        h->min_h[i].prev = NULL;
        if(s->distance == start)
            h->min_h[i].es_dist = 0;
        else
            h->min_h[i].es_dist = INF;
        
        s = rb_successor(t, s);
        i++;
        if(i > h->heap_lenght)
            fprintf(stderr, "[ERROR] h_fill : heap too small\n");
    }
    h->heap_size = i;
}

bool h_isALessThanB(heap *h, int a, int b)
{
    if(h->min_h[a].es_dist < h->min_h[b].es_dist)
        return true;
    else if(h->min_h[a].es_dist > h->min_h[b].es_dist)
        return false;
    else{
        if(h->min_h[a].key < h->min_h[b].key)
            return true;
        else return false;
    }
}

void h_swap(heap *h, int a, int b)
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
    if(h->heap_size == 0){
        fprintf(stderr, "[ERROR] h_extract_min : nothing to extract\n");
        return (heap_node){0,INF,0,NULL};
    }
        

    heap_node min = h->min_h[0];
    h_swap(h, 0, h->heap_size-1);
    h->heap_size--;
    h_min_heapify(h, 0);
    return min;
}

void h_decrement_prio(heap *h, int i, uint32_t prio)
{
    if(prio > h->min_h[i].es_dist){
        fprintf(stderr, "[ERROR] h_decrement_prio : new key not valid");
        return;
    }
    h->min_h[i].es_dist = prio;
    while(i > 0 && h_isALessThanB(h, i, h_parent(i))){
        h_swap(h, i, h_parent(i));
        i = h_parent(i);
    }
}

//___________________________________________________________________________//

car *v_init()
{
    car *p = calloc(32, sizeof(car));
    return p;
}

void v_deinit(car *p)
{
    free(p);
}

car *v_expand(car *p, int new_len)
{
    car *resized = realloc(p, new_len * sizeof(car));
    if(resized == NULL)
        fprintf(stderr, "[ERROR] p_expand : failed to reesize block\n");
    return resized;
}

int v_find(car *p, int size, uint32_t key)
{
    int l = 0, h = size - 1, m;
    while(l <= h){
        m = (l + h)/2;
        if(p[m].autonomy == key)
            return m;
        else if(p[m].autonomy < key)
            h = m - 1;
        else l = m + 1;
    }
    return -1;
}

void v_insert(car *p, int size, uint32_t key)
{
    int l = 0, h = size - 1, m;
    if(size == 0){
        p[0] = (car){key, 1};
        return;
    }
    if(key > p[l].autonomy)
        h = l;
    else if(key < p[h].autonomy)
        h++;
    else{
        while(l + 1 < h){
            m = (l + h)/2;
            if(p[m].autonomy > key)
                l = m;
            else h = m;
        }
    }
    memmove(p + h + 1, p + h, (size - h) * sizeof(car));
    p[h] = (car){key, 1};
}

void v_insert_sort(rb_node *s, int n_cars, uint32_t *cars)
{   
    if(n_cars == 0)
        return;
    v_merge_sort(cars, 0, n_cars - 1);

    int car_types = 0;
    for(int i = 0;  i<n_cars; i++){
        if(car_types == 0 || cars[i] != s->parking[car_types -1].autonomy){
            if(car_types == s->p_len){
                s->p_len = s->p_len * 4;
                s->parking = v_expand(s->parking, s->p_len);
            }
            s->parking[car_types] = (car){cars[i], 1};
            car_types++;
        }else s->parking[car_types-1].count++;
    }
    s->p_size = car_types;
    s->n_cars = n_cars;
    s->max_autonomy = cars[0];
}

void v_merge_sort(uint32_t *a, int p, int r)
{
    if(p < r - 1){
        int q = (p + r)/2;
        v_merge_sort(a, p, q);
        v_merge_sort(a, q + 1, r);
        v_merge(a, p, q, r);
    }else{
        if(a[p] < a[r]){
            uint32_t tmp = a[r];
            a[r] = a[p];
            a[p] = tmp;
        }
    }
}

void v_merge(uint32_t *a, int p, int q, int r)
{
    int len1 = q - p + 1;
    int len2 = r - q;
    uint32_t L[len1];
    uint32_t R[len2];
    memcpy(L, a + p, len1 * sizeof(uint32_t));
    memcpy(R, a + q + 1, len2 * sizeof(uint32_t));

    int i = 0, j = 0, k = p;
    while(i < len1 && j < len2){
        if(L[i] >= R[j]){
            a[k] = L[i];
            i++;
        }else{
            a[k] = R[j];
            j++;
        }
        k++;
    }
    if(i < len1)
        memcpy(a + k, L + i, sizeof(uint32_t) * (len1 - i));
    else if(j < len2)
        memcpy(a + k, R + j, sizeof(uint32_t) * (len2 - j));

}

void v_delete(car *p, int size, int pos)
{
    memmove(p + pos, p + pos + 1, (size - pos - 1) * sizeof(car));
}

//___________________________________________________________________________//
rb_node *rb_new_node(rb_tree *t, rb_color c, uint32_t key)
{
    rb_node *new = calloc(1, sizeof(rb_node));
    new->distance = key;
    new->max_autonomy = 0;
    new->p_len = 32;
    new->p_size = 0;
    new->n_cars = 0;
    new->parking = v_init();
    new->p = t->nil;
    new->left = t->nil;
    new->right = t->nil;
    new->c = c;
    return new;
}

void rb_delete_node(rb_node *n)
{
    v_deinit(n->parking);
    free(n);
}

void rb_init(rb_tree *t)
{
    rb_node *nil = calloc(1, sizeof(rb_node));
    nil->c = RB_BLACK;
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

rb_node *rb_insert(rb_tree *t, uint32_t key)
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
                y->c = RB_BLACK;
                z->p->p->c = RB_RED;
                z = z->p->p;
            }else {
                if(z == z->p->right){
                    z = z->p;
                    rb_left_rotate(t, z);
                }
                z->p->c = RB_BLACK;
                z->p->p->c = RB_RED;
                rb_right_rotate(t, z->p->p);
            }
        }else{
            rb_node *y = z->p->p->left;
            if(y->c == RB_RED){
                z->p->c = RB_BLACK;
                y->c = RB_BLACK;
                z->p->p->c = RB_RED;
                z = z->p->p;
            }else {
                if(z == z->p->left){
                    z = z->p;
                    rb_right_rotate(t, z);
                }
                z->p->c = RB_BLACK;
                z->p->p->c = RB_RED;
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

rb_node *rb_subtree_max(rb_tree *t, rb_node *x)
{
    while(x->right != t->nil)
        x = x->right;
    return x;
}

rb_node *rb_find(rb_tree *t, uint32_t key)
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

bool rb_remove(rb_tree *t, uint32_t key)
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
    while(y != t->nil && x == y->right){
        x = y;
        y = y->p;
    }
    return y;
}

rb_node *rb_predecessor(rb_tree *t, rb_node *x)
{
    if(x->left != t->nil)
        return rb_subtree_max(t, x->left);
    rb_node *y = x->p;
    while(y != t->nil && x == y->left){
        x = y;
        y = y->p;
    }
    return y;
}

//___________________________________________________________________________//
void rb_print(rb_tree *t, rb_node *n)
{
    if(n == t->nil)
        return;
    else{
        rb_print(t, n->left);
        fprintf(stderr, "%10u [%10u] | ", n->distance, n->max_autonomy);
        for(int i = 0; i < n->p_size; i++)
            fprintf(stderr, "%u/%d ", n->parking[i].autonomy, n->parking[i].count);
        fprintf(stderr, "\n");
        rb_print(t, n->right);
    }
}

void h_print(heap *h)
{
    fprintf(stderr, "lenght: %u size: %3u\n", h->heap_lenght, h->heap_size);
    for(int i=0; i<h->heap_size; i++)
        fprintf(stderr, "%10u | %3u | %3u\n", h->min_h[i].es_dist, h->min_h[i].key, h->min_h[i].tree_node->distance);
}

void h_print_visited(heap_node *v, int len)
{
    fprintf(stderr, " key | pre | dist\n-----|-----|-----\n");
    fprintf(stderr, "%4u | SRC | %u\n", v[0].key, v[0].es_dist);
    for(int i=1; i<len; i++){
        fprintf(stderr, "%4u |", v[i].key);
        if(v[i].prev == NULL)
            fprintf(stderr, " NULL|");
        else fprintf(stderr, "%4u |", v[i].prev->key);
        fprintf(stderr, " %u\n", v[i].es_dist);
    }
        
}
