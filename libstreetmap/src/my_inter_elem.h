/* 
 * File:   my_inter_elem.h
 * Author: yehchenh
 *
 * Created on March 18, 2019, 2:50 PM
 */

#ifndef MY_INTER_ELEM_H
#define MY_INTER_ELEM_H

#include "Intersection.h"

/**
 * A Node in the Graph
 */
class my_inter_elem {
public:

    Intersection *inter_ptr;
    unsigned street_seg_id; // out going segment (edge) from intersection with id inter_id
    double travel_time;
    
    my_inter_elem();    
    my_inter_elem(Intersection *ip, unsigned ss_id, double t_t);    
    my_inter_elem(const my_inter_elem& orig);
    virtual ~my_inter_elem();
    
    bool operator<(my_inter_elem const& rhs) const { return travel_time > rhs.travel_time;}
    bool operator>(my_inter_elem const& rhs) const{return travel_time < rhs.travel_time; }
    bool operator<=(my_inter_elem const& rhs)const{return travel_time >= rhs.travel_time;}
    bool operator>=(my_inter_elem const& rhs)const{return travel_time <= rhs.travel_time;}
    bool operator==(my_inter_elem const& rhs)const{return travel_time == rhs.travel_time;}
private:

};

#endif /* MY_INTER_ELEM_H */

