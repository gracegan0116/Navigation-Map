/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StreetSegment.cpp
 * Author: yehchenh
 * 
 * Created on January 25, 2019, 10:34 AM
 */

#include "StreetSegment.h"
#include "StreetsDatabaseAPI.h"
#include "m1.h"

StreetSegment::StreetSegment(unsigned id) {
    segment_id = id;
    InfoStreetSegment info_street_segment = getInfoStreetSegment(id);
    
    street_id = info_street_segment.streetID;
    toIntersection_id = info_street_segment.to;
    fromIntersection_id = info_street_segment.from;
    toIntersection = getIntersectionPosition(info_street_segment.to);
    fromIntersection = getIntersectionPosition(info_street_segment.from);
    num_curve_points = info_street_segment.curvePointCount;
    is_one_way = info_street_segment.oneWay;
    speedLimit = info_street_segment.speedLimit;
    
    // street seg length
    double totalLength = 0;
    
    if (num_curve_points == 0) { // no curve points
        street_segment_length = find_distance_between_two_points(toIntersection,
                fromIntersection);
    } else { // at least one curve point
        totalLength += find_distance_between_two_points(fromIntersection,
                getStreetSegmentCurvePoint(0, segment_id));
        
        // add 
        for (int i = 0; i < (num_curve_points - 1); ++i){
            // loop through each curve point to get respective length 
            // between each curve point
            totalLength += find_distance_between_two_points(
                    getStreetSegmentCurvePoint(i, segment_id), 
                    getStreetSegmentCurvePoint(i+1, segment_id));
        }
        
        // last segment from last curve point to final intersection
        totalLength += find_distance_between_two_points(getStreetSegmentCurvePoint(num_curve_points - 1, 
                segment_id),
                toIntersection);
        street_segment_length = totalLength;  
    }
    
    // travel time
    travel_time = street_segment_length / speedLimit * 3.6;
}


StreetSegment::StreetSegment() {
}

StreetSegment::~StreetSegment() {
}

// Getters
unsigned StreetSegment::getId(){
    return segment_id;
}
unsigned StreetSegment::getStreetId(){
    return street_id;
}

unsigned StreetSegment::getToIntersectionId(){
    return toIntersection_id;
}

unsigned StreetSegment::getFromIntersectionId(){
    return fromIntersection_id;
}

LatLon StreetSegment::getToIntersection(){
    return toIntersection;
}

LatLon StreetSegment::getFromIntersection(){
    return fromIntersection;
}

bool StreetSegment::getIsOneWay(){
    return is_one_way;
}

int StreetSegment::getNumCurvePoints(){
    return num_curve_points;
}

float StreetSegment::getSpeedLimit(){
    return speedLimit;
}

int StreetSegment::getIntersectionStreetSegmentCount(){
    return street_segment_count;
}

// Setters
void StreetSegment::setId(unsigned newId){
    segment_id = newId;
}

void StreetSegment::setStreetId(unsigned newStreetId){
    street_id = newStreetId;
}

void StreetSegment::setToIntersectionId(unsigned newId) {
    toIntersection_id = newId;
}
void StreetSegment::setFromIntersectionId(unsigned newId){
   fromIntersection_id = newId;
}

void StreetSegment::setToIntersection(LatLon newToIntersection){
    toIntersection = newToIntersection;
}

void StreetSegment::setFromIntersection(LatLon newFromIntersection) {
    fromIntersection = newFromIntersection;
}

void StreetSegment::setIsOneWay(bool newIsOneWay){
    is_one_way = newIsOneWay;
}

void StreetSegment::setNumCurvePoints(int newNum){
    num_curve_points = newNum;
}

void StreetSegment::setSpeedLimit(float newSpeedLimit) {
    speedLimit = newSpeedLimit;
}

void StreetSegment::setIntersectionStreetSegmentCount(int newStreetSegmentCount){
    street_segment_count = newStreetSegmentCount;
}

double StreetSegment::getStreetSegmentLength() {
    return street_segment_length;
    
}

void StreetSegment::setStreetSegmentLength() {
    double totalLength = 0;
    
    if (num_curve_points == 0) { // no curve points
        street_segment_length = find_distance_between_two_points(toIntersection,
                fromIntersection);
        return;
    } else { // at least one curve point
        totalLength += find_distance_between_two_points(fromIntersection,
                getStreetSegmentCurvePoint(0, segment_id));
        
        // add 
        for (int i = 0; i < (num_curve_points - 1); ++i){
            // loop through each curve point to get respective length 
            // between each curve point
            totalLength += find_distance_between_two_points(
                    getStreetSegmentCurvePoint(i, segment_id), 
                    getStreetSegmentCurvePoint(i+1, segment_id));
        }
        
        // last segment from last curve point to final intersection
        totalLength += find_distance_between_two_points(getStreetSegmentCurvePoint(num_curve_points - 1, 
                segment_id),
                toIntersection);
        street_segment_length = totalLength;
        
    }
     
    
}

double StreetSegment::getTravelTime() {
    return travel_time;
}

void StreetSegment::setTravelTime() {
    travel_time = street_segment_length / speedLimit * 3.6;
}

LatLon StreetSegment::getPosition(){
    return street_segment_position;
}