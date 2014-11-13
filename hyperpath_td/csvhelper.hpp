//
//  csvhelper.h
//  MyGraph
//
//  Created by tonny.achilles on 5/27/14.
//  Copyright (c) 2014 tonny.achilles. All rights reserved.
//
#include <vector>
#include <sstream>
#include <fstream>

#ifndef MyGraph_csvhelper_h
#define MyGraph_csvhelper_h

std::vector<std::vector<std::string> > load_csv(const std::string& filename,
                                                const char delimiter) {
    std::ifstream file(filename);
    // TODO: file good?
    if(!file.good()) {throw "file " + filename + " does not exist";}
    std::vector<std::vector<std::string> > matrix;
    std::vector<std::string> row;
    std::string line;
    std::string cell;
    
    while (file) {
        std::getline(file, line);
        std::stringstream lineStream(line);
        row.clear();
        
        while (std::getline(lineStream, cell, delimiter))
            row.push_back(cell);
        
        if (!row.empty())
            matrix.push_back(row);
    }
    
    return matrix;
}

#endif
