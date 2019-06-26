#ifndef M3_HELPER_H
#define M3_HELPER_H

#include "my_inter_elem.h"
void printout(std::vector<unsigned> const &input);

unsigned find_best_seg (unsigned intersection_id1, unsigned intersection_id2);

bool bfs_path(Intersection* source, unsigned destination, double right_turn_penalty, double left_turn_penalty);

std::vector<unsigned> bfs_trace_back(unsigned destination);

void reset_flags();



#endif /* M3_HELPER_H */

