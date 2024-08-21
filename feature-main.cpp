//
//  feature-main.cpp
//  CppGraphTest
//
//  Created by lgb1hc on 21/08/2024.
//  Tested on Ubuntu 22.04.3 LTS
//  
//  DESCRIPTION: This program determines whether or not an undirected graph contains a cycle by doing:
//    1. Create a random collection of edges of an undirected graph.
//       (a default collection of edges defaultEdges can be used instead by calling createEdges(true))
//    2. Draw the graph (collection of edges) into out.svg file in current directory and open it.
//       (currently the graph is drawed as directed graph; next version will fix this to undirected)
//    3. Check if that graph contains a cycle or not via has_cycle function.
//
//  TODO:
//    1. createEdges is based on a C program https://stackoverflow.com/questions/12790337/generating-a-random-dag?rq=3.
//       Will change it to C++ style to handle error/exception correctly.
//    2. Consider to write unit tests to efficiently and conveniently check the correctness of solution for each further change.
//    3. Make the program portable for other OSes.
//    4. Change desgin of has_cycle to use a queue and a list of colored vertices (BFS in CLRS book) for optimization.
//    5. Research ways to draw undirected graph for testing. 


#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_map>
#include <set>
#include <map>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_PER_RANK 1 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 5
#define MIN_RANKS 3    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 5
#define PERCENT 30     /* Chance of having an Edge.  */

using namespace std;

struct Edge {
  const int source;
  const int destination;
};

bool operator< (const Edge& e1, const Edge& e2)
{
  return ((e1.source < e2.source) || (e1.destination < e2.destination));
}

// bool operator== (const Edge& e1, const Edge& e2)
// {
//  return ((e1.source == e2.source) && (e1.destination == e2.destination));
// }

namespace {
  const vector<Edge> defaultEdges
    { {0,4},{3,4},{2,5},{0,6},{1,8},{2,8},{3,8},{4,6},{5,6},{5,8} };
}

vector<Edge> createEdges (bool useDefault)
{
  int i, j, k, nodes = 0;
  srand (time (NULL));

  int ranks = MIN_RANKS
              + (rand () % (MAX_RANKS - MIN_RANKS + 1));

  ofstream myfile;
  myfile.open("graph.txt");

  vector<Edge> vec;
  string res {"{ "};
  string tmp;

  printf ("digraph {\n");
  myfile << "digraph {\n";

  if (useDefault)
  {
    for (auto const e : defaultEdges)
    {
      printf ("  %d - %d;\n", e.source, e.destination); /* An Edge.  */
      myfile << ("  " + to_string(e.source) + " -> " + to_string(e.destination) + ";\n");
      tmp = "{" + to_string(e.source) + "," + to_string(e.destination) + "},";
      res += tmp;
    }
  }
  else
  {
    for (i = 0; i < ranks; i++)
    {
      /* New nodes of 'higher' rank than all nodes generated till now.  */
      int new_nodes = MIN_PER_RANK
                      + (rand () % (MAX_PER_RANK - MIN_PER_RANK + 1));

      /* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
      for (j = 0; j < nodes; j++)
        for (k = 0; k < new_nodes; k++)
          if ( (rand () % 100) < PERCENT)
          {
            printf ("  %d - %d;\n", j, k + nodes); /* An Edge.  */
            myfile << ("  " + to_string(j) + " -> " + to_string(k+nodes) + ";\n");

            vec.push_back({j,k+nodes});
            tmp = "{" + to_string(j) + "," + to_string(k+nodes) + "},";
            res += tmp;
          }

      nodes += new_nodes; /* Accumulate into old node set.  */
    }
  }
  
  printf ("}\n");
  myfile << "}\n";
  myfile.close();
  res[res.size()-1] = ' ';
  res += "}";
  cout << res << "\n";

  system("dot -Tsvg graph.txt > out.svg");
  system("display out.svg &");

  return useDefault ? defaultEdges : vec;
}


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


// input: E edges
bool has_cycle(const vector<Edge> &edges) {
  // Implement your solution here to detect whether this undirected graph contains a cycle or not.

  printEdges(edges);

  //step1: create adjacency list "adj" & a set of remaining vertices to be discovered "remain_vertices"
  map<int,set<int>> adj; // this is safe if edges contains duplicate items!!!
  set<int> remain_vertices;
  for(auto const e : edges) // O(E) average
  {
    if (e.source == e.destination)
    {
      cout << "The vertex " << e.source << " causes cycle\n";
      return true;
    }
    adj[e.source].insert(e.destination);
    adj[e.destination].insert(e.source);
    remain_vertices.insert(e.source);
    remain_vertices.insert(e.destination);
  }
  printAdjs(adj);

  //step2: create a (queue) set of next vertices to be discovered "next_vertices"
  set<int> next_vertices;
  if (!adj.empty()) // O(1) average
  {
    next_vertices.insert(adj.cbegin()->first);
    remain_vertices.erase(adj.cbegin()->first);
  }

  //step3: while doing BFS, return true whenever we try to add a vertex into next_vertices while that vertex is already presented in next_vertices
  set<int> discovered_vertices; //debug
  printContainer(next_vertices, "next_vertices init"); // debug
  printContainer(remain_vertices, "remain_vertices init"); // debug
  while(true)
  {
    if (next_vertices.empty())
    {
      if (remain_vertices.empty()) // we've done traversing all vertices
      {
        return false;
      }
      else // when "next_vertices" is empty, take a vertex from "remain_vertices" for continuing BFS
      {
        printContainer(remain_vertices, "remain_vertices"); // debug
        next_vertices.insert(*(remain_vertices.cbegin()));
      }
    }
    auto current_vertices = *(next_vertices.cbegin());
    discovered_vertices.insert(current_vertices);
    printContainer(discovered_vertices, "discovered_vertices"); // debug
    next_vertices.erase(current_vertices);
    remain_vertices.erase(current_vertices);
    for (auto const e : adj[current_vertices]) // O(E)
    {
      // if e hasn't been discovered yet, try to add e to next_vertices;
      // but if e is already in "next_vertices", that means we have a cycle
      if ( (discovered_vertices.count(e) == 0) && ((next_vertices.insert(e)).second == false) )
      {
        cout << "The vertex " << e << " causes cycle\n";
        return true;
      }
    }
    printContainer(next_vertices, "next_vertices"); // debug
    printContainer(remain_vertices, "remain_vertices"); // debug
  }

  return false; // BFS is done and no cycle is found
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

  //const vector<Edge> edges_with_cycle    = { {0, 1}, {0, 2}, {0, 3}, {1, 4}, {1, 5}, {4, 8}, {4, 9}, {3, 6}, {3, 7}, {6, 10}, {6, 11}, {5, 9} };
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

  //const vector<Edge> edges_without_cycle = { {0, 1}, {0, 2}, {0, 3}, {1, 4}, {1, 5}, {4, 8}, {4, 9}, {3, 6}, {3, 7}, {6, 10}, {6, 11} };
  const vector<Edge> edges_without_cycle = createEdges(false);
//{ {0,1},{0,2},{1,3},{1,6},{2,4},{2,5} };
//{ {0,5},{0,7},{1,7},{2,6},{2,7} };
//{ {0,4},{4,7} };
//{ {7,11} };
//{        };
//{ {0,2},{1,2},{1,4},{1,5},{2,6} };
//{ {1,2},{1,4},{1,5},{0,8},{3,8},{4,8} };
//{ {0,4},{0,6},{1,6},{2,6},{3,6},{4,7},{2,8},{5,8} };
  //check_for_cycles(edges_with_cycle);
  check_for_cycles(edges_without_cycle);

  return 0;
}
