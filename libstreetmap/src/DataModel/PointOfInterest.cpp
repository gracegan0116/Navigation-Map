/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PointOfInterest.cpp
 * Author: yehchenh
 * 
 * Created on January 30, 2019, 9:39 AM
 */

#include "PointOfInterest.h"

PointOfInterest::PointOfInterest() {
}

PointOfInterest::PointOfInterest(unsigned poiIdx) {
    id = poiIdx;
    type = getPointOfInterestType(poiIdx);
    name = getPointOfInterestName(poiIdx);
    position = getPointOfInterestPosition(poiIdx);
    osmID = getPointOfInterestOSMNodeID(poiIdx);
    
}

PointOfInterest::~PointOfInterest() {
}

unsigned PointOfInterest::getId() {
    return id;
}
std::string PointOfInterest::getType(){
    return type;
}
std::string PointOfInterest::getName(){
    return name;
}
LatLon PointOfInterest::getPosition(){
    return position;
}
OSMID PointOfInterest::getOSMNodeID(){
    return osmID;
}

void PointOfInterest::setId(unsigned newId){
    id = newId;
}
void PointOfInterest::setType(std::string& newType){
    type = newType;
}
void PointOfInterest::setName(std::string& newName){
    name = newName;
}
void PointOfInterest::setPosition(LatLon& newPosition){
    position = newPosition;
}
void PointOfInterest::setOSMNodeID(OSMID& newOsmID){
    osmID = newOsmID;
}

