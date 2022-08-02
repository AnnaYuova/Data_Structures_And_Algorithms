#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  int data;
  struct node *left;
  struct node *right;
  struct node *parent;
}node;

typedef struct splay_tree {
  struct node *root;
}splay_tree;

node* new_node(int data) {
  node *n = malloc(sizeof(node));
  n->data = data;
  n->parent = NULL;
  n->right = NULL;
  n->left = NULL;

  return n;
}

splay_tree* new_splay_tree() {
  splay_tree *t = malloc(sizeof(splay_tree));
  t->root = NULL;

  return t;
}

node* maximum(splay_tree *t, node *x) {
  while(x->right != NULL)
    x = x->right;
  return x;
}

void left_rotate(splay_tree *t, node *x) {
  node *y = x->right;
  x->right = y->left;
  if(y->left != NULL) {
    y->left->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == NULL) { //x is root
    t->root = y;
  }
  else if(x == x->parent->left) { //x is left child
    x->parent->left = y;
  }
  else { //x is right child
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void right_rotate(splay_tree *t, node *x) {
  node *y = x->left;
  x->left = y->right;
  if(y->right != NULL) {
    y->right->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == NULL) { //x is root
    t->root = y;
  }
  else if(x == x->parent->right) { //x is left child
    x->parent->right = y;
  }
  else { //x is right child
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
}

void splay(splay_tree *t, node *n) {
  while(n->parent != NULL) { //node is not root
    if(n->parent == t->root) { //node is child of root, one rotation
      if(n == n->parent->left) {
        right_rotate(t, n->parent);
      }
      else {
        left_rotate(t, n->parent);
      }
    }
    else {
      node *p = n->parent;
      node *g = p->parent; //grandparent

      if(n->parent->left == n && p->parent->left == p) { //both are left children
        right_rotate(t, g);
        right_rotate(t, p);
      }
      else if(n->parent->right == n && p->parent->right == p) { //both are right children
        left_rotate(t, g);
        left_rotate(t, p);
      }
      else if(n->parent->right == n && p->parent->left == p) {
        left_rotate(t, p);
        right_rotate(t, g);
      }
      else if(n->parent->left == n && p->parent->right == p) {
        right_rotate(t, p);
        left_rotate(t, g);
      }
    }
  }
}

void insertSplay(splay_tree *t, node *n) {
  node *y = NULL;
  node *temp = t->root;
  while(temp != NULL) {
    y = temp;
    if(n->data < temp->data)
      temp = temp->left;
    else
      temp = temp->right;
  }
  n->parent = y;

  if(y == NULL) //newly added node is root
    t->root = n;
  else if(n->data < y->data)
    y->left = n;
  else
    y->right = n;

  splay(t, n);
}

node* search(splay_tree *t, node *n, int x) {
  if(x == n->data) {
    splay(t, n);
    return n;
  }
  else if(x < n->data)
    return search(t, n->left, x);
  else if(x > n->data)
    return search(t, n->right, x);
  else
    return NULL;
}


void inorder(splay_tree *t, node *n) {
  if(n != NULL) {
    inorder(t, n->left);
    printf("%d\n", n->data);
    inorder(t, n->right);
  }
}

int main() {
  splay_tree *t = new_splay_tree();

  /*node *a, *b, *c, *d, *e, *f, *g, *h, *i, *z;
  a = new_node(100);
  b = new_node(50);
  c = new_node(200);
  d = new_node(40);
  e = new_node(30);
  f = new_node(20);
  g = new_node(150);
  h = new_node(250);
  i = new_node(33);*/

  insertSplay(t, new_node(100));
  insertSplay(t, new_node(50));
  insertSplay(t, new_node(200));
  insertSplay(t,  new_node(40));
  insertSplay(t, new_node(30));
  insertSplay(t, new_node(20));
  insertSplay(t, new_node(150));
  insertSplay(t, new_node(250));
  insertSplay(t, new_node(33));
  
  inorder(t, t->root);
search(t,new_node(100) ,100);



  return 0;
}

