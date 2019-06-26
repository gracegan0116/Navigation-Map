/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   compare_pairs.h
 * Author: yehchenh
 *
 * Created on March 18, 2019, 3:03 PM
 */

#ifndef COMPARE_PAIRS_H
#define COMPARE_PAIRS_H

#include "my_inter_elem.h"


class compare_pairs {
public:
    bool operator() (my_inter_elem const& elem1, my_inter_elem const& elem2) {return elem1.travel_time > elem2.travel_time; }
//    bool operator<(my_inter_elem const& elem1, my_inter_elem const& elem2) { return elem1.travel_time > elem2.travel_time;}
//    bool operator>(my_inter_elem const& elem1, my_inter_elem const& elem2) {return elem1.travel_time < elem2.travel_time; }
//    bool operator<=(my_inter_elem const& elem1, my_inter_elem const& elem2){return elem1.travel_time >= elem2.travel_time;}
//    bool operator>=(my_inter_elem const& elem1, my_inter_elem const& elem2){return elem1.travel_time <= elem2.travel_time;}
//    bool operator==(my_inter_elem const& elem1, my_inter_elem const& elem2){return elem1.travel_time == elem2.travel_time;}
private:

};

#endif /* COMPARE_PAIRS_H */

