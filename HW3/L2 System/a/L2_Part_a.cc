// Frome Here :
// https://www.geeksforgeeks.org/kruskals-minimum-spanning-tree-using-stl-in-c/
//
// C++ program for Kruskal's algorithm to find Minimum
// Spanning Tree of a given connected, undirected and
// weighted graph
#include<bits/stdc++.h>
using namespace std;
  
// Creating shortcut for an integer pair
typedef  pair<int, int> iPair;
  
// Structure to represent a graph
struct Graph
{
    int V, E;
    vector< pair<int, iPair> > edges;
  
    // Constructor
    Graph(int V, int E)
    {
        this->V = V;
        this->E = E;
    }
  
    // Utility function to add an edge
    void addEdge(int u, int v, int w)
    {
        edges.push_back({w, {u, v}});
    }
  
    // Function to find MST using Kruskal's
    // MST algorithm
    vector<pair<double, pair<int, int>>> kruskalMST();
};
  
// To represent Disjoint Sets
struct DisjointSets
{
    int *parent, *rnk;
    int n;
  
    // Constructor.
    DisjointSets(int n)
    {
        // Allocate memory
        this->n = n;
        parent = new int[n+1];
        rnk = new int[n+1];
  
        // Initially, all vertices are in
        // different sets and have rank 0.
        for (int i = 0; i <= n; i++)
        {
            rnk[i] = 0;
  
            //every element is parent of itself
            parent[i] = i;
        }
    }
  
    // Find the parent of a node 'u'
    // Path Compression
    int find(int u)
    {
        /* Make the parent of the nodes in the path
           from u--> parent[u] point to parent[u] */
        if (u != parent[u])
            parent[u] = find(parent[u]);
        return parent[u];
    }
  
    // Union by rank
    void merge(int x, int y)
    {
        x = find(x), y = find(y);
  
        /* Make tree with smaller height
           a subtree of the other tree  */
        if (rnk[x] > rnk[y])
            parent[y] = x;
        else // If rnk[x] <= rnk[y]
            parent[x] = y;
  
        if (rnk[x] == rnk[y])
            rnk[y]++;
    }
};
  
 /* Functions returns vector of the MST*/ 
  
vector< pair<double, pair<int, int> >> Graph::kruskalMST()
{
    double mst_wt = 0; // Initialize result
    vector< pair<double, pair<int, int> >> out; // vector of the tree

    // Sort edges in increasing order on basis of cost
    sort(edges.begin(), edges.end());
  
    // Create disjoint sets
    DisjointSets ds(V);
  
    // Iterate through all sorted edges
    vector< pair<int, iPair> >::iterator it;
    for (it=edges.begin(); it!=edges.end(); it++)
    {
        int u = it->second.first;
        int v = it->second.second;
        double w = it->first;
  
        int set_u = ds.find(u);
        int set_v = ds.find(v);
  
        // Check if the selected edge is creating
        // a cycle or not (Cycle is created if u
        // and v belong to same set)
        if (set_u != set_v)
        {
            // Current edge will be in the MST
            // so print it
            //cout << u << " ---- " << v << endl;
            out.push_back({w, {u, v}}); // nodes of the tree
  
            // Update MST weight
            mst_wt += it->first;
  
            // Merge two sets
            ds.merge(set_u, set_v);
        }
    }
  
    return out; //vector, links of tree
}
  
// Driver program to test above functions
int main()
{
    // Let us create weighted graph

    int V = 6, E = 12;
    Graph g(V, E);
  
    //  making the graph
    g.addEdge(1, 2, 50);
    g.addEdge(1, 3, 50);
    g.addEdge(1, 4, 186.6);

    g.addEdge(2, 3, 50);
    g.addEdge(2, 5, 100);
    g.addEdge(2, 6, 145.5);

    g.addEdge(3, 4, 193.2);
    g.addEdge(3, 5, 145.5);
    g.addEdge(3, 6, 186.6);

    g.addEdge(4, 5, 50);
    g.addEdge(4, 6, 50);

    g.addEdge(5, 6, 50);

    //cout << "Edges of MST are \n";
    vector< pair <double, pair<int, int> >> out = g.kruskalMST();
    
    int i = 0;
    cout << "the links are : "<<"\n";
    for(i;i<out.size();i++){
        cout << out[i].second.first << " -- " << out[i].first << " -- " << out[i].second.second  << endl;
    }
  
    return 0;
}