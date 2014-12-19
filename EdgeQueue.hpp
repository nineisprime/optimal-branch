#ifndef EDGE_QUEUE_HPP
#define EDGE_QUEUE_HPP

#include <list>
#include <iostream>
#include <cstdlib>

/* NOTE:

each node (int) is associated with pointer to an EPQ

each EPQ contains two lists, one is list of edges (from-node), one is list of doubles (weights), an offset

init, union, max, add

*/ 

using namespace std;

struct Edge {
  int from;
  int to;
  double weight;
  int null_edge;

  Edge(int from, int to, double weight) : from(from),
					  to(to), weight(weight),
					  null_edge(0) {}

  Edge() : null_edge(1) {}
  
  
  void setEdge(Edge other) {
    from = other.from;
    to = other.to;
    weight = other.weight;
    null_edge = other.null_edge;
  }

  bool isNull(){
    return(null_edge);
  }


  bool operator< (const Edge &other) {
    return(from < other.from);
  }

  void print() {
    if (null_edge){
      cout << "NULL EDGE\n";
    } else {
      cout << "(" << from << " " << to << ") " << weight << "\n";
    }
  }

};


struct EdgeQueue {
  list<Edge> edge_list;
  double offset;

  bool empty() {
    return(edge_list.empty());
  }

  void print() {
    cout << "Queue:\n";
    list<Edge>::iterator it = edge_list.begin();
    for (;it!=edge_list.end();it++){
      it->print();
    }
  }

};

void initEQ(EdgeQueue* edge_q, list<Edge> incident_edges){
  incident_edges.sort();
  
  edge_q->edge_list = incident_edges;
  edge_q->offset = 0;
  return;
}

void offsetEQ(EdgeQueue* edge_q, double offset){
  edge_q->offset += offset;
}


// returns a NEW dynamically allocated Queue
// will NOT deallocate two input queues

EdgeQueue* UnionQueue(EdgeQueue* q1, EdgeQueue* q2){
  
  list<Edge>::iterator it1 = q1->edge_list.begin();
  list<Edge>::iterator it2 = q2->edge_list.begin();
  list<Edge> new_ls;

  while (it1 != q1->edge_list.end() && it2 != q2->edge_list.end()){
    if ( *it1 < *it2 ){
      it1->weight += q1->offset;
      new_ls.push_back(*it1);
      it1++;
    } else if (*it2 < *it1){
      it2->weight += q2->offset;
      new_ls.push_back(*it2);
      it2++;
    } else if ( (it1->weight + q1->offset) > (it2->weight + q2->offset) ){
      it1->weight += q1->offset;
      new_ls.push_back(*it1);
      it1++; it2++;
    } else {
      it2->weight += q2->offset;
      new_ls.push_back(*it2);
      it1++; it2++;
    }
  }

  while (it1 != q1->edge_list.end()){
    it1->weight += q1->offset;
    new_ls.push_back(*it1);
    it1++;
  }

  while (it2 != q2->edge_list.end()){
    it2->weight += q2->offset;
    new_ls.push_back(*it2);
    it2++;
  }

  EdgeQueue* res_edge_q = new EdgeQueue();
  initEQ(res_edge_q, new_ls);

  return(res_edge_q);
}


Edge maxEQ(EdgeQueue* edge_q){
  
  if (edge_q->empty()){
    cout << "ERROR: Edge Queue is empty, cannot delete \n";
    exit(0);
  }

  double best_val = -10000000;
  list<Edge>::iterator best_ix;
  Edge best_edge;

  list<Edge>::iterator it;
  for (it=edge_q->edge_list.begin(); it!=edge_q->edge_list.end(); it++){
    //it->print();
    if (it->weight > best_val){
      best_val = it->weight;
      best_ix = it;
      best_edge = *it;
    }
  }

  edge_q->edge_list.erase(best_ix);
  best_edge.weight += edge_q->offset;
  return(best_edge);
}




#endif
