/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   path_elem.h
 * Author: linkuanc
 *
 * Created on April 6, 2019, 12:30 PM
 */

#ifndef PATH_ELEM_H
#define PATH_ELEM_H

struct path_elem{
    
    //unsigned destination;
    bool pick_up_drop_off;
    float weight;
    unsigned index;
    std::vector<unsigned> pickup_indices;
    
};

#endif /* PATH_ELEM_H */

