/* 
 * File:   Intersection.h
 * Author: yehchenh
 *
 * Created on January 25, 2019, 12:32 PM
 */

#ifndef INTERSECTION_H
#define INTERSECTION_H
#pragma once
#include <string>
#include "StreetsDatabaseAPI.h"
#include <set>

class Intersection {
private:
    unsigned intersection_id;
    OSMID osm_id;
    std::string intersection_name;
    LatLon intersection_position;
    int street_segment_count;
    

    
public:
    // Constructors
    Intersection();
    Intersection(unsigned intersectionId);
    // Destructor
    virtual ~Intersection();
    
    // Getters
    unsigned getIntersectionId();
    std::string getName();
    LatLon getPosition();
    int getStreetSegmentCount();
    
    bool highlight = false;
    bool highlight2 = false;

    // Setters
    void setIntersectionId(unsigned newId);
    void setIntersectionName(std::string& newName);
    void setIntersectionPosition(LatLon& newPosition);
    
    //flags
    unsigned reaching_edge; //id of the edge use to reach this node
    double travel_time; //shortest time found to this node so far

};

#endif /* INTERSECTION_H */

