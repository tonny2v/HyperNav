//
//  graphbuilder.h
//  MyGraph
//
//  Created by tonny.achilles on 5/28/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//

#ifndef MyGraph_graphbuilder_h
#define MyGraph_graphbuilder_h
#include <iostream>
#include "graph.hpp"
#include "csvhelper.hpp"
#include "PREDEFINE.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <unordered_map>
#include <pqxx/pqxx>
#include "drmhelper.hpp"

using namespace std;
using namespace boost;

const Graph get_papola_graph(float* const _weight_min, float* const _weight_max) {
	Graph g(3, 4);
	string vertices[3] = { "A", "B", "C" };
	for (auto v : vertices)
		g.add_vertex(v);
	g.add_edge("1-AC", "A", "C");
	g.add_edge("2-AB", "A", "B");
	g.add_edge("3-BC_up", "B", "C");
	g.add_edge("4-BC_down", "B", "C");
	return g;
}

const Graph get_test_graph(float* const _weights_min, float* const _weights_max) {
	Graph g(6, 11);
	string vertices[6] = { "A", "B", "C", "D", "E", "F" };

	for (auto v : vertices)
		g.add_vertex(v);

	g.add_edge("AB", g.get_vertex("A"), g.get_vertex("B"));
	auto AB = g.get_edge("AB")->idx;
	_weights_min[AB] = 2;
	_weights_max[AB] = 4;

	g.add_edge("AC", g.get_vertex("A"), g.get_vertex("C"));
	auto AC = g.get_edge("AC")->idx;
	_weights_min[AC] = 5;
	_weights_max[AC] = 6;

	g.add_edge("AD", g.get_vertex("A"), g.get_vertex("D"));
	auto AD = g.get_edge("AD")->idx;
	_weights_min[AD] = 1;
	_weights_max[AD] = 4.1;

	g.add_edge("BC", g.get_vertex("B"), g.get_vertex("C"));
	auto BC = g.get_edge("BC")->idx;
	_weights_min[BC] = 3;
	_weights_max[BC] = 5;

	g.add_edge("BD", g.get_vertex("B"), g.get_vertex("D"));
	auto BD = g.get_edge("BD")->idx;
	_weights_min[BD] = 2;
	_weights_max[BD] = 5;

	g.add_edge("CF", g.get_vertex("C"), g.get_vertex("F"));
	auto CF = g.get_edge("CF")->idx;
	_weights_min[CF] = 5;
	_weights_max[CF] = 6;

	g.add_edge("DB", g.get_vertex("D"), g.get_vertex("B"));
	auto DB = g.get_edge("DB")->idx;
	_weights_min[DB] = 2;
	_weights_max[DB] = 4;

	g.add_edge("DC", g.get_vertex("D"), g.get_vertex("C"));
	auto DC = g.get_edge("DC")->idx;
	_weights_min[DC] = 3;
	_weights_max[DC] = 5.2;

	g.add_edge("DE", g.get_vertex("D"), g.get_vertex("E"));
	auto DE = g.get_edge("DE")->idx;
	_weights_min[DE] = 1;
	_weights_max[DE] = 3.3;

	g.add_edge("EF", g.get_vertex("E"), g.get_vertex("F"));
	auto EF = g.get_edge("EF")->idx;
	_weights_min[EF] = 2;
	_weights_max[EF] = 5;

	g.add_edge("EC", g.get_vertex("E"), g.get_vertex("C"));
	auto EC = g.get_edge("EC")->idx;
	_weights_min[EC] = 1;
	_weights_max[EC] = 5;

	return g; // returns a copy of g (return an object)
}

