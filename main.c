#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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

//------------------------------ PROTOTIPI ------------------------------//

rb_node *rb_new_node(rb_tree *t, rb_color c, int key);
void rb_delete_node(rb_node *n);
void rb_init(rb_tree *t);
void rb_delete_subtree(rb_tree *t, rb_node *n);
void rb_deinit(rb_tree *t);
void rb_left_rotate(rb_tree *t, rb_node *x);
void rb_right_rotate(rb_tree *t, rb_node *x);
rb_node *rb_insert(rb_tree *t, int key);
rb_node *rb_insert_node(rb_tree *t, rb_node *z);
void rb_insert_fixup(rb_tree *t, rb_node *z);





void rb_print(rb_tree *t, rb_node *n);

//------------------------------ MAIN ------------------------------//

int main()
{
    rb_tree a ;

    rb_init(&a);
    
    rb_insert(&a, 23);
    rb_insert(&a, -23);
    rb_insert(&a, 4555);
    rb_insert(&a, 0);


    rb_print(&a, a.root);
    rb_deinit(&a);


    return 0;
}

//------------------------------ DICHIARAZIONI ------------------------------//

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

void rb_delete_subtree(rb_tree *t, rb_node *n)
{
    if(n == t->nil)
        return;
    else if(n->p != t->nil){
        if(n == n->p->left)
            n->p->left = t->nil;
        else
            n->p->right = t->nil;
    }else if(n == t->root){
        t->root = t->nil;
    }

    rb_node *left = n->left;
    if(left != t->nil)
        left->p = t->nil;

    rb_node *right = n->right;
    if(right != t->nil)
        right->p = t->nil;

    rb_delete_node(n);
    rb_delete_subtree(t, left);
    rb_delete_subtree(t, right);
}

void rb_deinit(rb_tree *t)
{
    rb_delete_subtree(t, t->root);
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
        x->p->left = y;
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
    rb_node *pre = t->nil;
    rb_node *curr = t->root;

    while(curr != t->nil){
        pre = curr;
        if(z->distance == curr->distance){
            return NULL;
        }else if(z->distance < curr->distance){
            curr = curr->left;
        }else{
            curr = curr->right;
        }
    }
    
    z->p = pre;
    if(pre == t->nil){
        t->root = z;
    }else if(z->distance < pre->distance){
        pre->left = z;
    }else{
        pre->right = z;
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

void rb_print(rb_tree *t, rb_node *n)
{
    if(n == t->nil)
        return;
    else{
        rb_print(t, n->left);
        printf("%d\n", n->distance);
        rb_print(t, n->right);
    }
}