/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Feature.cpp
 * Author: yehchenh
 * 
 * Created on February 18, 2019, 4:58 PM
 */

#include "MapFeature.h"
#include <cmath>
#include "stdlib.h"

MapFeature::MapFeature() {
}
MapFeature::MapFeature(TypedOSMID id, std::string new_name, FeatureType new_type, std::vector<LatLon> new_pts, int num_pts) {
    osm_id = id;
    name = new_name;
    type = new_type;
    num_points = num_pts;
    pts_latlon = new_pts;
    int lastIndex = pts_latlon.size() - 1;
    
    // check if first and last lat lon are the same
    if (pts_latlon[0].lat() == pts_latlon[lastIndex].lat() && pts_latlon[0].lon() == pts_latlon[lastIndex].lon()) {
         is_closed = true;
    }else {
        is_closed = false;
    }
}

MapFeature::~MapFeature() {}

    
FeatureType MapFeature:: getType(){
    return type;
}

std::string MapFeature:: getName(){
    return name;
}

std::vector<LatLon> MapFeature::getPointsLatLon() {
    return pts_latlon;
}

bool MapFeature::getIsClosed() {
    return is_closed;
}

int MapFeature::getNumPoints(){
    return num_points;
}

TypedOSMID MapFeature::getOSM_ID(){
    return osm_id;
}






