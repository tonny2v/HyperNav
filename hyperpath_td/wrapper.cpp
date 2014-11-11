#include <boost/python.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <boost/python/numeric.hpp>
#include <boost/make_shared.hpp>
#include <boost/python/tuple.hpp>
#include "graph.hpp"
#include "hyperpath.h"
#include "dijkstra.h"
#include "stdio.h"
#include "drmhelper.hpp"
#include "hyperpath_td.h"
#include <set>
#include <boost/python/exception_translator.hpp>
using namespace boost::python;
namespace bp = boost::python;
using namespace std;

struct graph_pickle_suite : boost::python::pickle_suite
{
    static boost::python::tuple
    getinitargs(Graph const& g) { return boost::python::make_tuple(g.get_vertex_number(), g.get_edge_number()); }
    
    static boost::python::tuple
    getstate(boost::python::object g_obj){
        Graph const &g = boost::python::extract<Graph const&>(g_obj);
        // serialize as vertices, saves space
/*        boost::python::tuple vertices;
        for (int i = 0; i < g.get_vertex_number(); ++i) {
            auto v_i = g.get_vertex(i);
            boost::python::tuple in_edges;
            boost::python::tuple out_edges;
            for (int in = 0; in < v_i->in_cnt ; ++in) {
                in_edges[in] = v_i->in_edges[in]->id;
            }
            
            for (int in = 0; in < v_i->out_cnt ; ++in) {
                out_edges[in] = v_i->out_edges[in]->id;
            }
            // NOTE: the storage idx may different after de-pickled (actually the same?)
            vertices[i] = boost::python::make_tuple(v_i->id, in_edges, out_edges);
        }
        return vertices;
 */
        boost::python::tuple edges;
        // serialize as edges
        boost::python::list l;
        for (int i = 0; i < g.get_edge_number(); ++i) {
            l.append(g.get_edge(i)->id);
            l.append(g.get_edge(i)->from_vertex->id);
            l.append(g.get_edge(i)->to_vertex->id);
            edges[i] = l;
        }
        return edges;
    }
    static void
    setstate(Graph &g, boost::python::tuple state) {
        for(int i = 0; i< g.get_edge_number(); ++i)
        {
            string eid = boost::python::extract<string>(state[i][0]);
            string fid = boost::python::extract<string>(state[i][1]);
            string tid = boost::python::extract<string>(state[i][2]);
            g.add_edge(eid, fid, tid);
        }
    }
 

};



void translate(const myexception_notaccessible & e)
{
    // Use the Python 'C' API to set up an exception object
    PyErr_SetString(PyExc_RuntimeError, e.what());
}


const bp::list summarize(const bp::object &array) {
	size_t m = bp::len(array);
	std::set<string> vertices_set;
	for (int i = 0; i < m; ++i) {
		string fid = extract<string>(array[i][1]);
		string tid = extract<string>(array[i][2]);
		vertices_set.insert(fid);
		vertices_set.insert(tid);
	}
	size_t n = vertices_set.size();
	bp::list l;
	l.append(n);
	l.append(m);
	return l;
}

// the input should by m * 3 array-like
const boost::shared_ptr<Graph> make_graph(const bp::object& array, int n, int m) {
    boost::shared_ptr<Graph> g (boost::make_shared<Graph>(n, m));
	for (int i = 0; i < bp::len(array); ++i) {
		string eid = extract<string>(array[i][0]);
		string fid = extract<string>(array[i][1]);
		string tid = extract<string>(array[i][2]);
		g->add_edge(eid, fid, tid);
	}
	return g;
}

