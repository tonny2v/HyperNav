//
//  dijkstra.cpp
//  MyGraph
//
//  Created by tonny.achilles on 5/25/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//

#include "dijkstra_rev.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <Python.h>
#include <boost/python/numeric.hpp>
#include <boost/python.hpp>
#include <exception>
using namespace std;
namespace bp = boost::python;
Dijkstra_rev::Dijkstra_rev(Graph* const _g)
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

Dijkstra_rev::~Dijkstra_rev(){
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
void Dijkstra_rev::run(string _oid, const float* weights,  const set<string>& turn_restrictions){
    auto o_idx = g->get_vidx(_oid);
    
    //initialization
    u[o_idx] = 0.0;
    heap->insert(o_idx, u[o_idx]);
    
    int vis_idx = 0;
    
    // previous visited node
    Vertex* vis_pre = nullptr;
    
    while (heap->nItems() > 0)
    {
        vis_idx = heap->deleteMin();
        auto vis = g->get_vertex(vis_idx);
        
        Edge* e_pre = nullptr;
        
        //previous edge, note that it's reverse search here, so the first parameter is actually to node
        if (vis_pre != nullptr){
            e_pre = g->get_edge(vis, vis_pre);
        }
        close[vis_idx] = true;
        open[vis_idx] = false;
        auto vis_in = vis->in_edges;
        for (const auto &e : vis_in)
        {
//            string turn = "";
//            if (vis_pre != nullptr){
//                turn = e->id + "," + e_pre->id;
//            }
//            if (turn_restrictions.size()!=0 && turn != ""){
//            // if a turn is found in restriction table, skip updating the link
//            if (turn_restrictions.find(turn) != turn_restrictions.end()) continue;
//            }
            auto v = e->from_vertex;
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
        vis_pre = vis;
    }
}



// python wrapper
void Dijkstra_rev::wrapper_run(string _oid, const Drmhelper& helper){
    
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
        auto vis_in = vis->in_edges;
        for (const auto &e : vis_in)
        {
            auto v = e->from_vertex;
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

const float* Dijkstra_rev::get_vlabels(){
    //    for(int i=0;i< 195669; i++) cout << u[i] << endl;
    return u;
    
}


