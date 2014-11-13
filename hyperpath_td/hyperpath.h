//
//  hyperpath.h
//  MyGraph
//
//  Created by tonny.achilles on 5/26/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//

#ifndef __MyGraph__hyperpath__
#define __MyGraph__hyperpath__

#include <iostream>
#include <limits>
#include <string>
#include "algorithm.h"
#include "graph.hpp"
#include <unordered_map>
#include "fibheap.h"
#include <boost/python.hpp>
using namespace std;
namespace bp = boost::python;
class Hyperpath: public Algorithm {
private:
    Graph *g;
    float *u_i; // node labels
    float* f_i; // weight sum
    float* p_i;
    
    float* u_a;
    float* p_a; // edge choice possiblities
    bool* open;
    bool* close;
    vector<pair<string, float> > hyperpath;
    Heap* heap;
    vector<string> path_rec;
    // get a random path recommendation
    
public:
    
    Hyperpath(Graph* const _g);
    
    ~Hyperpath();
    
    // to make it thread-safe, run shouldn't change anything of Graph (no write)
    void run(string _oid, string _did, const float* _weights_min,
             const float* _weights_max, const float* _h);
    
    void wrapper_run(string _oid, string _did, const bp::object &_weights_min,
                     const bp::object &_weights_max, const bp::object &_h);
    
    vector<pair<string, float> > get_hyperpath();
    bp::list wrapper_get_hyperpath();
    
    vector<string> get_path_rec(string _oid, string _did);
    bp::list wrapper_get_path_rec(string _oid, string _did);
    
    string wrapper_get_path_rec_vstring(const bp::list &path,
                                        const string _delimiter);
    string get_path_rec_vstring(const vector<string> &path,
                                const string _delimiter);
    
    float get_path_weights_sum(const vector<string>& _path,
                               float * _weights_min);
    
};

#endif /* defined(__MyGraph__hyperpath__) */

