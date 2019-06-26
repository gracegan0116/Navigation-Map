/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Feature.h
 * Author: yehchenh
 *
 * Created on February 18, 2019, 4:58 PM
 */

#ifndef FEATURE_H
#define FEATURE_H
#include "StreetsDatabaseAPI.h"
#include "stdlib.h"
#include <string>
#include <vector>
#include "ezgl/point.hpp"

class MapFeature {
public:
    MapFeature();
    MapFeature(TypedOSMID id, std::string new_name, FeatureType new_type, std::vector<LatLon> new_pts, int num_pts);
    virtual ~MapFeature();
    std::string getName();
    FeatureType getType();
    bool getIsClosed();
    std::vector<LatLon> getPointsLatLon();
    int getNumPoints();
    TypedOSMID getOSM_ID();
private:
    TypedOSMID osm_id;
    std::string name;
    FeatureType type;
    std::vector<LatLon> pts_latlon;
    bool is_closed;
    int num_points;

};

#endif /* FEATURE_H */

