//
//  dijkstra.cpp
//  MyGraph
//
//  Created by tonny.achilles on 5/25/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//

#include "dijkstra.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <Python.h>
#include <boost/python/numeric.hpp>
#include <boost/python.hpp>
#include <exception>
using namespace std;
namespace bp = boost::python;
Dijkstra::Dijkstra(Graph* const _g)
{
    g = _g;
    int n = int(g->get_vertex_number());
    u = new float[n];
    pre_idx = new int[n];
    open = new bool[n]; // vertices with T labels
    close = new bool[n]; //vertices with P labels
    HeapD<RadixHeap> heapD;
    heap = heapD.newInstance(n);
    
    for (int i=0;i<n;++i){
        u[i] = numeric_limits<float>::infinity();
        pre_idx[i] = -1;
        open[i] = false;
        close[i] = false;
    }
}

Dijkstra::~Dijkstra(){
    delete [] u;
    u = nullptr;
    delete [] pre_idx;
    pre_idx = nullptr;
    delete [] open;
    open = nullptr;
    delete [] close;
    close = nullptr;
    delete heap;
    heap = nullptr;
}

// u is link label, has to be initialized numeric_limits<float>::infinity()) before running,
// pre_idx stores the idx of shortest path tree for path trace back.
void Dijkstra::run(string _oid, const float* weights){
    auto o_idx = g->get_vidx(_oid);
    
    //initialization
    u[o_idx] = 0.0;
    heap->insert(o_idx, u[o_idx]);
    
    int vis_idx = 0;
    
    while (heap->nItems() > 0)
    {
        vis_idx = heap->deleteMin();
        auto vis = g->get_vertex(vis_idx);
        close[vis_idx] = true;
        open[vis_idx] = false;
        auto vis_out = vis->out_edges;
        for (const auto &e : vis_out)
        {
            auto v = e->to_vertex;
            float dist = 0.0;
            if (!close[v->idx])
            {
                dist = u[vis_idx] + weights[e->idx];
                
                if (dist < u[v->idx])
                {
                    u[v->idx] = dist;
                    if (open[v->idx])
                    {
                        heap->decreaseKey(v->idx, dist);
                    }
                    else
                    {
                        heap->insert(v->idx, dist);
                        open[v->idx] = true;
                    }
                    pre_idx[v->idx] = vis_idx;
                }
            }
        }
    }
}



// python wrapper
void Dijkstra::wrapper_run(string _oid, const Drmhelper& helper){
    
    //    int m = g->get_vertex_number();
    //    float* weights = new float[m];
    //    //
    //    for (int i = 0; i< m; ++i)
    //    {
    //        weights[i] = helper.get_length(g->get_edge(i)->id) / (helper.get_ffspeed(g->get_edge(i)->id)/ 3.6);
    //    }
    
    auto o_idx = g->get_vidx(_oid);
    
    //initialization
    u[o_idx] = 0.0;
    heap->insert(o_idx, u[o_idx]);
    
    int vis_idx = 0;
    
    while (heap->nItems() > 0)
    {
        vis_idx = heap->deleteMin();
        auto vis = g->get_vertex(vis_idx);
        close[vis_idx] = true;
        open[vis_idx] = false;
        auto vis_out = vis->out_edges;
        for (const auto &e : vis_out)
        {
            auto v = e->to_vertex;
            float dist = 0.0;
            if (!close[v->idx])
            {
                dist = u[vis_idx] + helper.get_length(e->id) / (helper.get_ffspeed(e->id)/ 3.6);
                
                if (dist < u[v->idx])
                {
                    u[v->idx] = dist;
                    if (open[v->idx])
                    {
                        heap->decreaseKey(v->idx, dist);
                    }
                    else
                    {
                        heap->insert(v->idx, dist);
                        open[v->idx] = true;
                    }
                    pre_idx[v->idx] = vis_idx;
                }
            }
        }
    }
    
}



// path trace back
// use try ... catch to call get_path
vector<string> Dijkstra::get_path(string _oid, string _did){
    vector<string> path;
    auto d_idx = g->get_vertex(_did)->idx;
    int idx = d_idx;
    do {
        path.push_back(g->get_vertex(idx)->id);
        idx = pre_idx[idx];
    } while(idx!=-1);
    
    reverse(path.begin(), path.end());
    if (path[0] != _oid)
    {
        throw "ERROR: "+ _did + " unaccessible from " + _oid;
    }
    return path;
}

bp::list Dijkstra::wrapper_get_path(string _oid, string _did) {
    bp::list path;
    auto d_idx = g->get_vertex(_did)->idx;
    int idx = d_idx;
    do {
        path.append(g->get_vertex(idx)->id);
        idx = pre_idx[idx];
    } while (idx != -1);
    path.reverse();
    if (path[0] != _oid) {
        const string s = "ERROR: " + _did + " unaccessible from " + _oid;
        PyErr_SetString(PyExc_Exception, s.c_str());
        
    }
    return path;
}

const float* Dijkstra::get_vlabels(){
    //    for(int i=0;i< 195669; i++) cout << u[i] << endl;
    return u;
    
}


string Dijkstra::get_path_string(const vector<string> &_path, const string _delimiter){
    stringstream ss("");
    for (auto it = _path.begin(); it!= _path.end(); ++it){
        if (it != _path.end() - 1) ss << *it << _delimiter;
        else ss<<*it;
    }
    return ss.str();
}



