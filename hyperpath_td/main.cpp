
#include <boost/python.hpp>
#include <iostream>
//#include "graphbuilder.hpp" // problem: overlapped definition... don't know why
#include "hyperpath_td.h"
#include "hyperpath.h"
#include <pqxx/pqxx>
#include "drmhelper.hpp"
#include "PREDEFINE.h"
#include "dijkstra.h"
using namespace std;
//
////____________________________________________________________________________//
//
//// you could easily implement test cases as a free functions
//// this test case will need to be explecetely registered in test tree
//
////void simple() {
////	// Initialize the Python runtime.
////	Py_Initialize();
////
////	PyRun_SimpleString("import numpy\n"
////			"from matplotlib import pylab\n"
////			"z1 = numpy.zeros((5,6), dtype=float)\n"
////			"z2 = numpy.zeros((4,3), dtype=float)\n"
////			"print z1\n"
////			"print z2\n"
////			"pylab.plot(z1)\n"
////			"pylab.show()\n");
////	Py_Finalize();
////}
////
////
////basic worked
//
//
//void basic() {
//    cout << "start" << endl;
//    string o = "533974-0070";
//    string d = "533974-0721";
//    string links_view = "links_533974";
//    auto g = get_drm_graph(links_view);
//    Hyperpath_TD alg(g.get());
//    
//    // TODO: need to be revised since the node number is not pre-determined
//    //	cout << g->get_vertex_number()<<endl;
//    //	cout << g->get_edge_number()<<endl;
//    
//    float* weights = new float[2651];
//    
//    float *h = new float[g->get_vertex_number()];
//    Tdhelper helper(HDF5, "speeds", links_view);
//    //	float h[DRM_NUM_NODES]{}; // over assigned
//    alg.run(o, d, h, 0, links_view, helper); // no hyperpath
//    
//    int cnt = 0;
//    for (auto i : alg.get_hyperpath())
//    {
//        cnt ++;
//        cout << cnt<< ": " << i.first << "," << i.second << endl;
//    }
//    delete [] h;
//    cout << "end" << endl;
//}

void basic2(){
    cout << "start" << endl;
    
//    Drmhelper::Coordinate o = {139.6067, 35.9907};
//
//    Drmhelper::Coordinate d = {139.5611, 35.9488};
//    Drmhelper::Coordinate d = {139.6062, 35.9484};
// NO:   Drmhelper::Coordinate d = {139.7354, 35.8030};
//    Drmhelper::Coordinate d = {139.6206, 35.9063};
    

//    Drmhelper::Coordinate o = {139.625, 35.9921}; //oid: 533974-0313
//    Drmhelper::Coordinate d = {139.6282, 35.9943}; //did: 533975-1449
    
    auto helper = Drmhelper(HDF5, "speeds");
//    string oid = helper.get_nearest_nodecode(o);
//    string did = helper.get_nearest_nodecode(d);
    //--------------OK, 38 links----------------
//    oid = "533974-0070";
//    did = "533974-0721";
    
    //--------------OK, 22 links------------------
//    oid = "POINT(139.625 35.9921166666667)"; //533974-0313
//    did = "POINT(139.6047875 35.9676916666667)"; //533974-0721
    
    //-------------OK, 344 links ---------------
//    string oid = "POINT(139.625 35.9921166666667)"; //533974-0313
//    string did = "POINT(139.7179 35.6774)"; //533945-0313
 
    //533945-4380
    float o_lon = 139.362125;
    float o_lat = 35.7014583333333;
    
    //533946-8893
    float d_lon = 139.765475;
    float d_lat = 35.6683583333333;
   
    o_lon=139.62;o_lat=35.0;d_lon=139.68;d_lat=35.9;
    
    Drmhelper::Coordinate o = {o_lon, o_lat};
    Drmhelper::Coordinate d = {d_lon, d_lat};
 
    cout << "query o id"<<endl;
    string oid = helper.get_nearest_nodecode(o, "noexpress_nodes");
    cout << "query d id"<<endl;
    string did = helper.get_nearest_nodecode(d, "noexpress_nodes");
    
    cout << "oid: "<< oid << endl;
    cout << "did: "<< did << endl;
    
//    auto g = helper.make_subgraph(o, d, "basic_roadtype < 10", 5000);
    auto g = helper.make_graph("noexpress", 190141, 453430);
    int m = int(g->get_edge_number());
    Hyperpath_TD alg(g.get());
    
    // TODO: need to be revised since the node number is not pre-determined
    cout << g->get_vertex_number()<<endl;
    cout << g->get_edge_number()<<endl;
    
    
    Dijkstra dij(g.get());
    alg.wrapper_run(oid, did, dij, 27000, helper); // no hyperpath
    
    
    int cnt = 0;
    for (const auto &i : alg.get_hyperpath())
    {
        cnt ++;
//        cout << cnt<< ": " << i.first << "," << i.second << endl;
        cout << cnt<< ": " << i.id << "," << i.p << "," << i.od_flg << ","<< i.len << endl;
    }
    cout << "end" << endl;
}

