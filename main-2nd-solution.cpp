//
//  main-2nd-solution.cpp
//  CppGraphTest
//

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <queue>

using namespace std;


struct Edge {
  const int source;
  const int destination;
};


void printEdges(const vector<Edge>& in)
{
  cout << "*****printEdges*****start\n";
  cout << "size=" << in.size() << "\n";
  for(auto const e : in)
  {
    cout << "(" << e.source << "," << e.destination << ")\n";
  }
  cout << "*****printEdges*****end\n";
}


template<template<class,class...> class C, class... Args>
void printAdjs(const C<Args...>& adj)
{
  cout << "*****printAdjs*****start\n";
  cout << "size=" << adj.size() << "\n";
  for(auto const& e : adj)
  {
    cout << e.first << ": ";
    for (auto const obj : e.second)
    {
      cout << obj << " ";
    }
    cout << "\n";
  }
  cout << "*****printAdjs*****end\n";
}


template< typename C>
void printContainer(const C& in, const string& s)
{
  cout << "*****printContainer*****start:\t";
  cout << s << "\n";
  cout << "size=" << in.size() << "\n";
  for(auto const e : in)
  {
    cout << e << " ";
  }
  cout << "\n";
  cout << "*****printContainer*****end:\t";
  cout << s << "\n";
}


// final_act allows you to ensure something gets run at the end of a scope
template <class F>
class final_act
{
public:
  explicit final_act(F f) noexcept 
    : f_(std::move(f)), invoke_(true) {}

  final_act(final_act&& other) noexcept 
    : f_(std::move(other.f_)), 
      invoke_(other.invoke_)
  {
    other.invoke_ = false;
  }

  final_act(const final_act&) = delete;
  final_act& operator=(const final_act&) = delete;

  ~final_act() noexcept
  {
    if (invoke_) f_();
  }

private:
  F f_;
  bool invoke_;
};

template <class F>
inline final_act<F> finally(const F& f) noexcept
{
  return final_act<F>(f);
}

template <class F>
inline final_act<F> finally(F&& f) noexcept
{
  return final_act<F>(std::forward<F>(f));
}


// namespace for implemetation of BFS graph algorithm
namespace MyGraph {

// Bfs_label is an alias to the type of "source" data member of "Edge" struct (without const qualifier)
using Bfs_label = remove_const<decltype(Edge{}.source)>::type;
// Vertex's colors as per BFS in CLRS book
enum class Bfs_color { white, gray, black };

class BfsGraph
{
private:
  // nested class definition, only BfsGraph class should be able to manipulate instances of BfsVertex
  class BfsVertex
  {
  private:
    // data members
    int index; // index of vertex, continually increasing: 0,1,2,3,...
    Bfs_label label; // real name/value of vertex
    Bfs_color color; // color of vertex
    BfsGraph* graph; // pointer to a BfsGraph presenting the whole graph
    vector<int> adjs; // adjacency list of this vertex (as per index, not as per label/name/value)
  public:
    // construct a BfsVertex from an index and an label
    BfsVertex(int idx, Bfs_label val) : index{idx}, label{val}, color{Bfs_color::white}, graph{nullptr} {}
    // Rule of five: For current desgin, copy ctor and copy operator and move operator are not used, so mark them as deleted.
    // TODO: Revise the design and provide implementation for them if necessary.
    BfsVertex(const BfsVertex&) = delete;
    BfsVertex& operator=(const BfsVertex&) = delete;
    BfsVertex(BfsVertex&&) = default;
    BfsVertex& operator=(BfsVertex&&) = delete;
    ~BfsVertex() = default;

    int getIndex() const
    {
      return index;
    }
    Bfs_label getLabel() const
    {
      return label;
    }
    Bfs_color getColor() const
    {
      return color;
    }
    BfsGraph* getGraph() const
    {
      return graph;
    }
    const vector<int>& getAdjs() const
    {
      return adjs;
    }

    // initialize graph ptr to point to the whole graph data structure
    void initGraphPtr(BfsGraph* g)
    {
      graph = g;
    }

