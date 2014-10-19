//
//  hyperpath.cpp
//  MyGraph
//
//  Created by tonny.achilles on 5/26/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "hyperpath_td.h"
#include "fibheap.h"
#include "dijkstra.h"
#include "heap.h"
#include "graph.hpp"
#include <algorithm>
#include <sstream>
#include <exception>
#include <hdf5.h>
#include <hdf5_hl.h>
#include "PREDEFINE.h"
#include "drmhelper.hpp"
#include <math.h>
#define LARGENUMBER 9999999999999



Hyperpath_TD::Hyperpath_TD(Graph* const _g) {
    g = _g;
    size_t n = g->get_vertex_number();
    size_t m = g->get_edge_number();
    u_i = new float[n];
    f_i = new float[n];
    p_i = new float[n];
    
    u_a = new float[m];
    p_a = new float[m];
    open = new bool[m];
    close = new bool[m];
    HeapD<FHeap> heapD;
    heap = heapD.newInstance(int(m));
    for (int i = 0; i < n; ++i) {
        u_i[i] = numeric_limits<float>::infinity();
        f_i[i] = 0.0;
        p_i[i] = 0.0;
    }
    
    for (int i = 0; i < m; ++i) {
        u_a[i] = numeric_limits<float>::infinity();
        p_a[i] = 0.0;
        open[i] = false;
        close[i] = false;
    }
    
}

Hyperpath_TD::~Hyperpath_TD() {
    delete[] u_i;
    delete[] f_i;
    delete[] p_i;
    
    delete[] u_a;
    delete[] p_a;
    delete[] open;
    delete[] close;
    delete heap;
}

//========================================================================================================
//* sf_di, link set overhead
// const float * denotes a constant pointer while float * const denotes the pointed content is constant
// since we may need to adjust weights_min and weights, the pointed content shouldn't be constant
// in time-dependent hyperpath algorithm, only weights_min and weigths_max are time-dependent
// to ensure FIFO property, weights_min and weights_max are correspondant to speeds_min and speeds_max *//
//========================================================================================================
// TODO: Profile speeds_min and speeds_max can be changed to numpy array
// but note that the link must be added according to the profile link sequence.
enum Speed_mode {MAX, MIN};

