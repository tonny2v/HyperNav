//
//  PREDEFINE.h
//  MyGraph
//
//  Created by tonny.achilles on 5/29/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//

#ifndef MyGraph_PREDEFINE_h
#define MyGraph_PREDEFINE_h

//#define DRM_NUM_NODES 195669
//#define DRM_NUM_LINKS 460098

#define HDF5  "./data/large.h5"

#define CONNECTION "host=localhost port=5432 dbname=testdb user=postgres password=password"

constexpr auto GRAPH_BELL_ONEWAY_CSV_PATH = "./data/BellOneway.csv";

constexpr auto GRAPH_BELL_CSV_PATH = "./data/Bell_biway.csv";

constexpr auto PROFILE_TOKYO_MATSIM_CSV_PATH = "./data/maxdelay.csv";

constexpr auto GRAPH_TOKYO_DRM_BIDIRECTIONAL_CSV_PATH = "./data/Tokyo_DRM_bidirectional.csv";

constexpr auto GRAPH_TOKYO_MATSIM_CSV_PATH = "./data/Tokyo_matsim.csv";

constexpr auto TURN_RESTRICTIONS_CSV_PATH = "./data/turn_restrictions.csv";

#endif
