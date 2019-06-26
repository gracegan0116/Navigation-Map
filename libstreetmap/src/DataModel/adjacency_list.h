#ifndef ADJACENCY_LIST_H
#define ADJACENCY_LIST_H
#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <cfloat>

// data structure to store graph edges
struct Edge {
        int src, dest, weight;
};

typedef std::pair<int, int> intPair;

class adjacency_list{
    
public:
    // Graph Constructor
    adjacency_list(std::vector<Edge> const &edges, int N);
    
    // print adjacency list representation of graph
    void printGraph(adjacency_list const &graph, int N);
    
private:
    	// construct a vector of vectors of Pairs to represent an adjacency list
	std::vector<std::vector<intPair>> adjList;
        int size; // size of list
};
#endif 