void Hyperpath_TD::run(const string &_oid, const string &_did, const float* h, int dep_time, const Drmhelper &helper) {
    
    //    Tdhelper helper(HDF5, "speeds", links_view);
    
    // TODO: define the lambda function according to Sung et. al. 2000
    
    // Non-passing travel time is necessary only for the cases when link length is longer than 15min's travel
    // especially the freeways.
    // speeds is m * 96 or less 1-D array for every 15 min case
    // if road length > 10 * 0.25 * 1000 = 2500 m, use the non-pass calculating function, else directly get the travel time
    // whether lambda function is processed as inline function and optimized is fully dependent on the compiler and compile optimization level
    // TODO: all passing by reference?
    const std::function<float(string, float, Speed_mode mode)> get_weights =
    // auto get_weights = /*better to use explicit declaration here*/
    // hdf_col: 0 is 5% speed, 1 is 95% speed
    [&](string _a_id, float _dep_time, int mode) -> float {
        float res_length = helper.get_length(_a_id);
        int interval = 900; // 15 min seconds
        int total = 86400;// day seconds
        int size = total / interval;
        vector<int> dep_times(size);
        for (int i = 0; i< size; ++i) {
            dep_times[i] = i * interval;
        }
        int overday = 0;
        // get time dependent speed
        while (dep_time >= total) { _dep_time -= total; overday++; }
        int pos = 0;
        if (_dep_time!= 0)
        {
            pos = int(std::lower_bound(dep_times.begin(),
                                       dep_times.end(),
                                       _dep_time) - dep_times.begin()) - 1 ;
        }
        // get h5key of _a_id
        hsize_t h5key = helper.get_h5key(_a_id);
        // set  buffer by reference
        float buffer[96][2];
        helper.fill_speeds(h5key, buffer);
        
        float ffspeed = helper.get_ffspeed(_a_id);
        
        // km/h
        float speed = buffer[pos][mode] == 0 ? ffspeed : buffer[pos][mode]; // mode 0 is max speed, mode 1 is min speed
        //        float nonfifo_time = res_length / (speed / 3.6);
        res_length -= speed / 3.6 * (900 * (pos + 1) - _dep_time);
        while (res_length > 0) {
            pos += 1;
            res_length -= speed / 3.6 * ( 900 * (pos + 1) - 900 * pos);
        }
        float arr_time = 900 *(pos+1) + res_length/ (speed / 3.6);
        float fifo_time = arr_time - (_dep_time - overday * total);
        return fifo_time;
    };
    
    // this code produces non-FIFO travel times, slightly faster, applies when most link lengths are not long (within an interval's drive)
    /* Non-FIFO (near FIFO) time_profile
     auto get_weights =
     //			auto get_weights = //better to use explicit declaration here
     // hdf_col: 0 is 5% speed, 1 is 95% speed
     [&](string _a_id, float _dep_time, Speed_mode mode) -> float {
     float res_length = helper.get_length(_a_id);
     int interval = 900; // 15 min seconds
     int total = 86400;// day seconds
     int size = total / interval;
     vector<int> dep_times(size);
     for (int i = 0; i< size; ++i) {
     dep_times[i] = i * interval;
     }
     
     // get time dependent speed
     while (_dep_time >= total) _dep_time -= total;
     int pos = 0;
     if (_dep_time!= 0)
     {
     pos = int(std::lower_bound(dep_times.begin(),
     dep_times.end(),
     _dep_time) - dep_times.begin()) - 1 ;
     }
     // get h5key of _a_id
     hsize_t h5key = helper.get_h5key(_a_id);
     // set  buffer by reference
     float buffer[96][2];
     helper.fill_speeds(h5key, buffer);
     
     float ffspeed = helper.get_ffspeed(_a_id);
     
     // km/h
     float speed = buffer[pos][mode] == 0 ? ffspeed : buffer[pos][mode]; // mode 0 is max speed, mode 1 is min speed
     
     return res_length/(speed / 3.6);
     };
     */
    
    auto o_idx = g->get_vidx(_oid);
    auto d_idx = g->get_vidx(_did);
    //initialization
    
    vector<Edge*> po_edges;
    
//    u_i[o_idx] = 0.0;
    u_i[o_idx] = dep_time;
    p_i[d_idx] = 1.0;
    
    int i_idx = o_idx;
    int j_idx = 0;
    int a_idx = 0;
    
    // forward pass
    while (true) {
        auto i = g->get_vertex(i_idx);
        for (auto edge : i->out_edges) {
            a_idx = edge->idx;
            i_idx = edge->from_vertex->idx;
            j_idx = edge->to_vertex->idx;
            
            float temp = u_i[i_idx] + get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MAX)
            + h[j_idx];
            if (u_a[a_idx] > temp) {
                u_a[a_idx] = temp;
                if (!close[a_idx]) {
                    if (!open[a_idx]) {
                        heap->insert(a_idx, u_a[a_idx]);
                        open[a_idx] = true;
                    } else {
                        heap->decreaseKey(a_idx, temp);
                    }
                }
            }
        }
        // one-way of search to bound case
        if (0 == heap->nItems()) {
            break;
        } else {
            a_idx = heap->deleteMin();
        }
        open[a_idx] = false;
        close[a_idx] = true;
        i_idx = g->get_edge(a_idx)->from_vertex->idx;
        j_idx = g->get_edge(a_idx)->to_vertex->idx;
        //updating
        float w_max = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MIN);
        float w_min = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MAX);
        
        if (u_i[j_idx] >= u_i[i_idx] + w_min) {
            //            cout << "max: "<<w_max << " min:"<<w_min<<endl;
            float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
            float P_a = f_a / (f_i[j_idx] + f_a);
            string a_id = g->get_edge(a_idx)->id;
            if (f_i[j_idx] == 0) {
                u_i[j_idx] = u_i[i_idx] + w_max;
            } else {
                float tmp = (1 - P_a) * u_i[j_idx] + P_a * (u_i[i_idx] + w_min);
                if (u_i[j_idx] > tmp)
                    u_i[j_idx] = tmp;
            }
            
            f_i[j_idx] += f_a;
            po_edges.push_back(g->get_edge(a_idx)); //hyperpath is saved by id index of links
            
        }
        //        cout << u_i[i_idx] << '\t'<< w_min << '\t' << u_i[d_idx] << endl;
        if (u_i[i_idx] + w_min + h[j_idx] > u_i[d_idx])
            break;
        i_idx = j_idx;
        
    }
    
    if (u_i[d_idx] == numeric_limits<float>::infinity())
    {
        throw "ERROR: destination not accessible";
    }
    
    // backward pass
    
    sort(po_edges.begin(), po_edges.end(), [&](Edge* a, Edge* b)->bool
         {
             //				float w_max = get_weights(a_idx, speeds_max, u_i[i_idx]);
             float w_min_a = get_weights(a->id, u_i[a->from_vertex->idx], Speed_mode::MAX);
             float w_min_b = get_weights(b->id, u_i[b->from_vertex->idx], Speed_mode::MAX);
             return u_i[a->from_vertex->idx] + w_min_a > u_i[b->from_vertex->idx] + w_min_b;
         });
    
    for (auto po_edge : po_edges) {
        auto a_idx = po_edge->idx;
        auto i_idx = po_edge->from_vertex->idx;
        auto j_idx = po_edge->to_vertex->idx;
        string a_id = g->get_edge(a_idx)->id;
        float w_max = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MIN);
        float w_min = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MAX);
        float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
        float P_a = f_a / f_i[j_idx];
        p_a[a_idx] = P_a * p_i[j_idx];
        p_i[i_idx] += p_a[a_idx];
    }
    
    for (auto po_edge : po_edges) {
        auto linkcode = po_edge->id;
        int odflag = 0;
        if(po_edge->from_vertex->id == _oid) odflag = 1;
        else if(po_edge->to_vertex->id == _did) odflag = 2;
        
        if (p_a[po_edge->idx] > 0.000001)
//            hyperpath.push_back(make_pair(po_edge->id, p_a[po_edge->idx]));
        {
            ResEdge e {linkcode, p_a[po_edge->idx], helper.get_geojson(linkcode), "", "", odflag, helper.get_length(linkcode), po_edge->from_vertex->id, po_edge->to_vertex->id};
            hyperpath.push_back(e);
        }
    }
    
    // the for_each lambda way
    
    //    	for_each(po_edges.begin(), po_edges.end(), [&](Edge* po_edge){
    //    		if (p_a[po_edge->idx] != 0)
    //    			hyperpath.push_back(make_pair(po_edge->id, p_a[po_edge->idx]));
    //    	});
    
}

