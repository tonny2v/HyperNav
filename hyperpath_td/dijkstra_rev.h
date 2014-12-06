#pragma once
#include <limits>
#include <string>
// http://www.cplusplus.com/reference/unordered_map/unordered_map/
//#include <unordered_map>
#include "algorithm.h"
#include "graph.hpp"
#include "radixheap.h"
#include <boost/python/numeric.hpp>
#include "drmhelper.hpp"
namespace bp = boost::python;
class Dijkstra_rev :
public Algorithm
{
private:
    
    Graph* g;
    
    float* u;
    
    int* pre_idx;
    
    bool* open;
    
    bool* close;
    
    Heap* heap;
    
public:
    
    // const & passing is more efficient than passing by value
    Dijkstra_rev(Graph* const _g);
    
    ~Dijkstra_rev();
    
    // to make it thread-safe, run shouldn't change anything of Graph (no write)
    void run(string _oid, const float* _weights);
    
    // void wrapper_run(string _oid, const bp::object &_weights);
    void wrapper_run(string _oid, const Drmhelper& helper);
    
    // get final node labels as node potentials
    const float* get_vlabels();
    
    vector<string> get_path(string _oid, string _did);
    
    bp::list wrapper_get_path(string _oid, string _did);
    
    string get_path_string(const vector<string> &_path, const string _delimeter);
    
};

