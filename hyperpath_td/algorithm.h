/*
 * Algorithm.h
 *
 *  Created on: May 24, 2014
 *      Author: tonny
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_
//#include <time.h>
#include <sys/timeb.h>

class Algorithm {
private:
	int start_ms;
    int get_now_ms() const;
    
public:
	Algorithm();
    ~Algorithm();
    int get_elapesd_ms() const;
};

#endif /* ALGORITHM_H_ */