void Hyperpath_TD::wrapper_run(const string &_oid, const string &_did, Dijkstra &dij,
                               int dep_time, const Drmhelper &helper)
{
    // nearest node may not in the node set...
    try {
        g->get_vertex(_oid);
        g->get_vertex(_did);
    }
    catch (std::exception &e)
    {
        cout << "Origin or destination ID error: " << e.what() << endl;
    }
    
    
    int m = int(g->get_edge_number());
    float* weights = new float[m];
    for (int i = 0; i< m; ++i)
    {
        auto length = helper.get_length(g->get_edge(i)->id);
        auto speed =  helper.get_ffspeed(g->get_edge(i)->id)/ 3.6;
        double weight = length / speed;
        weights[i] = weight;
    }
    dij.run(_did, weights);
    auto h = dij.get_vlabels();
    
    delete[] weights;
//    const std::function<float(string, float, Speed_mode mode)> get_weights =
    auto get_weights =
    [&](const string &_a_id, float _dep_time, int mode) -> float {
        float res_length = helper.get_length(_a_id);
        int interval = 900; // 15 min seconds
        int total = 86400;// day seconds
        int size = total / interval;
        vector<int> dep_times(size);
        for (int i = 0; i< size; ++i) {
            dep_times[i] = i * interval;
        }
        int overday = 0;
        while (dep_time >= total) { _dep_time -= total; overday++; }
        int pos = 0;
        if (_dep_time!= 0)
        {
            pos = int(std::lower_bound(dep_times.begin(),
                                       dep_times.end(),
                                       _dep_time) - dep_times.begin()) - 1 ;
        }
        hsize_t h5key = helper.get_h5key(_a_id);
        float buffer[96][2];
        helper.fill_speeds(h5key, buffer);
        float ffspeed = helper.get_ffspeed(_a_id);
        float speed = buffer[pos][mode] == 0 ? ffspeed : buffer[pos][mode]; // mode 0 is max speed, mode 1 is min speed
        res_length -= speed / 3.6 * (900 * (pos + 1) - _dep_time);
        while (res_length > 0) {
            pos += 1;
            res_length -= speed / 3.6 * ( 900 * (pos + 1) - 900 * pos);
        }
        float arr_time = 900 *(pos+1) + res_length/ (speed / 3.6);
        float fifo_time = arr_time - (_dep_time - overday * total);
        return fifo_time;
    };
    
    auto o_idx = g->get_vidx(_oid);
    auto d_idx = g->get_vidx(_did);
    //initialization
    
    vector<Edge*> po_edges;
    
    u_i[o_idx] = dep_time;
    p_i[d_idx] = 1.0;
    
    int i_idx = o_idx;
    int j_idx = 0;
    int a_idx = 0;
    
    // forward pass
    while (true) {
        auto i = g->get_vertex(i_idx);
        auto i_out = i->out_edges;
        
        for (auto edge : i_out) {
            a_idx = edge->idx;
            i_idx = edge->from_vertex->idx;
            j_idx = edge->to_vertex->idx;
            
            float temp = u_i[i_idx] + get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MAX)
            + h[j_idx];
//            cout << g->get_vertex(i_idx)->id <<" -> " << g->get_vertex(j_idx)->id <<": "<< h[j_idx] << endl;
            if (u_a[a_idx] > temp) {
                u_a[a_idx] = temp;
                if (!close[a_idx]) {
                    if (!open[a_idx]) {
                        heap->insert(a_idx, u_a[a_idx]);
                        open[a_idx] = true;
                    } else {
                        heap->decreaseKey(a_idx, temp);
                    }
                }
            }
        }
        // one-way of search to bound case
        if (0 == heap->nItems()) {
            break;
        } else {
            a_idx = heap->deleteMin();
        }
        open[a_idx] = false;
        close[a_idx] = true;
        i_idx = g->get_edge(a_idx)->from_vertex->idx;
        j_idx = g->get_edge(a_idx)->to_vertex->idx;
        //updating
        float w_max = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MIN);
        float w_min = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MAX);
        
        if (u_i[j_idx] >= u_i[i_idx] + w_min) {
            //            cout << "max: "<<w_max << " min:"<<w_min<<endl;
            float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
            float P_a = f_a / (f_i[j_idx] + f_a);
            string a_id = g->get_edge(a_idx)->id;
            if (f_i[j_idx] == 0) {
                u_i[j_idx] = u_i[i_idx] + w_max;
            } else {
                float tmp = (1 - P_a) * u_i[j_idx] + P_a * (u_i[i_idx] + w_min);
                if (u_i[j_idx] > tmp)
                    u_i[j_idx] = tmp;
            }
            
            f_i[j_idx] += f_a;
            po_edges.push_back(g->get_edge(a_idx)); //hyperpath is saved by id index of links
            
        }
        //        cout << u_i[i_idx] << '\t'<< w_min << '\t' << u_i[d_idx] << endl;
        if (u_i[i_idx] + w_min + h[j_idx] > u_i[d_idx])
