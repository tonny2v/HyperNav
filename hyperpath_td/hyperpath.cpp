//
//  hyperpath.cpp
//  MyGraph
//
//  Created by tonny.achilles on 5/26/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include "hyperpath.h"
#include "fibheap.h"
#include "dijkstra.h"
#include "heap.h"
#include <algorithm>
#include <sstream>

#define LARGENUMBER 9999999999

Hyperpath::Hyperpath(Graph * const _g) {
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
	heap = heapD.newInstance(m);
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

Hyperpath::~Hyperpath() {
	delete[] u_i;
    u_i = nullptr;
	delete[] f_i;
    f_i = nullptr;
	delete[] p_i;
    p_i = nullptr;
    
	delete[] u_a;
    u_a = nullptr;
	delete[] p_a;
    p_a = nullptr;
	delete[] open;
    open = nullptr;
	delete[] close;
    close = nullptr;
	delete heap;
    heap = nullptr;
}

// sf_di, link set overhead
// const float * denotes a constant pointer while float * const denotes the pointed content is constant
// since we may need to adjust weights_min and weights, the pointed content shouldn't be constant
void Hyperpath::run(string _oid, string _did, const float* weights_min,
		const float* weights_max, const float* h) {

	auto o_idx = g->get_vidx(_oid);
	auto d_idx = g->get_vidx(_did);
//    auto d = g->get_vertex(_did); // destination vertex
//    int n = g->get_vertex_number();
//    int m = g->get_edge_number();
	//initialization

	vector<Edge*> po_edges;

	u_i[d_idx] = 0.0;
	p_i[o_idx] = 1.0;

	int j_idx = d_idx;
	int i_idx = 0;
	int a_idx = 0;

	// backward pass
	while (true) {
		auto j = g->get_vertex(j_idx);
		for (auto edge : j->in_edges) {
			a_idx = edge->idx;
            cout << "id" << edge->to_vertex->id << endl;
			i_idx = edge->from_vertex->idx;
			j_idx = edge->to_vertex->idx;

			float temp = u_i[j_idx] + weights_min[a_idx] + h[i_idx];
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
		float w_max = weights_max[a_idx];
		float w_min = weights_min[a_idx];

		if (u_i[i_idx] >= u_i[j_idx] + w_min) {
//            cout << "max: "<<w_max << " min:"<<w_min<<endl;
			float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
			float P_a = f_a / (f_i[i_idx] + f_a);

			if (f_i[i_idx] == 0) {
				u_i[i_idx] = u_i[j_idx] + w_max;
			} else {
				if (u_i[i_idx]
						> (1 - P_a) * u_i[i_idx] + P_a * (u_i[j_idx] + w_min))
					u_i[i_idx] = (1 - P_a) * u_i[i_idx]
							+ P_a * (u_i[j_idx] + w_min);
			}

			f_i[i_idx] += f_a;
			po_edges.push_back(g->get_edge(a_idx)); //hyperpath is saved by id index of links

		}

		if (u_i[j_idx] + w_min + h[i_idx] > u_i[o_idx])
			break;
		j_idx = i_idx;

	}

	// forward pass

	sort(po_edges.begin(), po_edges.end(),
			[&](Edge* a, Edge* b)->bool
			{
				return u_i[a->to_vertex->idx] + weights_min[a->idx] > u_i[b->to_vertex->idx] + weights_min[b->idx];
			});

	for (auto po_edge : po_edges) {
		auto a_idx = po_edge->idx;
		auto i_idx = po_edge->from_vertex->idx;
		auto j_idx = po_edge->to_vertex->idx;
		float w_max = weights_max[a_idx];
		float w_min = weights_min[a_idx];
		float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
		float P_a = f_a / f_i[i_idx];
		p_a[a_idx] = P_a * p_i[i_idx];
		p_i[j_idx] += p_a[a_idx];
	}

	for (auto po_edge : po_edges) {
		if (p_a[po_edge->idx] != 0)
			hyperpath.push_back(make_pair(po_edge->id, p_a[po_edge->idx]));
	}

}

// sf_di, link set overhead
void Hyperpath::wrapper_run(string _oid, const string _did,
		const bp::object &weights_min, const bp::object &weights_max,
		const bp::object &h) {

	auto o_idx = g->get_vidx(_oid);
	auto d_idx = g->get_vidx(_did);

	//initialization
	vector<Edge*> po_edges;

	u_i[d_idx] = 0.0;
	p_i[o_idx] = 1.0;

	int j_idx = d_idx;
	int i_idx = 0;
	int a_idx = 0;

	// backward pass
	while (true) {
		auto j = g->get_vertex(j_idx);
		for (const auto &edge : j->in_edges) {
			a_idx = edge->idx;
			i_idx = edge->from_vertex->idx;
			j_idx = edge->to_vertex->idx;

			float temp = u_i[j_idx] + bp::extract<float>(weights_min[a_idx]) + bp::extract<float>(h[i_idx]);
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
		float w_max = bp::extract<float>(weights_max[a_idx]);
		float w_min = bp::extract<float>(weights_min[a_idx]);

		if (u_i[i_idx] >= u_i[j_idx] + w_min) {
			float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
			float P_a = f_a / (f_i[i_idx] + f_a);

			if (f_i[i_idx] == 0) {
				u_i[i_idx] = u_i[j_idx] + w_max;
			} else {
				if (u_i[i_idx]
						> (1 - P_a) * u_i[i_idx] + P_a * (u_i[j_idx] + w_min))
					u_i[i_idx] = (1 - P_a) * u_i[i_idx]
							+ P_a * (u_i[j_idx] + w_min);
			}

			f_i[i_idx] += f_a;
			po_edges.push_back(g->get_edge(a_idx)); //hyperpath is saved by id index of links

		}

		if (u_i[j_idx] + w_min + h[i_idx] > u_i[o_idx])
			break;
		j_idx = i_idx;

	}

	// forward pass

	sort(po_edges.begin(), po_edges.end(),
			[&](Edge* a, Edge* b)->bool
			{
				return u_i[a->to_vertex->idx] + weights_min[a->idx] > u_i[b->to_vertex->idx] + weights_min[b->idx];
			});

	for (auto po_edge : po_edges) {
		auto a_idx = po_edge->idx;
		auto i_idx = po_edge->from_vertex->idx;
		auto j_idx = po_edge->to_vertex->idx;
		float w_max = bp::extract<float>(weights_max[a_idx]);
		float w_min = bp::extract<float>(weights_min[a_idx]);
		float f_a = w_max == w_min ? LARGENUMBER : 1.0 / (w_max - w_min);
		float P_a = f_a / f_i[i_idx];
		p_a[a_idx] = P_a * p_i[i_idx];
		p_i[j_idx] += p_a[a_idx];
	}

	for (auto po_edge : po_edges) {
		if (p_a[po_edge->idx] != 0)
			hyperpath.push_back(make_pair(po_edge->id, p_a[po_edge->idx]));
	}

}

vector<pair<string, float> > Hyperpath::get_hyperpath() {
	return hyperpath;
}

bp::list Hyperpath::wrapper_get_hyperpath() {
	bp::list l;
	for (auto it = hyperpath.begin(); it != hyperpath.end(); it++) {
		bp::tuple e = bp::make_tuple((*it).first, (*it).second);
		l.append(e);
	}
	return l;
}

vector<string> Hyperpath::get_path_rec(string _oid, string _did) {
	vector<string> path_rec;

	auto vis = g->get_vertex(_oid);
	auto d = g->get_vertex(_did);
//    path_rec.push_back("110040");
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
			throw "ERROR: hyperpath not set, please conduct Hyperpath::run(string _oid, string _did, float* weights_min, float* weights_max, const float* h) first";
		}
	}
	return path_rec;
}

bp::list Hyperpath::wrapper_get_path_rec(string _oid, string _did) {
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
string Hyperpath::get_path_rec_vstring(const vector<string> & _path,
		string _delimiter) {
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

string Hyperpath::wrapper_get_path_rec_vstring(const bp::list &_path,
		const string _delimiter) {
	stringstream ss("");
	int len = bp::len(_path);
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

float Hyperpath::get_path_weights_sum(const vector<string>& _path,
		float * _weights_min) {
	float sum = 0.0;
	for (auto e_id : _path) {
		auto e_idx = g->get_edge(e_id)->idx;
		sum += _weights_min[e_idx];
	}
	return sum;
}