int main(){
    basic2();
    return 0;
}

////int main()
////{
////   	cout << "start" << endl;
////    float _weights_min[112]{};
////    float _weights_max[112]{};
////    Graph g = get_belloneway_graph(_weights_min, _weights_max);
////
////    // TODO: need to be revised since the node number is not pre-determined
////    float h[64]{};
////    cout << g.get_vertex_number()<<endl;
////    cout << g.get_edge_number()<<endl;
////    //	alg.run("533974-0302", "533974-0276", h, 0, link_view); // all link p = 1
////    Hyperpath alg(&g);
////
////    alg.run("v1", "v37", _weights_min, _weights_max, h);
////    for (auto i : alg.get_hyperpath())
////    {
////        cout << i.first << ":" << i.second << endl;
////    }
////    
////    cout << "end" << endl;
////}

//void easypath(string o, string d, int dep_time){
//    cout << "start" << endl;
//    //    string o = "533974-0070";
//    //    string d = "533974-0721";
//    //TODO: automatically calculate links view
//    string links_view = "links_533974";
//    auto g = get_drm_graph(links_view);
//    int m = int(g->get_edge_number());
//    //    int m = 2651;
//    cout << g->get_edge(1)->id << endl;
//    Hyperpath_TD alg(g.get());
//    
//    // TODO: need to be revised since the node number is not pre-determined
//    cout << g->get_vertex_number()<<endl;
//    cout << g->get_edge_number()<<endl;
//    //	float h[DRM_NUM_NODES]{}; // over assigned
//    
//    auto gr = g->make_reverse();
//    Tdhelper helper(HDF5, "speeds", links_view);
//    
//    float* weights = new float[m];
//    //
//    for (int i = 0; i< m; ++i)
//    {
//        weights[i] = helper.get_length(g->get_edge(i)->id) / (helper.get_ffspeed(g->get_edge(i)->id)/ 3.6);
//    }
//    
//    // use reverse graph to make sure node potentials are available
//    Dijkstra dij(gr.get());
//    dij.run(d, weights);
//    const float *h = dij.get_vlabels();
//    //    float *h = new float[g->get_vertex_number()];
//    alg.run(o, d, h, dep_time, links_view, helper); // no hyperpath
//    
//    
//    int cnt = 0;
//    for (auto i : alg.get_hyperpath())
//    {
//        cnt ++;
//        cout << cnt<< ": " << i.first << "," << i.second << endl;
//    }
//    delete [] weights;
//    cout << "end" << endl;
//    
//}

//string crs2id(float x, float y, string links_view){
//    pqxx::result r;
//    try {
//        pqxx::connection c(CONNECTION);
//        pqxx::work x(c);
//        string query = "";
//        r = x.exec(query);
//        
//    } catch (std::exception &e) {
//        cout << e.what() << endl;
//    }
//    return "";
//}


