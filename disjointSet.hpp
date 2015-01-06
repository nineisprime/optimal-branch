#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

//=================
// Disjoint Set  data structure
// author: Min Xu
// 
// SetElem* findSet(SetElem*);
// path compression
//
// bool sameSet(SetElem* x, SetElem* y);
//
// void UnionSet(SetElem* x, SetELem* y);
// union by rank. 
//
//=================


struct SetElem {
  SetElem* parent;
  int rank;
  int id;

  SetElem() : rank(0), parent(NULL) {}

  SetElem(int id) : rank(0), parent(NULL), id(id) {}

};

SetElem* findSet(SetElem*) ;

bool sameSet(SetElem* x, SetElem* y){
  SetElem* x_root;
  SetElem* y_root;

  x_root = findSet(x);
  y_root = findSet(y);

  return(x_root == y_root);
}

SetElem* findSet(SetElem* x) {
  if (x->parent == NULL){
    return(x);
  } else {
    x->parent = findSet(x->parent);
    return(x->parent);
  }
}

void UnionSet(SetElem* x, SetElem* y){
  
  SetElem* x_root;
  SetElem* y_root;

  x_root = findSet(x);
  y_root = findSet(y);
  
  if (x_root == y_root)
    return;
  
  if (x_root->rank < y_root->rank) {
    x_root->parent = y_root;
  } else if (x_root->rank > y_root->rank) {
    y_root->parent = x_root;
  } else {
    y_root->parent = x_root;
    x_root->rank = x_root->rank+1;
  }
  return;
}


#endif
