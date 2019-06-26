/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Intersection.cpp
 * Author: yehchenh
 * 
 * Created on January 25, 2019, 12:32 PM
 */

#include "Intersection.h"

// Constructor
Intersection::Intersection(){
    
}

Intersection::Intersection(unsigned intersectionId){
    intersection_id = intersectionId; 
    intersection_name = getIntersectionName(intersectionId);
    intersection_position = getIntersectionPosition(intersectionId);
    osm_id = getIntersectionOSMNodeID(intersectionId);
    street_segment_count = getIntersectionStreetSegmentCount(intersectionId);
    
}


// Destructor
Intersection::~Intersection() {
}

// Getters
unsigned Intersection::getIntersectionId() {
    return intersection_id;
}
std::string Intersection::getName(){
    return intersection_name;
}
LatLon Intersection::getPosition(){
    return intersection_position;
}
    
// Setters
void Intersection::setIntersectionId(unsigned newId) {
    intersection_id = newId;
}
void Intersection::setIntersectionName(std::string& newName){
    intersection_name = newName;
}
void Intersection::setIntersectionPosition(LatLon& newPosition){
    intersection_position = newPosition;
}

int Intersection::getStreetSegmentCount() {
    return street_segment_count;
}
