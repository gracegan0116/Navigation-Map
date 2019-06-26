/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PointOfInterest.h
 * Author: yehchenh
 *
 * Created on January 30, 2019, 9:39 AM
 */

#ifndef POINTOFINTEREST_H
#define POINTOFINTEREST_H
#pragma once
#include <string>
#include "StreetsDatabaseAPI.h"

class PointOfInterest {
private:
    unsigned id;
    std::string type;
    std::string name;
    LatLon position;
    OSMID osmID;
    
public:
    
    bool highlight = false;
    
    PointOfInterest();
    PointOfInterest(unsigned poiIdx);
    virtual ~PointOfInterest();
    
    // Getters
    unsigned getId();
    std::string getType();
    std::string getName();
    LatLon      getPosition();
    OSMID       getOSMNodeID();
    
    void setId(unsigned newId);
    void setType(std::string& newType);
    void setName(std::string& newName);
    void setPosition(LatLon& newPosition);
    void setOSMNodeID(OSMID& newOsmID);


};

#endif /* POINTOFINTEREST_H */