//        if (u_i[i_idx] + w_min  > u_i[d_idx])
            break;
        i_idx = j_idx;
        
    }
    
    if (u_i[d_idx] == numeric_limits<float>::infinity())
    {
//        throw "ERROR: destination not accessible";
        throw myexception_notaccessible();
    }
    
    // backward pass
    
    sort(po_edges.begin(), po_edges.end(), [&](Edge* a, Edge* b)->bool
         {
             //				float w_max = get_weights(a_idx, speeds_max, u_i[i_idx]);
             float w_min_a = get_weights(a->id, u_i[a->from_vertex->idx], Speed_mode::MAX);
             float w_min_b = get_weights(b->id, u_i[b->from_vertex->idx], Speed_mode::MAX);
             return u_i[a->from_vertex->idx] + w_min_a > u_i[b->from_vertex->idx] + w_min_b;
         });
    
    for (auto po_edge : po_edges) {
        auto a_idx = po_edge->idx;
        auto i_idx = po_edge->from_vertex->idx;
        auto j_idx = po_edge->to_vertex->idx;
        string a_id = g->get_edge(a_idx)->id;
        float w_max = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MIN);
        float w_min = get_weights(g->get_edge(a_idx)->id, u_i[i_idx], Speed_mode::MAX);
        float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
        float P_a = f_a / f_i[j_idx];
        p_a[a_idx] = P_a * p_i[j_idx];
        p_i[i_idx] += p_a[a_idx];
    }
    
