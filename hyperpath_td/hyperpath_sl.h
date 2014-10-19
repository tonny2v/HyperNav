//
//  hyperpath.h
//  MyGraph
//
//  Created by tonny.achilles on 5/26/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//

#ifndef __MyGraph__hyperpath_sl__
#define __MyGraph__hyperpath_sl__

#include <iostream>

#include <limits>
#include <string>
#include "algorithm.h"
#include "graph.hpp"
#include <unordered_map>
#include "fibheap.h"
using namespace std;
class Hyperpath_SL: public Algorithm
{
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
    
	Hyperpath_SL(Graph* const _g);
	
    
	~Hyperpath_SL();
	
    // to make it thread-safe, run shouldn't change anything of Graph (no write)
    void run(string _oid, string _did, float* _weights_min, float* _weights_max, const float* _h);
    
    vector<pair<string, float> > get_hyperpath();
    
    vector<string> get_path_rec(string _oid, string _did);

    string get_path_rec_vstring(const vector<string> &path, const string _delimiter);
    
    float get_path_weights_sum(const vector<string>& _path, float * _weights_min);
    
};


#endif /* defined(__MyGraph__hyperpath_sl__) */


