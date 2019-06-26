/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Street.h
 * Author: yehchenh
 *
 * Created on January 24, 2019, 10:48 PM
 */

#ifndef STREET_H
#define STREET_H
#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include "StreetsDatabaseAPI.h"
#include "StreetSegment.h"
#include "Intersection.h"


class Street {
        
private:
    unsigned street_id; // Street ID
    std::string street_name; // Street name
    std::map<unsigned, StreetSegment> segments; // all segments on the street
    std::vector<unsigned>segment_ids;
    std::set<unsigned> intersections; // all segments on the street
    double street_length;
       
public:
    // Constructors
    Street();
    Street(const unsigned& id);
    
    // Destructor
    virtual ~Street();
    
    // Getters
    unsigned getStreetId();
    std::string getName();
    std::map<unsigned, StreetSegment> getStreetSegments();
    std::set<unsigned> getStreetIntersections();
    double getStreetLength();
    std::vector<unsigned> getSegmentIds();
    
    // Setters
    void setStreetId(unsigned newId);
    void setStreetName(std::string newName);
    void setStreetSegments(std::map<unsigned, StreetSegment> new_segments);
    void setStreetIntersections(std::set<unsigned> newIntersections);
    
    void insertStreetSegment(std::pair <unsigned, StreetSegment> new_street_segment);
    void insertStreetIntersection(unsigned newIntersectionID);
    void setStreetLength(double new_street_length);
    void setSegmentIds();
    
    
    

};

#endif /* STREET_H */