// eid, fid, tid, direction, time, maxdelay (no column names)
const Graph get_bell_graph(float* const _weights_min, float* const _weights_max) {
	Graph g(64, 224);
	auto rows = load_csv(GRAPH_BELL_CSV_PATH, ',');
	int n = 64;
	for (int i = 0; i < n; ++i)
		g.add_vertex("v" + to_string(i + 1)); //id are "1", "2", ... according to the csv
	int e_cnt = 0;
	for (const auto &row : rows) {
		auto eid = "v" + row[1] + "->" + "v" + row[2];
		auto fvid = "v" + row[1];
		auto tvid = "v" + row[2];
		auto fv = g.get_vertex(fvid);
		auto tv = g.get_vertex(tvid);
		g.add_edge(eid, fv, tv);
//		auto direction = row[3]; // ignore
		auto time = stod(row[4]);
		auto maxdelay = stod(row[5]);
		_weights_min[e_cnt] = time;
		_weights_max[e_cnt] = time + maxdelay;
		e_cnt++;
	}
	return g;
}

const Graph get_belloneway_graph(float* const _weights_min, float* const _weights_max) {
	int n = 64;
	int m = 112;
	Graph g(n, m);
	auto rows = load_csv(GRAPH_BELL_ONEWAY_CSV_PATH, ',');
	for (int i = 0; i < n; ++i)
		g.add_vertex("v" + to_string(i + 1)); //id are "1", "2", ... according to the csv
	int e_cnt = 0;
	for (const auto &row : rows) {
		auto eid = "v" + row[1] + "->" + "v" + row[2];
		auto fvid = "v" + row[1];
		auto tvid = "v" + row[2];
		auto fv = g.get_vertex(fvid);
		auto tv = g.get_vertex(tvid);
		g.add_edge(eid, fv, tv);
		//		auto direction = row[3]; // ignore
		auto time = std::stod(row[4]);
		auto maxdelay = std::stod(row[5]);
		_weights_min[e_cnt] = time;
		_weights_max[e_cnt] = time + maxdelay;
		e_cnt++;
	}
	return g;
}

const Graph get_tokyo_graph(float* const _weights_min, float* const _weights_max) {
	int n = 38111;
	int m = 111438;
	Graph g(n, m);
	auto rows = load_csv(GRAPH_TOKYO_DRM_BIDIRECTIONAL_CSV_PATH, ',');

	for (int i = 0; i < n; ++i)
		g.add_vertex("v" + to_string(i + 1)); //id are "1", "2", ... according to the csv
	int e_cnt = 0;
	for (const auto &row : rows) {
		auto eid = "v" + row[1] + "->" + "v" + row[2];
		auto fvid = "v" + row[1];
		auto tvid = "v" + row[2];
		auto fv = g.get_vertex(fvid);
		auto tv = g.get_vertex(tvid);
		g.add_edge(eid, fv, tv);
		//		auto direction = row[3]; // ignore
		auto time = stod(row[4]);
		auto maxdelay = stod(row[5]);
		_weights_min[e_cnt] = time;
		_weights_max[e_cnt] = time + maxdelay;
		e_cnt++;
	}
	return g;

}

/* The following codes are all for matsim xml graph*/

