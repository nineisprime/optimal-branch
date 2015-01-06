#ifndef OPTIMUM_BRANCHING_HPP
#define OPTIMUM_BRANCHING_HPP

/*=============

OptimumBranching.hpp
optimumBranching

author: Min Xu


ASSUME: input graph is fully connected

IN:
-- "graph_edges" list<pair<int,int> >, list of directed edge pairs
-- "edge_weight" list<double>
-- "n"

OUT:
-- "opt_branch" list<pair<int,int> >

========*/

/*================
OptimumBranching version 2

IN:
-- "weights" <vector<vector<double>> adjacency matrix of the graph
             weights[1][2] is weight of edge 1->2
-- "n" int, number of nodes in graph

OUT:
-- "opt_branch", list<pair<int,int>> list of directed edges 
             first->second 

=============*/

#include "EdgeQueue.hpp"
#include "disjointSet.hpp"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <assert.h>

/*==========
branch

IN:
--"edges_in_list" of type vector< list<Edge> >
for each node, list from-node of edges INTO node with weight

--"n" int, is number of nodes in the graph

OUT:
--"H" of type vector< list<Edge> >
for each node, list of to-node of edges OUT OF node with weight

--"rset" of type list<int>
==*/

void optimumBranching(list<pair<int,int> > graph_edges, list<double> edge_weights, int n, list<pair<int,int> > &opt_branch);

void optimumBranching(vector<vector<double> > &weights, int n, list<pair<int,int> > &opt_branch);

void branch(vector<list<Edge> > &edges_in_list, int n,
	    vector<list<Edge> > &H, list<int> &branch_roots);

void rootSpread(vector<list<Edge> > &H, int n, list<int> &branch_roots, 
		list<Edge> &opt_branch);

void rootSpreadHelper(vector<list<Edge> > &H, int cur_node, list<Edge> &opt_branch,
		      vector<bool> &visited);

void edgeList2PairList(list<Edge> &edge_list, list<pair<int,int> > &pair_list);

// Body of Functions

void optimumBranching(list<pair<int,int> > graph_edges, list<double> edge_weights,
		      int n, list<pair<int,int> > &opt_branch){

  vector<list<Edge> > edges_in_list;
  edges_in_list.resize(n);

  list<pair<int,int> >::iterator it1;
  list<double>::iterator it2;
  
  for(it1=graph_edges.begin(), it2=edge_weights.begin(); it1!=graph_edges.end(); it1++, it2++){
    Edge tmp(it1->first, it1->second, *it2);

    edges_in_list[it1->second].push_back(tmp);
  }

  vector<list<Edge> > H;
  list<int> branch_roots;
  branch(edges_in_list, n, H, branch_roots);
  list<Edge> opt_edges;
  rootSpread(H, n, branch_roots, opt_edges);
  
  edgeList2PairList(opt_edges,opt_branch);
}

void optimumBranching(vector<vector<double> > &weights, int n, list<pair<int,int> > &opt_branch){
  
  vector<list<Edge> > edges_in_list;
  edges_in_list.resize(n);

  for (int i=0; i<n; i++){
    for (int j=0; j<n; j++){
      if (j==i)
	continue;
      
      Edge tmp(i,j,weights[i][j]);
      edges_in_list[j].push_back(tmp);
    }
  }

  vector<list<Edge> > H;
  list<int> branch_roots;
  branch(edges_in_list, n, H, branch_roots);
  list<Edge> opt_edges;
  rootSpread(H, n, branch_roots, opt_edges);
  
  edgeList2PairList(opt_edges,opt_branch);
}

void edgeList2PairList(list<Edge> &opt_edges, list<pair<int,int> > &opt_branch){
  assert(opt_branch.empty());

  list<Edge>::iterator it1 = opt_edges.begin();
  for (;it1!=opt_edges.end(); it1++){
    pair<int,int> new_pair(it1->from, it1->to);
    opt_branch.push_back(new_pair);
  }
}


