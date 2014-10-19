//
//  hyperpath.h
//  MyGraph
//
//  Created by tonny.achilles on 5/26/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//

#ifndef __MyGraph__hyperpath_TD__
#define __MyGraph__hyperpath_TD__

#include <iostream>
#include <limits>
#include <string>
#include "algorithm.h"
#include "graph.hpp"
#include <unordered_map>
#include "fibheap.h"
#include <boost/python.hpp>
#include "dijkstra.h"
#include "drmhelper.hpp"
using namespace std;
namespace bp = boost::python;

struct ResEdge
{
    string id;
    float p;
    string geojson;
    string con;
    string turn;
    int od_flg;
    float len;
    string fid;
    string tid;
    bool operator == (ResEdge const & another) const
    {
        return another.id == this->id;
    }
    
    bool operator != (ResEdge const & another) const
    {
        return another.id != this->id;
    }
};

class Hyperpath_TD: public Algorithm {
private:
    Graph *g;
    float *u_i; // node labels
    float* f_i; // weight sum
    float* p_i;
    
    float* u_a;
    float* p_a; // edge choice possiblities
    bool* open;
    bool* close;
//    vector<pair<string, float> > hyperpath;
    vector<ResEdge> hyperpath;
    Heap* heap;
    vector<string> path_rec;
    // get a random path recommendation
    
public:
    
    Hyperpath_TD(Graph* const _g);
    
    ~Hyperpath_TD();
    
    //	typedef unordered_map <string, unordered_map <int, float> > Profile; // link: time: speed
    // to make it thread-safe, run shouldn't change anything of Graph (no write)
    void run(const string &_oid, const string &_did, const float* h,
             int dep_time, const Drmhelper& helper);
    
    void wrapper_run(const string &_oid, const string &_did, Dijkstra &dij,
                     int dep_time, const Drmhelper &helper);
    
    // const after function means the function doesn't change the member variables
//    vector<pair<string, float> > get_hyperpath() const;
    vector<ResEdge> get_hyperpath() const;
    
    bp::list wrapper_get_path_rec(const string &_oid, const string &_did) const;
    
    vector<string> get_path_rec(const string &_oid, const string &_did) const;
    
    string wrapper_get_path_rec_vstring(const bp::list &_path, const string &_delimiter) const;
    
    bp::list wrapper_get_hyperpath();
    
    string get_path_rec_vstring(const vector<string> &path,
                                const string &_delimiter) const;
    
    float get_path_weights_sum(const vector<string> &_path,
                               float * _weights_min) const;
    
};

#endif /* defined(__MyGraph__hyperpath_TD__) */

