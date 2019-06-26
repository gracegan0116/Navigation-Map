/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StreetSegment.h
 * Author: yehchenh
 *
 * Created on January 25, 2019, 10:34 AM
 */

#ifndef STREETSEGMENT_H
#define STREETSEGMENT_H

#include "StreetsDatabaseAPI.h"

class StreetSegment {
private:
    unsigned segment_id;
    unsigned street_id;
    unsigned toIntersection_id;
    unsigned fromIntersection_id;
    LatLon toIntersection;
    LatLon fromIntersection;
    int num_curve_points;
    bool is_one_way;
    float speedLimit;
    int street_segment_count;
    double street_segment_length;
    double travel_time;
    
    LatLon street_segment_position;

    
public:
    // Constructors
    StreetSegment();
    StreetSegment(unsigned id);
    // Destructor
    virtual ~StreetSegment();
    
    // Getters
    unsigned getId();
    unsigned getStreetId();
    unsigned getToIntersectionId();
    unsigned getFromIntersectionId();
    LatLon getToIntersection();
    LatLon getFromIntersection();
    bool getIsOneWay();
    int getNumCurvePoints();
    float getSpeedLimit();
    int getIntersectionStreetSegmentCount();
    double getStreetSegmentLength();
    double getTravelTime();
    
    // Setters
    void setId(unsigned newId);
    void setStreetId(unsigned newStreetId);
    void setToIntersectionId(unsigned newId);
    void setFromIntersectionId(unsigned newId);
    void setToIntersection(LatLon newToIntersection);
    void setFromIntersection(LatLon newFromIntersection);
    void setIsOneWay(bool isOneWay);
    void setNumCurvePoints(int newNumCurvePoints);
    void setSpeedLimit(float newSpeedLimit);
    void setIntersectionStreetSegmentCount(int newStreetSegmentCount);
    void setStreetSegmentLength();
    void setTravelTime();

    LatLon getPosition();
};

#endif /* STREETSEGMENT_H */