void branch(vector<list<Edge> > &edges_in_list, int n,
	    vector<list<Edge> > &H, list<int> &branch_roots){
  
  //READ:
  // if node index is k
  // "findSet(scompns[k])->id" gives the id of the component with k 
   
  list<int> roots;
  vector<SetElem*> scompns; 
  vector<SetElem*> wcompns;
 
  vector<Edge> enter;
  vector<int> min_vec;
 
  vector<EdgeQueue*> all_edge_qs;

  // Initialization Data Structures
  scompns.resize(n);
  wcompns.resize(n);
  all_edge_qs.resize(n);
  enter.resize(n);
  min_vec.resize(n);

  for (int i=0; i<n; i++){
    scompns[i] = new SetElem(i);
    wcompns[i] = new SetElem(i);
    all_edge_qs[i] = new EdgeQueue();
    
    initEQ(all_edge_qs[i], edges_in_list[i]);

    roots.push_back(i);
    min_vec[i] = i;

    assert(enter[i].isNull());
  }
  
  // Output Data structures
  H.resize(n);

  list<int> rset;
  
  int k;
  Edge cur_in_edge;
  int vertex_in_wcomp;
  Edge fixed_null_edge;


  while (!roots.empty()){
    k = roots.front();
    roots.pop_front();

    //DEBUG
    //cout << k << "\n";

    assert(findSet(scompns[k])->id == k);
    assert(enter[k].isNull());

    if (all_edge_qs[k]->empty()){
      rset.push_back(k);
      continue;
    }

    cur_in_edge = maxEQ(all_edge_qs[k]);

    //cout << "hi\n";

    if (sameSet(scompns[k], scompns[cur_in_edge.from])) {
      roots.push_back(k);
      continue;
    }

    H[cur_in_edge.from].push_back(cur_in_edge);

    assert(sameSet(wcompns[k], wcompns[cur_in_edge.to]));

    if (!sameSet(wcompns[k], wcompns[cur_in_edge.from])) {
      UnionSet(wcompns[cur_in_edge.from], wcompns[k]);
      
      assert(!sameSet(scompns[k],scompns[cur_in_edge.from]));
      
      enter[k] = cur_in_edge;

    } else {

      assert(enter[k].isNull());
      double val = numeric_limits<double>::max();
      Edge edge_in_wcomp = cur_in_edge;

      while (!edge_in_wcomp.isNull()){

	int x = findSet(scompns[edge_in_wcomp.from])->id;
	int y = findSet(scompns[edge_in_wcomp.to])->id;

	if (edge_in_wcomp.weight < val){
	  val = edge_in_wcomp.weight;
	  vertex_in_wcomp = y;
	}
	edge_in_wcomp = enter[x];
      }
      
      offsetEQ(all_edge_qs[k], val - cur_in_edge.weight);
      edge_in_wcomp = enter[findSet(scompns[cur_in_edge.from])->id];

      //DEBUG
      //cur_in_edge.print();
      assert(enter[k].isNull());

      while (!edge_in_wcomp.isNull()){
	
	//edge_in_wcomp.print();

	int x = findSet(scompns[edge_in_wcomp.from])->id;
	int y = findSet(scompns[edge_in_wcomp.to])->id;
	
	/* moved to later */
	//if (sameSet(scompns[x],scompns[k])){
	//  break;
	//}
	
	assert(!sameSet(scompns[y],scompns[k]));
	assert(!sameSet(scompns[x],scompns[y]));

	offsetEQ(all_edge_qs[y], val - edge_in_wcomp.weight);
	
	UnionSet(scompns[y], scompns[k]);
	enter[findSet(scompns[k])->id] = fixed_null_edge;

	//Queue Union
	EdgeQueue* tmp_q1 = all_edge_qs[y];
	EdgeQueue* tmp_q2 = all_edge_qs[k];
	EdgeQueue* new_q = UnionQueue(tmp_q1, tmp_q2);
	
	assert(tmp_q1 != tmp_q2);

	all_edge_qs[y] = new_q;
	all_edge_qs[k] = new_q;

	delete tmp_q1;
	delete tmp_q2;

	k = findSet(scompns[k])->id;
	edge_in_wcomp = enter[x];

      }

      //DEBUG
      int new_scompn_id = findSet(scompns[k])->id;

      min_vec[new_scompn_id] = min_vec[vertex_in_wcomp];
      roots.push_back(new_scompn_id);
      enter[new_scompn_id] = fixed_null_edge;

    }
   
  }


  int cur_rset;
  while (!rset.empty()){
    cur_rset = rset.front();
    rset.pop_front();
    branch_roots.push_back(min_vec[cur_rset]);
    delete all_edge_qs[cur_rset];
  }

  for (int i=0; i<n; i++){
    delete scompns[i];
    delete wcompns[i];
  }

  
}

// currently assumes "branch_roots" has one element

void rootSpread(vector<list<Edge> > &H, int n, list<int> &branch_roots, 
		list<Edge> &opt_branch){
  
  int root = branch_roots.front();
  branch_roots.pop_front();

  assert(branch_roots.empty());

  vector<bool> visited;
  visited.resize(n);
  for (int i=0; i<n; i++){
    visited[i] = 0;
  }

  rootSpreadHelper(H, root, opt_branch, visited);
}

void rootSpreadHelper(vector<list<Edge> > &H, int cur_node, 
		      list<Edge> &opt_branch, vector<bool> &visited){

  if (visited[cur_node]){
    return;
  }

  visited[cur_node] = 1;

  list<Edge>::iterator it;
  for (it=H[cur_node].begin(); it!=H[cur_node].end(); it++){
    assert(cur_node == it->from);
    
    if (!visited[it->to]){
      opt_branch.push_back(*it);
    }

    rootSpreadHelper(H, it->to, opt_branch, visited);
  }
}
  

#endif