    // initialize adjacency list based on egdes of the graph (as per index, not as per label/name/value)
    void initAdjs(const vector<pair<int,int>>& v)
    {
      for (auto const& e : v)
      {
        if (index == e.first)
        {
          adjs.push_back(e.second);
        }
        else if (index == e.second)
        {
          adjs.push_back(e.first);
        }
      }
    }
    void changeColor(Bfs_color c)
    {
      color = c;
    }
    bool operator< (const BfsVertex& rhs) const
    {
      return this->index < rhs.index;
    }
  };

  // data member
  vector<BfsVertex> bfsGraph; // a BfsGraph is a vector of BfsVertex

public:
  // construct a BfsGraph from a vector of Edges, should be a explicit ctor to prevent implicit conversion
  // input: E edges
  // time complexity: O(V.E) = O(E^(3/2)), with V^2 <= E
  // invariant: all BfsVertex are white and have correct pointer to a BfsGraph presenting the whole graph
  explicit BfsGraph(const vector<Edge>& in)
  {
    // s is a set of unique label/vertex
    set<Bfs_label> s;
    for (auto const e : in) // O(ElogE)
    {
      s.insert(e.source);
      s.insert(e.destination);
    }
    // Now there is V vertex in s, with V^2 <= E

    // m is a map of unique label/vertex (label is key_type, index is vale_type)
    map<Bfs_label,int> m;
    for (auto it = s.begin(); it != s.end(); it++) // O(VlogV)
    {
      m.insert({*it, distance(s.begin(),it)});
    }
    //for (auto e : m) { cout << "{" << e.first << "," << e.second << "}\n"; } //debug

    // edge_idx is a vector of edges-like information, but as per index, not as per label/name/value
    vector<pair<int,int>> edge_idx;
    edge_idx.reserve(in.size());
    for (auto const e : in) // O(ElogV)
    {
      //cout << "    [" << e.source << "," << e.destination << "]\n";
      auto it1 = m.find(e.source);
      auto it2 = m.find(e.destination);
      if (it1 != m.end() && it2 != m.end())
      {
        edge_idx.push_back({it1->second,it2->second});
      }
    }
    //for (auto e : edge_idx) { cout << "[" << e.first << "," << e.second << "]\n"; } //debug

    // create instances of BfsVertex for bfsGraph vector
    for (auto const& e : m) // O(V)
    {
      BfsVertex ver(e.second, e.first);
      bfsGraph.push_back(move(ver));
    }

    // initialize instances of BfsVertex for bfsGraph vector
    for (auto& vertex : bfsGraph) // O(V.E) !!!
    {
      vertex.initGraphPtr(this);
      vertex.initAdjs(edge_idx);
    }
  }

  // Rule of five: For current desgin, no special member functions are used, so mark them as deleted.
  // TODO: Revise the design and provide implementation for them if necessary.  
  BfsGraph(const BfsGraph&) = delete;
  BfsGraph& operator=(const BfsGraph&) = delete;
  BfsGraph(BfsGraph&&) = delete;
  BfsGraph& operator=(BfsGraph&&) = delete;
  ~BfsGraph() = default;


private:
  void colorVertex(int i, Bfs_color c)
  {
    bfsGraph[i].changeColor(c);
  }

