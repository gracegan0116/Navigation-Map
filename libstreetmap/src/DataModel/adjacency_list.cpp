#include "adjacency_list.h"
#include "MapData.h"
#include "m2.h"
#include <math.h>

double x_from_lon(double lon);
double y_from_lat(double lat);
double lon_from_x(double x);
double lat_from_y(double y);

adjacency_list::adjacency_list(std::vector<Edge> const &edges, int N){
        size = N;
        // resize the vector to N elements of type vector<Pair>
        adjList.resize(N);

        // add edges to the directed graph
        for (auto &edge: edges)
        {
                int src = edge.src;
                int dest = edge.dest;
                int weight = edge.weight;

                // insert at the end
                adjList[src].push_back(std::make_pair(dest, weight));

                // Uncomment below line for undirected graph
                // adjList[dest].push_back(make_pair(src, weight));
        }
}
    
void adjacency_list::printGraph(adjacency_list const &graph, int N)
{
	for (int i = 0; i < N; i++)
	{
		// print all neighboring vertices of given vertex
		for (intPair v : graph.adjList[i])
			std::cout << "(" << i << ", " << v.first <<", " << v.second << ") ";
		std::cout << std::endl;
	}
}