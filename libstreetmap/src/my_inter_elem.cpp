/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   my_inter_elem.cpp
 * Author: yehchenh
 * 
 * Created on March 18, 2019, 2:50 PM
 */

#include "my_inter_elem.h"
my_inter_elem::my_inter_elem(){
    
}
my_inter_elem::my_inter_elem(Intersection *ip, unsigned ss_id, double t_t){
    inter_ptr = ip;
    street_seg_id = ss_id;
    travel_time = t_t;
    
}

my_inter_elem::my_inter_elem(const my_inter_elem& orig) {
    inter_ptr = orig.inter_ptr;
    street_seg_id = orig.street_seg_id;
    travel_time = orig.travel_time;
}

my_inter_elem::~my_inter_elem() {
}