  // change color of all vertices to white after doing BFS to ensure invariant of BfsGraph
  void resetColor()
  {
    for (unsigned i=0; i<bfsGraph.size(); ++i)
    {
      colorVertex(i, Bfs_color::white);
    }
  }

public:
  // input: E edges
  //        V vertices, with V^2 <= E
  // time complexity: O(VlogV+E) with V = O(E^(1/2))
  bool has_cycle()
  {
    // change color of all vertices to white after doing BFS
    auto action = finally([&] { 
      resetColor(); 
      //cout << "BFS is done. Colors changed to white\n"; //debug
      //cout << *this; //debug
      }
    );  // establish exit action

    if (bfsGraph.empty())
    {
      return true;
    }

    vector<int> discovered_vertices;

    // BFS algorithm in CLRS book does not consider disjoint sets,
    // so remain_vertices is newly introduced to run BFS for each disjoint set in the graph 
    set<int> remain_vertices;
    for (auto const& e : bfsGraph) // O(VlogV)
    {
      remain_vertices.insert(e.getIndex());
    }

    queue<int> q;
    
    while (!remain_vertices.empty()) // there is a disjoint set to start BFS
    {
      //printContainer(remain_vertices, "remain_vertices (as per index)"); //debug
      auto index = *(remain_vertices.cbegin());
      //cout << "Starting BFS from index(" << index << ")_label(" << bfsGraph[index].getLabel() << ") ..........\n"; //debug
      q.push(index);
      remain_vertices.erase(index);

      while (!q.empty()) // main BFS algorithm in CLRS book for each disjoint set
      {
        auto curVertex = q.front();
        //cout << "Discovering vertex with index(" << curVertex << ")_label(" << bfsGraph[curVertex].getLabel() << ") ..........\n"; //debug
        q.pop();
        for (auto const i : bfsGraph[curVertex].getAdjs())
        {
          switch (bfsGraph[i].getColor())
          {
            case Bfs_color::white: // vertex is white, color it gray and push the index to queue
            {
              colorVertex(i, Bfs_color::gray);
              q.push(i);
              break;
            }
            case Bfs_color::gray: // while we are discovering a gray vertex "curVertex",
                                  // we see an edge from "curVertex" to another gray vertex "i",
                                  // that means the graph has a cycle
            {
              //cout << "The vertex " << bfsGraph[i].getLabel() << " causes cycle\n"; //debug
              return true;
            }
            case Bfs_color::black:
            {
              break;
            }
            default:
            break;
          }
        }
        colorVertex(curVertex, Bfs_color::black);
        discovered_vertices.push_back(curVertex);
        remain_vertices.erase(curVertex);
        //printContainer(discovered_vertices, "discovered_vertices (as per index)"); //debug
        //printContainer(remain_vertices, "remain_vertices (as per index)"); //debug
        //cout << *this; //debug
      }
    }

    return false; // BFS is done and no cycle is found
  }

  // friend declaration for non-member functions to access private data of BfsGraph and BfsVertex
  friend std::ostream& operator<< (std::ostream& os, const BfsGraph::BfsVertex& bfsVertex);
  friend std::ostream& operator<< (std::ostream& os, const BfsGraph& bfsGraph);
};

std::ostream& operator<< (std::ostream& os, const BfsGraph::BfsVertex& bfsVertex) {
  os << bfsVertex.getLabel();

  auto printColor = [&](const Bfs_color c){ // function object used to print color of vertex
    os << "-";
    switch (c)
    {
      case Bfs_color::white:
        os << "w";
        break;
      case Bfs_color::gray:
        os << "g";
        break;
      case Bfs_color::black:
        os << "b";
        break;
      default:
        break;
    }
  };
  printColor(bfsVertex.getColor());

  os << "\t: ";
  os << "graph=" << bfsVertex.getGraph() << " ";
  os << "adjsSize=" << bfsVertex.getAdjs().size() << " ";
  for (auto const i : bfsVertex.getAdjs())
  {
    auto ptr = bfsVertex.getGraph();
    auto& vertex = ptr->bfsGraph[i];
    os << &vertex << "(" << vertex.getLabel();
    printColor(vertex.getColor());
    os << ") ";
  }
  //os << "\n";
  return os;
}

std::ostream& operator<< (std::ostream& os, const BfsGraph& bfsGraph) {
  cout << "*****printBfsGraph*****start\n";
  cout << "size=" << bfsGraph.bfsGraph.size() << "\n";
  for (auto const& e : bfsGraph.bfsGraph)
  {
    os << e << "\n";
  }
  cout << "*****printBfsGraph*****end\n";
  return os;
}

} // namespace BfsGraph


void printBfsEdges(const vector<Edge>& in)
{
  cout << "*****printBfsEdges*****start\n";
  cout << "size=" << in.size() << "\n";
  for(auto const e : in)
  {
    cout << "(" << e.source << "," << e.destination << ")\n";
  }
  cout << "*****printBfsEdges*****end\n";
}