//    auto analyze_con = []()->string{return "";};
//    auto analyze_turn = []()->string{return "";};
    
    for (auto po_edge : po_edges) {
        if (p_a[po_edge->idx] > 0.000001)
        {
            int odflag = 0;
            if(po_edge->from_vertex->id == _oid) odflag = 1;
            else if(po_edge->to_vertex->id == _did) odflag = 2;
            auto linkcode = po_edge->id;
        // 4 digits accuracy
        // hyperpath.push_back(make_pair(po_edge->id, (round(p_a[po_edge->idx] *10000)/10000)));
            ResEdge e {linkcode, p_a[po_edge->idx], helper.get_geojson(linkcode), "", "", odflag, helper.get_length(linkcode), po_edge->from_vertex->id, po_edge->to_vertex->id};
            hyperpath.push_back(e);
        }
    }
}


//vector<pair<string, float> > Hyperpath_TD::get_hyperpath() const {
vector<ResEdge> Hyperpath_TD::get_hyperpath() const {
    return hyperpath;
}

vector<string> Hyperpath_TD::get_path_rec(const string &_oid, const string &_did) const {
    vector<string> path_rec;
    
    auto vis = g->get_vertex(_oid);
    auto d = g->get_vertex(_did);
    //initialize random seed
    srand(time(NULL));
    while (vis->idx != d->idx) {
        auto out_edges = vis->out_edges;
        
        float p_sum = 0;
        for (auto it = out_edges.begin(); it != out_edges.end(); it++) {
            int idx = (*it)->idx;
            //if current link is a hyperpth link
            if (0 != p_a[idx]) {
                float x = p_a[idx];
                p_sum += x;
            }
        }
        //		if (p_sum != p_i[vis->idx]) cout << "NOT Equal" << endl;
        for (auto it = out_edges.begin(); it != out_edges.end(); it++) {
            //if current link is a hyperpth link
            if (0 != p_a[(*it)->idx]) {
                float r = rand() % std::numeric_limits<int32_t>::max()
                / ((float) std::numeric_limits<int32_t>::max());
                if (r < p_a[(*it)->idx] / p_sum)
                    //				cout <<"r:" <<r <<endl;
                    //				if (r< 0.5)
                {
                    string id = (*it)->id;
                    path_rec.push_back(id);
                    vis = (*it)->to_vertex;
                    break; // one link being recommended, skip the others
                }
            }
        }
        if (p_i[vis->idx] == 0) {
            throw "ERROR: hyperpath not set,please run "
            "Hyperpath::run(string _oid, string _did, float* weights_min, float* weights_max, const float* h) first";
        }
    }
    return path_rec;
}


