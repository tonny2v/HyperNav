
#include <boost/python.hpp>
#include <iostream>
//#include "graphbuilder.hpp" // problem: overlapped definition... don't know why
#include "hyperpath_td.h"
#include "hyperpath.h"
#include <pqxx/pqxx>
#include "drmhelper.hpp"
#include "PREDEFINE.h"
#include "dijkstra.h"
#include <gtest/gtest.h>

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

Drmhelper helper;
//
//TEST(NoExpress, SimpleRun) {
//
//    auto g = helper.make_graph("noexpress", 182778, 453430);
//    helper.open_hdf(HDF5, "speeds");
//    //533945-4380
//    float o_lon = 139.362125;
//    float o_lat = 35.7014583333333;
//
//    //533946-8893
//    float d_lon = 139.765475;
//    float d_lat = 35.6683583333333;
//
////    o_lon=139.62;o_lat=35.0;d_lon=139.68;d_lat=35.9;
//
//    Drmhelper::Coordinate o = {o_lon, o_lat};
//    Drmhelper::Coordinate d = {d_lon, d_lat};
//
//    cout << "query o id"<<endl;
//    string oid = helper.get_nearest_nodecode(o, "noexpress_nodes");
//    cout << "query d id"<<endl;
//    string did = helper.get_nearest_nodecode(d, "noexpress_nodes");
//
//    cout << "oid: "<< oid << endl;
//    cout << "did: "<< did << endl;
//
////        auto g = helper.make_subgraph(o, d, "basic_roadtype < 10", 5000);
////     int m = int(g->get_edge_number());
//    Hyperpath_TD alg(g.get());
//
//    // TODO: need to be revised since the node number is not pre-determined
//    cout << g->get_vertex_number()<<endl;
//    cout << g->get_edge_number()<<endl;
//
//
//    cout << "expected arrival time: "<< alg.wrapper_run(oid, did, 27000, helper, 0.0); // no hyperpath
//
//
//    int cnt = 0;
//    for (const auto &i : alg.get_hyperpath())
//    {
//        cnt ++;
//        cout << cnt<< ": " << i.id << "," << i.p << "," << i.od_flg << ","<< i.len << endl;
//    }
//    helper.close_hdf();
//};
//
//TEST(NoExpress, LengthCountLimits) {
//
//    auto g = helper.make_graph2("noexpress", 182778, 453430, 30, 10);
//    helper.open_hdf(HDF5, "speeds");
//    //533945-4380
//    float o_lon = 139.362125;
//    float o_lat = 35.7014583333333;
//
//    //533946-8893
//    float d_lon = 139.765475;
//    float d_lat = 35.6683583333333;
//
//    //    o_lon=139.62;o_lat=35.0;d_lon=139.68;d_lat=35.9;
//
//    Drmhelper::Coordinate o = {o_lon, o_lat};
//    Drmhelper::Coordinate d = {d_lon, d_lat};
//
//    cout << "query o id"<<endl;
//    string oid = helper.get_nearest_nodecode(o, "noexpress_nodes");
//    cout << "query d id"<<endl;
//    string did = helper.get_nearest_nodecode(d, "noexpress_nodes");
//
//    cout << "oid: "<< oid << endl;
//    cout << "did: "<< did << endl;
//
//    //        auto g = helper.make_subgraph(o, d, "basic_roadtype < 10", 5000);
//    //     int m = int(g->get_edge_number());
//    Hyperpath_TD alg(g.get());
//
//    // TODO: need to be revised since the node number is not pre-determined
//    cout << g->get_vertex_number()<<endl;
//    cout << g->get_edge_number()<<endl;
//
//
//    cout << "expected arrival time: "<< alg.wrapper_run(oid, did, 27000, helper, 0.0); // no hyperpath
//
//
//    int cnt = 0;
//    for (const auto &i : alg.get_hyperpath())
//    {
//        cnt ++;
//        cout << cnt<< ": " << i.id << "," << i.p << "," << i.od_flg << ","<< i.len << endl;
//    }
//    helper.close_hdf();
//};
//
TEST(NOEXPRESS, WRONGSP)
{
//    auto g = helper.make_graph2("noexpress", 182778, 453430, 30, 20);
    auto g = helper.make_graph2("noexpress", 182778, 453430);
    
    // add try clause here
    try {
        helper.open_hdf(HDF5, "speeds");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
   
//   http://localhost:5000/tdhp?o_lon=139.67782&o_lat=35.610134&d_lon=139.464216&d_lat=35.504513&level=1&t=0
    double o_lon=139.67782;
    double o_lat=35.610134;
    double d_lon=139.464216;
    double d_lat=35.504513;
    int dep_time = 0;
    float level = 1.0;
    
    Drmhelper::Coordinate o = {o_lon, o_lat};
    Drmhelper::Coordinate d = {d_lon, d_lat};
    
    cout << "query o id"<<endl;
    string oid = helper.get_nearest_nodecode(o, "noexpress_nodes");
    cout << "query d id"<<endl;
    string did = helper.get_nearest_nodecode(d, "noexpress_nodes");
    
    cout << "oid: "<< oid << endl;
    cout << "did: "<< did << endl;
    
    //        auto g = helper.make_subgraph(o, d, "basic_roadtype < 10", 5000);
    //     int m = int(g->get_edge_number());
    Hyperpath_TD alg(g.get());
    
    // TODO: need to be revised since the node number is not pre-determined
    cout << g->get_vertex_number()<<endl;
    cout << g->get_edge_number()<<endl;
    
    cout << "expected arrival time: "<< alg.run(oid, did, dep_time, helper, level) << endl; // no hyperpath
    
    int cnt = 0;
//    for (const auto &i : alg.get_hyperpath())
//    {
//        cnt++;
//        cout << cnt<< ": " << i.id << "," << i.p << "," << i.od_flg << ","<< endl;
//    }
    cout << "\"linkcode\" in (";
    for (const auto &i : alg.get_hyperpath())
    {
        cout <<'\''<<i.id<<'\''<<",";
    }
    cout << ")" << endl;
    
    helper.close_hdf();
}


//TEST (SquareRootTest, ZeroAndNegativeNos) {
//    ASSERT_EQ (0.0, 0.0);
//}