// input: E edges
// time complexity: O(V.E) = O(E^(3/2)), with V = O(E^(1/2))
bool has_cycle(const vector<Edge> &edges) {
  // Implement your solution here to detect whether this undirected graph contains a cycle or not.

  //step1: hande special cases
  //printEdges(edges); //debug
  if (edges.empty())
  {
    return false;
  }
  for (auto const e : edges)
  {
    if (e.source == e.destination)
    {
      //cout << "The vertex " << e.source << " causes cycle\n"; //debug
      return true;
    }
  }

  //step2: create a BfsGraph and invoke has_cycle() member function
  MyGraph::BfsGraph mygraph{edges}; // O(V.E) = O(E^(3/2)), with V = O(E^(1/2))
  //cout << mygraph; //debug
  return mygraph.has_cycle();       // O(VlogV+E) with V = O(E^(1/2))
}


void report_results(bool cycle_found) {
  if (cycle_found)
    cout << "Graph contains a cycle\n";
  else
    cout << "Graph does NOT contain any cycles\n";
}


void check_for_cycles(const vector<Edge> &edges) {
  bool cycle_found = has_cycle(edges);
  report_results(cycle_found);
}


int main(int argc, const char *argv[]) {

  const vector<Edge> edges_with_cycle    = { {0, 1}, {0, 2}, {0, 3}, {1, 4}, {1, 5}, {4, 8}, {4, 9}, {3, 6}, {3, 7}, {6, 10}, {6, 11}, {5, 9} };
//{ {0,6},{1,3},{1,4},{2,5},{2,6},{2,8},{5,8},{7,8} };
//{ {1,2},{0,6},{1,5},{1,8},{2,7},{2,8},{3,5},{4,6},{4,7} };
//{ {0,6},{1,6},{3,4},{2,8},{6,8},{1,9},{7,9},{8,9} };
//{ {0,4},{0,5},{1,3},{1,4},{1,9},{2,8},{3,9},{5,8},{6,10},{7,10} };
//{ {0,4},{0,7},{1,4},{1,6},{2,7},{3,6},{1,9},{3,9},{7,9},{8,9} };
//{ {0,5},{0,6},{0,7},{1,6},{2,6},{2,7},{2,9},{3,6},{4,7},{4,8} };
//{ {1,2},{0,6},{1,4},{2,4},{2,5},{2,6},{2,7},{2,9},{2,10},{2,11},{3,10},{5,8},{5,11},{6,11},{7,8},{7,11} };
//{ {1,2},{1,3},{1,5},{2,7},{3,6},{3,8},{4,9},{0,10},{2,10},{4,10},{6,10},{9,10},{9,11} };
//{ {1,3},{1,4},{1,6},{2,4},{0,8},{1,8},{3,7},{4,7},{5,8} };
//{ {1,2},{2,3},{3,4},{4,5},{5,6},{6,1} };
//{ {0,5},{0,6},{1,3},{1,4},{2,4},{2,7},{2,9},{3,8},{3,9},{5,8} };
//{ {0,5},{0,6},{1,3},{1,4},{2,4},{2,7},{2,9},{3,8},{5,8},{7,7} };
  const vector<Edge> edges_without_cycle = { {0, 1}, {0, 2}, {0, 3}, {1, 4}, {1, 5}, {4, 8}, {4, 9}, {3, 6}, {3, 7}, {6, 10}, {6, 11} };
//{ {0,1},{0,2},{1,3},{1,6},{2,4},{2,5} };
//{ {0,5},{0,7},{1,7},{2,6},{2,7} };
//{ {0,4},{4,7} };
//{ {7,11} };
//{        };
//{ {0,2},{1,2},{1,4},{1,5},{2,6} };
//{ {1,2},{1,4},{1,5},{0,8},{3,8},{4,8} };
//{ {0,4},{0,6},{1,6},{2,6},{3,6},{4,7},{2,8},{5,8} };
  
  check_for_cycles(edges_with_cycle);
  check_for_cycles(edges_without_cycle);

  return 0;
}