const Graph get_tokyo_matsim_graph(float * const _weights_min,
		float* const _weights_max) {

	int n = 82540; // can be analyzed from xml input
	int m = 187746;

	Graph g(n, m);
	string network_path = "/Users/tonny/Downloads/Tokyo_network.xml";

	// ------------------------------------ anaylze network.xml ---------------------------------------
	cout << "read network.xml" << endl;
	boost::property_tree::ptree *pt_network = new property_tree::ptree();
	boost::property_tree::xml_parser::read_xml(network_path, *pt_network);
	auto nodes = pt_network->get_child("network.nodes");
	auto links = pt_network->get_child("network.links");
	map<pair<string, string>, float> nodes_to_dist;

	delete pt_network;
	// ==================   build network =====================
	cout << "start building network" << endl;
	//because link has attribute tag, so the links count also include one <xmlattr>, use wc -l to confirm

	for (const auto &node : nodes) {
		if (node.first == "node") {
			auto v_id = node.second.get<string>("<xmlattr>.id");
			g.add_vertex(v_id);
		}
	}

	int e_idx = 0;
	for (const auto &link : links) {
		if (link.first == "link") {
//			auto e_id = link.second.get<string>("<xmlattr>.id");
			auto fv_id = link.second.get<string>("<xmlattr>.from");
			auto tv_id = link.second.get<string>("<xmlattr>.to");
			auto e_id = fv_id + "->" + tv_id;
			auto length = link.second.get<float>("<xmlattr>.length");
			nodes_to_dist[make_pair(fv_id, tv_id)] = length;
			auto freespeed = link.second.get<float>("<xmlattr>.freespeed");
			auto time = length / freespeed;
			float maxdelay = 0.0; // time-dependent, later set from delay profile
			g.add_edge(e_id, g.get_vertex(fv_id), g.get_vertex(tv_id));
			_weights_min[e_idx] = time;
			_weights_max[e_idx] = time + maxdelay;
			e_idx++;
		}
	}
	return g;
}

const Graph get_tokyo_matsim_graph2(float* const _weights_min,
		float* const _weights_max, float* const _weights_dist) {
	int n = 82540; // can be analyzed from xml input
	int m = 187746;
	Graph g(n, m);
	auto rows = load_csv(GRAPH_TOKYO_MATSIM_CSV_PATH, ',');
	int e_cnt = 0;
	for (const auto &row : rows) {
		auto eid = row[0];
		auto fvid = row[1];
		auto tvid = row[2];
		g.add_vertex(fvid);
		g.add_vertex(tvid);
		auto fv = g.get_vertex(fvid);
		auto tv = g.get_vertex(tvid);
		g.add_edge(eid, fv, tv);
		auto length = stod(row[3]);
		auto freespeed = stod(row[4]);
		auto time = length / freespeed;
		auto maxdelay = 0.0;
		_weights_min[e_cnt] = time;
		_weights_max[e_cnt] = time + maxdelay;
		_weights_dist[e_cnt] = length;
		e_cnt++;
	}
	return g;
}

// this is the correct way to return pointers
// TODO: set the input parameter to OD CRS, directly generate the subgraph here.
const boost::shared_ptr<Graph> get_drm_graph(string links_view) {
    int n = 0;
    int m = 0;
	pqxx::result r;
			try {
				pqxx::connection c(CONNECTION);
				pqxx::work x(c);
                // link_view is not from user input, no risk of sql injection
                r = x.exec("SELECT count(*) as m FROM " + links_view + ";");
                //***********************************************************
                // auto calculate m and n may be slow when the link view is large
                m = r[0]["m"].as<int>();
                auto query = "SELECT count(*) as n FROM (SELECT meshcode || "
                "fnodecode FROM " + links_view + " UNION SELECT meshcode || "
                "tnodecode FROM " + links_view + ") AS t;";
                
                r = x.exec(query);
                n = r[0]["n"].as<int>();
                //***********************************************************
                
				// TODO: this can be changed to the links inside the rectangle only
				r = x.exec("SELECT linkcode, meshcode || '-' || fnodecode AS fnode, "
						"meshcode || '-' || tnodecode AS tnode "
						"FROM " + links_view + ";");
                
			} catch (std::exception &e) {
				cout << e.what() << endl;
			}
    // this also works but is a waste of memory
//    boost::shared_ptr<Graph> g (boost::make_shared<Graph>(DRM_NUM_NODES, DRM_NUM_LINKS));
    boost::shared_ptr<Graph> g (boost::make_shared<Graph>(n, m));
//	Graph g(DRM_NUM_NODES, DRM_NUM_LINKS);

	for (auto row : r)
	{
		g->add_edge(row["linkcode"].as<string>(), row["fnode"].as<string>(), row["tnode"].as<string>());
	}
	return g;
}

#endif
