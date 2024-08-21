//
//  main.cpp
//  CppGraphTest
//
//  Created by Dave Duprey on 25/11/2019.
//  Copyright © 2019 Dave Duprey. All rights reserved.
//

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>

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


/**
*  @brief  Given an undirected graph, determine whether or not it contains a cycle.
*  @param  edges  A collection of edges of undirected graph.
*  
*  Idea: using Breadth-First Search (BFS) to check for cycle.
*  Time complexity: O(E) average, O(E^2) worst-case (for an input of E edges). Note: can use std::set instead of std::unordered_set for O(ElogE) worst-case.
*/
bool has_cycle(const vector<Edge> &edges) {
  // Implement your solution here to detect whether this undirected graph contains a cycle or not.
  printEdges(edges); //debug

  //step1: create adjacency list "adj" & a collection of remaining vertices to be discovered "remain_vertices"
  unordered_map<int,unordered_set<int>> adj; // this is safe if edges contain duplicate items!!!
  unordered_set<int> remain_vertices;
  for(auto const e : edges) // O(E) average
  {
    if (e.source == e.destination)
    {
      cout << "The vertex " << e.source << " causes cycle\n"; //debug
      return true;
    }
    adj[e.source].insert(e.destination);
    adj[e.destination].insert(e.source);
    remain_vertices.insert(e.source);
    remain_vertices.insert(e.destination);
  }
  printAdjs(adj); //debug

  //step2: create a collection of next vertices to be discovered "next_vertices"
  unordered_set<int> next_vertices;
  if (!adj.empty()) // O(1) average
  {
    next_vertices.insert(adj.cbegin()->first);
    remain_vertices.erase(adj.cbegin()->first);
  }

  //step3: while doing BFS, return true whenever we try to add a vertex into next_vertices while that vertex is already presented in next_vertices
  unordered_set<int> discovered_vertices; //debug
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
    for (auto const e : adj[current_vertices]) // O(E) average
    {
      // if e hasn't been discovered yet, try to add e to next_vertices;
      // but if e is already in "next_vertices", that means we have a cycle
      if ( (discovered_vertices.count(e) == 0) && ((next_vertices.insert(e)).second == false) )
      {
        cout << "The vertex " << e << " causes cycle\n"; //debug
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

  const vector<Edge> edges_with_cycle    = { {0, 1}, {0, 2}, {0, 3}, {1, 4}, {1, 5}, {4, 8}, {4, 9}, {3, 6}, {3, 7}, {6, 10}, {6, 11}, {5, 9} };
  const vector<Edge> edges_without_cycle = { {0, 1}, {0, 2}, {0, 3}, {1, 4}, {1, 5}, {4, 8}, {4, 9}, {3, 6}, {3, 7}, {6, 10}, {6, 11} };

  check_for_cycles(edges_with_cycle);
  check_for_cycles(edges_without_cycle);

  return 0;
}
