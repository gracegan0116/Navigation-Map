/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MapData.h
 * Author: yehchenh
 *
 * Created on February 18, 2019, 3:32 PM
 */

#ifndef MAPDATA_H
#define MAPDATA_H
#include <stdlib.h>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include "my_inter_elem.h"
#include "StreetsDatabaseAPI.h"
#include "Intersection.h"
#include "PointOfInterest.h"
#include "Street.h"
#include "StreetSegment.h"
#include "MapFeature.h"

#pragma once
/*------------------------GLOBAL VARIABLES-----------------------*/
//key = intersection number, value = connected segment ids
 extern std::vector<std::vector<unsigned>> g_intersection_street_segments;
//key = intersection number, value = street name
 extern std::vector<std::vector<std::string>> g_intersection_street_name;
// key = street segment number, value = street segment
extern std::vector<StreetSegment> g_street_segments;
//key = street segment id, value = travel time
extern std::vector<double> g_street_segment_travel_time;

// hash map - streets sorted by id
extern std::vector<Street> g_streets; 
// all street segments in a street
extern std::unordered_map<unsigned, std::vector<unsigned>> g_street_street_segment_ids;
// all street intersections in a street
extern std::unordered_map<unsigned, std::vector<unsigned>> g_street_intersection_ids;
// map for street ids with the same name
extern std::map<std::string,std::vector<unsigned>> g_streets_by_name; 
// hash map - intersections 
extern std::unordered_map<unsigned, Intersection> g_intersections;
// all point of interests
extern std::unordered_map<unsigned, PointOfInterest> g_pointOfInterests; 
// all features
//extern std::vector<MapFeature> g_features;
extern std::vector<MapFeature> g_features_park;
extern std::vector<MapFeature> g_features_beach;
extern std::vector<MapFeature> g_features_lake;
extern std::vector<MapFeature> g_features_river;
extern std::vector<MapFeature> g_features_island;
extern std::vector<MapFeature> g_features_building;
extern std::vector<MapFeature> g_features_greenspace;
extern std::vector<MapFeature> g_features_golfcourse;
extern std::vector<MapFeature> g_features_stream;
extern std::vector<MapFeature> g_features_unknown;
extern std::vector<LatLon> restaurants;
extern std::vector<LatLon> clinics;
extern std::vector<LatLon> stations;
extern std::vector<LatLon> supermarkets;
extern std::vector<LatLon> stations;

extern double g_max_lat;
extern double g_min_lat;
extern double g_max_lon;
extern double g_min_lon;
extern double g_lat_avg;

extern double g_right_turn_penalty;
extern double g_left_turn_penalty;

extern std::vector< std::vector<my_inter_elem>> g_adj_list;
//extern std::vector<std::vector< std::pair<double, my_inter_elem>>> g_dist;

#endif /* MAPDATA_H */

