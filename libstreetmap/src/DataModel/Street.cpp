/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Street.cpp
 * Author: yehchenh
 * 
 * Created on January 24, 2019, 10:48 PM
 */



#include "Street.h"

Street::Street() {
}

Street::Street(const unsigned& id){
    street_id = id;
    street_name = getStreetName(id);
}

Street::~Street() {
}

// Getters
unsigned Street::getStreetId(){
    return street_id;
}
std::string Street::getName(){
    return street_name;
}
std::map<unsigned, StreetSegment> Street::getStreetSegments(){
    return segments;
}

std::set<unsigned> Street::getStreetIntersections(){
    return intersections;
}

    
// Setters
void Street::setStreetId(unsigned newId){
    street_id = newId;
}
void Street::setStreetName(std::string newName){
    street_name = newName;
}
void Street::setStreetSegments(std::map<unsigned, StreetSegment> new_segments){
    segments = new_segments;
}

void Street::insertStreetSegment(std::pair <unsigned, StreetSegment> new_street_segment) {
    segments.insert(new_street_segment);
}

void Street::setStreetIntersections(std::set<unsigned> newIntersections){
    intersections = newIntersections;
}

void Street::insertStreetIntersection(unsigned newIntersectionID){
    intersections.insert(newIntersectionID);
}


double Street::getStreetLength() {
    return street_length;
}

void Street::setStreetLength(double new_street_length){
    street_length = new_street_length;
}

void Street::setSegmentIds() {
    segment_ids.resize(segments.size());
    int i = 0;
    for (auto segment_it = segments.begin(); segment_it != segments.end(); ++segment_it) {
        segment_ids[i] = segment_it->first;
        i++;
    }
}

std::vector<unsigned> Street::getSegmentIds(){
    return segment_ids;
}