//bp::list Hyperpath_TD::wrapper_get_hyperpath() {
//    bp::list l;
//    for (auto it = hyperpath.begin(); it != hyperpath.end(); it++) {
//        bp::tuple e = bp::make_tuple((*it).first, (*it).second);
//        l.append(e);
//    }
//    return l;
//}

bp::list Hyperpath_TD::wrapper_get_path_rec(const string &_oid, const string &_did) const{
    bp::list path_rec;
    
    auto vis = g->get_vertex(_oid);
    auto d = g->get_vertex(_did);
    //initialize random seed
    srand(time(NULL));
    while (vis->idx != d->idx) {
        auto out_edges = vis->out_edges;
        
        float p_sum = 0;
        for (auto it = out_edges.begin(); it != out_edges.end(); it++) {
            int idx = (*it)->idx;
            //if current link is a hyperpth link
            if (0 != p_a[idx]) {
                float x = p_a[idx];
                p_sum += x;
            }
        }
        for (auto it = out_edges.begin(); it != out_edges.end(); it++) {
            //if current link is a hyperpth link
            if (0 != p_a[(*it)->idx]) {
                float r = rand() % std::numeric_limits<int32_t>::max()
                / ((float) std::numeric_limits<int32_t>::max());
                if (r < p_a[(*it)->idx] / p_sum) {
                    string id = (*it)->id;
                    path_rec.append(id);
                    vis = (*it)->to_vertex;
                    break; // one link being recommended, skip the others
                }
            }
        }
        if (p_i[vis->idx] == 0) {
            throw "ERROR: hyperpath not set, please conduct Hyperpath::run(string _oid, string _did, float* weights_min, float* weights_max, const float* h) first";
        }
    }
    return path_rec;
}

// meet with MATSim convention, start from first startpoint and end with last endpoint
string Hyperpath_TD::get_path_rec_vstring(const vector<string> &_path,
                                          const string &_delimiter) const {
    stringstream ss("");
    for (auto it = _path.begin(); it != _path.end(); ++it) {
        if (it != _path.end() - 1)
            ss << g->get_edge(*it)->from_vertex->id << _delimiter;
        else {
            ss << g->get_edge(*it)->from_vertex->id << _delimiter;
            ss << g->get_edge(*it)->to_vertex->id;
        }
    }
    return ss.str();
}

string Hyperpath_TD::wrapper_get_path_rec_vstring(const bp::list &_path,
                                                  const string &_delimiter) const {
    stringstream ss("");
    auto len = bp::len(_path);
    for (int i = 0; i < len; ++i) {
        if (i != len - 1)
            ss << g->get_edge(bp::extract<string>(_path[i]))->from_vertex->id
            << _delimiter;
        else {
            ss << g->get_edge(bp::extract<string>(_path[i]))->from_vertex->id
            << _delimiter;
            ss << g->get_edge(bp::extract<string>(_path[i]))->to_vertex->id;
        }
        
    }
    return ss.str();
}

float Hyperpath_TD::get_path_weights_sum(const vector<string>& _path,
                                         float * _weights_min) const {
    float sum = 0.0;
    for (auto e_id : _path) {
        auto e_idx = g->get_edge(e_id)->idx;
        sum += _weights_min[e_idx];
    }
    return sum;
}