BOOST_PYTHON_MODULE(mygraph)
{
    // Register exceptions
    register_exception_translator<myexception_notaccessible>(&translate);
    
	// Edge vector
	class_<vector<Edge*> > Edges_vec("Edges_vec");
	Edges_vec.def(vector_indexing_suite<vector<Edge*> >());

    class_<pair<string, float> > Pair("Pair");
    Pair.def_readonly("first", &pair<string, float>::first);
    Pair.def_readonly("second", &pair <string, float>::second);
    class_<vector<pair<string, float> > > Hyperpath_vec("Hyperpath_vec");
	Hyperpath_vec.def(vector_indexing_suite<vector<pair<string, float> > >());
    
    class_<vector<ResEdge> > ResEdge_vec("ResEdge_vec");
	ResEdge_vec.def(vector_indexing_suite<vector<ResEdge > >());
    
    /// ************************************************************************
	///                                 Vertex
	/// ************************************************************************
	class_<Vertex> pyVertex("Vertex", init<string>());
	pyVertex.def_readwrite("id", &Vertex::id);
	pyVertex.def_readonly("idx", &Vertex::idx);
	pyVertex.def_readonly("in_cnt", &Vertex::in_cnt);
	pyVertex.def_readonly("out_cnt", &Vertex::out_cnt);
	pyVertex.def_readwrite("in_edges", &Vertex::in_edges);
	pyVertex.def_readwrite("out_edges", &Vertex::out_edges);

	/// ************************************************************************
	///                                 Edge
	/// ************************************************************************
	class_<Edge> pyEdge("Edge", init<string, Vertex*, Vertex*>());
	pyEdge.def_readwrite("id", &Edge::id);
	pyEdge.def_readonly("idx", &Edge::idx);

	// python won't delete the pointer if using reference_existing_object policy
	// on the contrary, if using manage_new_object, the pointer deletion will be python's duty.
	pyEdge.def("get_fv", &Edge::get_fv,
			return_value_policy<reference_existing_object>());
	pyEdge.def("get_tv", &Edge::get_tv,
			return_value_policy<reference_existing_object>());

	/// ************************************************************************
    ///                                 Graph
    /// ************************************************************************
    // shared_ptr should be added to the class declaration
    class_<Graph, boost::shared_ptr<Graph> > pyGraph("Graph", init<int, int>());
	pyGraph.def("add_vertex", &Graph::add_vertex);
    pyGraph.def_pickle(graph_pickle_suite());

	// manually create two function pointers to enable function overload.
	// autooverloading only appies to void functions
	void (Graph::*add_edge_v)(const string &_id, Vertex* _fv,
			Vertex* _tv) = &Graph::add_edge;
	void (Graph::*add_edge_s)(const string &_id, const string &_fv_id,
			const string &_tv_id) = &Graph::add_edge;
	pyGraph.def("add_edge", add_edge_v);
	pyGraph.def("add_edge", add_edge_s);
	pyGraph.def_readonly("edge_num", &Graph::get_edge_number);
	pyGraph.def_readonly("vertex_num", &Graph::get_vertex_number);

	// get vertex
	Vertex* (Graph::*get_vertex_byid)(const string &id) const = &Graph::get_vertex;
	Vertex* (Graph::*get_vertex_byidx)(int idx) const = &Graph::get_vertex;
    pyGraph.def("get_vertex", get_vertex_byid,
                return_value_policy<reference_existing_object>());
    pyGraph.def("get_vertex", get_vertex_byidx,
                return_value_policy<reference_existing_object>());
    
    pyGraph.def("reverse", &Graph::make_reverse);
    // get edge
    Edge* (Graph::*get_edge_byid)(string id) const = &Graph::get_edge;
    Edge* (Graph::*get_edge_byidx)(int idx) const= &Graph::get_edge;
    pyGraph.def("get_edge", get_edge_byid,
                return_value_policy<reference_existing_object>());
    pyGraph.def("get_edge", get_edge_byidx,
                return_value_policy<reference_existing_object>());
    
	// Graph from array
	boost::python::numeric::array::set_module_and_type("numpy", "ndarray");
//	def("make_graph", make_graph, return_value_policy<manage_new_object>());
    //no need to use manage_new_object since shared_ptr is used
	def("make_graph", make_graph); 
	def("summarize", summarize);
    
	/// ************************************************************************
	///                                 Dijkstra
	/// ************************************************************************
	class_<Dijkstra> pyDijkstra("Dijkstra", init<Graph*>());
	pyDijkstra.def("run", &Dijkstra::wrapper_run);
	pyDijkstra.def("get_path", &Dijkstra::wrapper_get_path);
    
	/// ************************************************************************
	///                                 Hyperpath
	/// ************************************************************************
	class_<Hyperpath> pyHyperpath("Hyperpath", init<Graph*>());
	pyHyperpath.def("run", &Hyperpath::wrapper_run);
	pyHyperpath.def("get_hyperpath", &Hyperpath::wrapper_get_hyperpath);
	pyHyperpath.def("get_path_rec", &Hyperpath::wrapper_get_path_rec);
	pyHyperpath.def("get_path_rec_vstring",
			&Hyperpath::wrapper_get_path_rec_vstring);
    
	/// ************************************************************************
    ///                                 Drmhelper
	/// ************************************************************************
    class_<Drmhelper> pyDrmhelper("Drmhelper", init<>());
//    pyDrmhelper.def("get_drm_graph", &Drmhelper::get_drm_graph);
    pyDrmhelper.def("get_nearest_node", &Drmhelper::wrapper_get_nearest_nodecode);
    pyDrmhelper.def("make_subgraph", &Drmhelper::wrapper_make_subgraph);
    pyDrmhelper.def("make_graph", &Drmhelper::make_graph2);
    pyDrmhelper.def("make_graph", &Drmhelper::make_graph);
    pyDrmhelper.def("open_hdf", &Drmhelper::open_hdf);
    pyDrmhelper.def("close_hdf", &Drmhelper::close_hdf);
    
    /// ************************************************************************
    ///                                 Hyperpath_TD
    /// ************************************************************************
    class_<ResEdge> pyResEdge("ResEdge");
    pyResEdge.add_property("id", &ResEdge::id);
    pyResEdge.add_property("p", &ResEdge::p);
    pyResEdge.add_property("geojson", &ResEdge::geojson);
    pyResEdge.add_property("od_flg", &ResEdge::od_flg);
    pyResEdge.add_property("len", &ResEdge::len);
    pyResEdge.add_property("fid", &ResEdge::fid);
    pyResEdge.add_property("tid", &ResEdge::tid);
    pyResEdge.def_readwrite("con", &ResEdge::con);
    pyResEdge.def_readwrite("turn", &ResEdge::turn);
    
    class_<Hyperpath_TD> pyHyperpath_TD("Hyperpath_TD", init<Graph*>());
    pyHyperpath_TD.def("run", &Hyperpath_TD::wrapper_run);
    pyHyperpath_TD.def("get_path_rec", &Hyperpath_TD::wrapper_get_path_rec);
    pyHyperpath_TD.def("get_path_rec_vstring",
                    &Hyperpath_TD::wrapper_get_path_rec_vstring);
	pyHyperpath_TD.def("get_hyperpath", &Hyperpath_TD::get_hyperpath);
    
    /// ************************************************************************
    ///                                 GraphBuilder
    /// ************************************************************************
//    def("get_drm_graph", get_drm_graph);
}