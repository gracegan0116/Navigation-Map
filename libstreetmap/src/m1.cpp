#include "m1.h"
#include "m3_helper.h"
#include "MapData.h"
#include <iterator>
#include <cmath>
#include <set>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include "StreetsDatabaseAPI.h"
#include "Intersection.h"
#include "PointOfInterest.h"
#include "Street.h"
#include "StreetSegment.h"
#include "MapFeature.h"
#include "Constants/turn_penalty.h"
//extra features
#include "OSMDatabaseAPI.h"
#include <iostream>
#include <utility>

/*------------------------GLOBAL VARIABLES-----------------------*/
//key = intersection number, value = connected segment ids
std::vector<std::vector<unsigned>> g_intersection_street_segments;
//key = intersection number, value = street name
std::vector<std::vector<std::string>> g_intersection_street_name;
// key = street segment number, value = street segment
std::vector<StreetSegment> g_street_segments;
//key = street segment id, value = travel time
std::vector<double> g_street_segment_travel_time;

// hash map - streets sorted by id
std::vector<Street> g_streets; 
// all street segments in a street
std::unordered_map<unsigned, std::vector<unsigned>> g_street_street_segment_ids;
// all street intersections in a street
std::unordered_map<unsigned, std::vector<unsigned>> g_street_intersection_ids;
// map for street ids with the same name
std::map<std::string,std::vector<unsigned>> g_streets_by_name; 
 // hash map - intersections 
std::unordered_map<unsigned, Intersection> g_intersections;
// all point of interests
std::unordered_map<unsigned, PointOfInterest> g_pointOfInterests; 
// all features
std::vector<MapFeature> g_features_park;
std::vector<MapFeature> g_features_beach;
std::vector<MapFeature> g_features_lake;
std::vector<MapFeature> g_features_river;
std::vector<MapFeature> g_features_island;
std::vector<MapFeature> g_features_building;
std::vector<MapFeature> g_features_greenspace;
std::vector<MapFeature> g_features_golfcourse;
std::vector<MapFeature> g_features_stream;
std::vector<MapFeature> g_features_unknown;

std::vector<LatLon> restaurants;
std::vector<LatLon> clinics;
std::vector<LatLon> stations;
std::vector<LatLon> supermarkets;

double g_right_turn_penalty;
double g_left_turn_penalty;
//
std::vector< std::vector<my_inter_elem>> g_adj_list;
std::vector<std::vector< std::pair<double, my_inter_elem>>> g_dist;
//std::vector< std::pair<int, int> > g_dist = DijkstraSP(adjList, start);
//map name for layer1 API
//std::string map_name = "/cad2/ece297s/public/maps/toronto_canada.osm.bin";
/*------------------------GLOBAL VARIABLES-----------------------*/

//functions to load necessary map data into data structure
void loadAllMapData();
void loadStreetSegments();
void loadStreets();
void loadIntersections();
void loadStreetStreetSegments();
void loadPointOfInterests();
void loadFeatures();
void load_extra_features();
void load_adj_list();
void deter_turn_penalty(std::string map_location);

bool load_map(std::string map_path) {
    bool load_successful = loadStreetsDatabaseBIN(map_path); 
    bool load_OSMData = loadOSMDatabaseBIN(map_path.substr(0,map_path.length()-12)+".osm.bin");
    
    std::string map_location = map_path.substr(0,map_path.length()-12);
    
    
    if (load_successful){
        deter_turn_penalty(map_location);
        loadAllMapData();
//        g_adjList = FormAdjList();
//        g_dist = DijkstraSP(g_adjList, start);
        
        
    }
    if (load_OSMData)
        load_extra_features();
    return load_successful;
}

void deter_turn_penalty(std::string map_location) {
    const char *map_locations[] = {"beijing_china", "cairo_egypt",
            "cape-town_south-africa", "golden-horseshoe_canada", 
            "hamilton_canada", "hong-kong_china", "iceland", 
            "interlaken_switzerland", "london_england", "moscow_russia",
             "new-delhi_india", "new-york_usa", "rio-de-janeiro_brazil",
            "saint-helena", "singapore", "sydney_australia", "tehran_iran", 
            "tokyo_japan", "toronto_canada"};
    
    if (map_location.compare(map_locations[0]) == 0) { // beijing
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    } else if (map_location.compare(map_locations[1]) == 0) { // cairo_egypt
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[2]) == 0) { // cape-town_south-africa
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[3]) == 0) { // golden-horseshoe_canada
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[4]) == 0) { // hamilton_canada
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[5]) == 0) { // hong-kong_china
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[6]) == 0) { // iceland
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[7]) == 0) { // interlaken_switzerland
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[8]) == 0) { // london_england
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[9]) == 0) { // moscow_russia
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[10]) == 0) { // new-delhi_india
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[11]) == 0) { // new-york_usa
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[12]) == 0) { // rio-de-janeiro_brazil
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[13]) == 0) { // saint-helena
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[14]) == 0) { // singapore
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[15]) == 0) { // sydney_australia
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[16]) == 0) { // tehran_iran
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }else if (map_location.compare(map_locations[17]) == 0) { // tokyo_japan
        g_right_turn_penalty = left_way_right_penalty;
        g_left_turn_penalty = left_way_left_penalty;
    }else if (map_location.compare(map_locations[18]) == 0) { // toronto_canada
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    } else { // default
        g_right_turn_penalty = right_way_right_penalty;
        g_left_turn_penalty = right_way_left_penalty;
    }
}

void close_map() {
    closeStreetDatabase();
    closeOSMDatabase();
    g_intersection_street_segments.clear();
    g_intersection_street_name.clear();
    g_street_segments.clear(); 
    g_streets.clear(); 
    g_streets_by_name.clear();
    g_intersections.clear(); 
    g_pointOfInterests.clear(); 
    g_street_street_segment_ids.clear();
    g_street_intersection_ids.clear();
//    g_features.clear();
    g_features_park.clear();
    g_features_beach.clear();
    g_features_lake.clear();
    g_features_river.clear();
    g_features_island.clear();
    g_features_building.clear();
    g_features_greenspace.clear();
    g_features_golfcourse.clear();
    g_features_stream.clear();
    g_features_unknown.clear();
    restaurants.clear();
    clinics.clear();
    
}

//Returns the street segments connected to the given intersection 
std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {
    return g_intersection_street_segments[intersection_id];
}

//Returns the street names at the given intersection(includes duplicate street names in returned vector)
std::vector<std::string> find_intersection_street_names(unsigned intersection_id){
    return g_intersection_street_name[intersection_id]; 
}

//Returns true if you can get from intersection1 to intersection2 using a single street segment
bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2){

    if (intersection_id1 == intersection_id2) {
        return true;
    }
    
    std::vector<unsigned> connected_segment_IDs_1 = g_intersection_street_segments.at(intersection_id1);
    std::vector<unsigned> connected_segment_IDs_2 = g_intersection_street_segments.at(intersection_id2);
    std::vector<unsigned> result;
    std::set_intersection(connected_segment_IDs_1.begin(), connected_segment_IDs_1.end(),
            connected_segment_IDs_2.begin(), connected_segment_IDs_2.end(),
            std::back_inserter(result));
    
    for (auto it = result.begin(); it != result.end(); ++it) {
        if (g_street_segments.at(*it).getIsOneWay()) {

            if(g_street_segments.at(*it).getFromIntersectionId() == intersection_id1 &&
                g_street_segments.at(*it).getToIntersectionId() == intersection_id2){
                    return true;
            }
        } 
        // not one way
        else {
            return true; 
        }
    }
    return false;
}

//Returns all street segments for the given street
std::vector<unsigned> find_street_street_segments(unsigned street_id){
    return g_street_street_segment_ids.at(street_id);
}

//Returns all intersections along the given street 
std::vector<unsigned> find_all_street_intersections(unsigned street_id){
    return g_street_intersection_ids.at(street_id);
}


//Return all intersection ids for two intersecting streets
std::vector<unsigned> find_intersection_ids_from_street_ids(unsigned street_id1, 
                                                              unsigned street_id2){
    std::vector<unsigned> intersectionIds;

    std::sort (g_street_intersection_ids.at(street_id1).begin(), g_street_intersection_ids.at(street_id1).end());
    std::sort (g_street_intersection_ids.at(street_id2).begin(), g_street_intersection_ids.at(street_id2).end());
    
    std::set_intersection(g_street_intersection_ids.at(street_id1).begin(), 
                g_street_intersection_ids.at(street_id1).end(), 
                g_street_intersection_ids.at(street_id2).begin(), 
                g_street_intersection_ids.at(street_id2).end(), std::back_inserter(intersectionIds));
    
    return intersectionIds;
}

// return distance between two points in meters
double find_distance_between_two_points(LatLon point1, LatLon point2){
    
    double latavg = (point1.lat() * DEG_TO_RAD + point2.lat() * DEG_TO_RAD) / 2.;
    
    //implemented the equation given in milestone1
    double x1 = point1.lon() * DEG_TO_RAD * cos(latavg);
    double y1 = point1.lat() * DEG_TO_RAD;
    double x2 = point2.lon() * DEG_TO_RAD * cos(latavg);
    double y2 = point2.lat() * DEG_TO_RAD;
    
    return EARTH_RADIUS_IN_METERS * sqrt((y2-y1) * (y2-y1) + (x2-x1) * (x2-x1));
}

//returns the length of the given street segment in meters
double find_street_segment_length(unsigned street_segment_id){    
    return g_street_segments.at(street_segment_id).getStreetSegmentLength();
}

//returns the length of the specified street in meters
double find_street_length(unsigned street_id){
    return g_streets.at(street_id).getStreetLength();
}

//Returns all intersections reachable by traveling down one street segment from given intersection
std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
    //getting all the street segments connected to the given intersection_id
    std::vector<unsigned> reachable_intersections;
    std::vector<unsigned> connected_segment_IDs = g_intersection_street_segments.at(intersection_id);
    
    //loop through the street segments
    for(auto it = connected_segment_IDs.begin(); it != connected_segment_IDs.end(); it++){      
            if (!(g_street_segments.at(*it).getIsOneWay())){
                if(g_street_segments.at(*it).getToIntersectionId()!=intersection_id)
                    reachable_intersections.push_back(g_street_segments.at(*it).getToIntersectionId());
                else
                    reachable_intersections.push_back(g_street_segments.at(*it).getFromIntersectionId());
            }
            else{
                //One-way case
                if(g_street_segments.at(*it).getFromIntersectionId()==intersection_id)
                    reachable_intersections.push_back(g_street_segments.at(*it).getToIntersectionId());
            }
    }
    //remove duplicates
    std::sort (reachable_intersections.begin(), reachable_intersections.end()); 
    reachable_intersections.erase( 
            std::unique( reachable_intersections.begin(), reachable_intersections.end() ), 
            reachable_intersections.end());
    
    return reachable_intersections;
}

//Returns the travel time to drive a street segment in seconds
double find_street_segment_travel_time(unsigned street_segment_id){
    return g_street_segment_travel_time[street_segment_id];
}

//Returns the nearest point of interest to the given position
unsigned find_closest_point_of_interest(LatLon my_position){
    unsigned closest_poi = 0;
    double closest_distance = find_distance_between_two_points(
            my_position, 
            g_pointOfInterests.begin()->second.getPosition());

    for (auto poiIt = g_pointOfInterests.begin(); poiIt != g_pointOfInterests.end(); ++ poiIt) {
        if (find_distance_between_two_points(my_position, poiIt->second.getPosition()) < closest_distance) {
            closest_poi = poiIt->second.getId();
            closest_distance = find_distance_between_two_points(my_position, poiIt->second.getPosition());
        }
    }
    return closest_poi;
}

//Returns the nearest intersection to the given position
unsigned find_closest_intersection(LatLon my_position){
    unsigned closest_intersection = 0;
    double closest_distance = find_distance_between_two_points(
            my_position, 
            g_intersections.begin()->second.getPosition());
    
    for (auto interIt = g_intersections.begin(); interIt != g_intersections.end(); ++interIt) {
        if (find_distance_between_two_points(my_position, interIt->second.getPosition()) < closest_distance) {
            closest_intersection = interIt->second.getIntersectionId();
            closest_distance = find_distance_between_two_points(my_position, interIt->second.getPosition());
        }
    }
    return closest_intersection;
}

//Returns all street ids corresponding to street names that start with the given prefix
std::vector<unsigned> find_street_ids_from_partial_street_name(std::string street_prefix){
    std::vector<unsigned> result;
    
    //empty case
    if(street_prefix.length()==0)
        return result;
    //searching
    street_prefix = boost::algorithm::to_lower_copy(street_prefix);
    auto start_it = g_streets_by_name.lower_bound(street_prefix);
    while(start_it!=g_streets_by_name.end() && start_it->first.substr(0, street_prefix.length()) == street_prefix){
        result.insert(result.end(), start_it->second.begin(), start_it->second.end());
        start_it++;
    }
    //remove duplicates
    std::sort (result.begin(), result.end()); 
    result.erase(std::unique(result.begin(), result.end()), result.end());
    //output
    return result;  
}

// ------------functions to load necessary map data into data structure---------------
void loadAllMapData(){
    loadPointOfInterests();
    loadStreetSegments();
    loadStreets();
    loadIntersections();
    loadFeatures();
    
}

void loadStreetSegments() {
    for (int segmentId = 0; segmentId < getNumStreetSegments(); ++ segmentId) {
         StreetSegment segment (segmentId);
         g_street_segments.push_back(segment);
         g_street_segment_travel_time.push_back(segment.getTravelTime());
         
         // insert segment to street
         g_street_street_segment_ids[segment.getStreetId()].push_back(segmentId);
         g_street_intersection_ids[segment.getStreetId()].push_back(segment.getFromIntersectionId());
         g_street_intersection_ids[segment.getStreetId()].push_back(segment.getToIntersectionId());
         
         Intersection intersectionFrom(segment.getFromIntersectionId());
         Intersection intersectionTo(segment.getToIntersectionId());

    }
    // remove duplicates in intersection ids
    for (auto street_it = g_street_intersection_ids.begin(); street_it != g_street_intersection_ids.end(); ++ street_it) {
        // first sort, then remove
        std::sort (street_it->second.begin(), street_it->second.end()); 
        street_it->second.erase( 
                std::unique( street_it->second.begin(), street_it->second.end() ), 
                street_it->second.end());
    }
}

void loadStreets() {
    for (int streetId = 0; streetId < getNumStreets(); ++ streetId) {
        
        Street street (streetId);
        double street_length = 0;
        for (auto street_seg_it = g_street_street_segment_ids.at(streetId).begin(); 
                street_seg_it != g_street_street_segment_ids.at(streetId).end(); ++street_seg_it) {
            street_length += g_street_segments.at(*street_seg_it).getStreetSegmentLength();
        }
        street.setStreetLength(street_length);
        
        g_streets.push_back(street); 
        g_streets_by_name[boost::algorithm::to_lower_copy(street.getName())].push_back(streetId) ; 
    }
}


void loadIntersections() {
    //initializing intersection_street_segments and intersection_street_name
    for(int intersectionId = 0; intersectionId < getNumIntersections(); ++intersectionId) {
        std::vector<unsigned> ss_ID_holder;
        std::vector<std::string> street_names;
        
        Intersection intersection(intersectionId);
        g_intersections[intersectionId] = intersection;
        
        for (int i = 0; i < getIntersectionStreetSegmentCount(intersectionId); ++i) {
            int ss_id = getIntersectionStreetSegment(i, intersectionId);
            ss_ID_holder.push_back(ss_id);

            int street_id = getInfoStreetSegment(ss_id).streetID;
            std::string street_name = getStreetName(street_id);
            street_names.push_back(street_name);
        }
        std::sort(ss_ID_holder.begin(), ss_ID_holder.end());
        std::sort(street_names.begin(), street_names.end());

        g_intersection_street_segments.push_back(ss_ID_holder);
        g_intersection_street_name.push_back(street_names);            
        
    }
}
void loadPointOfInterests() {
    for (int poiIdx = 0; poiIdx < getNumPointsOfInterest(); ++poiIdx) {
        PointOfInterest poi (poiIdx);
        g_pointOfInterests[poiIdx] = poi;
    }
    
}

void loadFeatures() {
//    g_features.resize(getNumFeatures());
    
    for (int featIdx = 0; featIdx < getNumFeatures(); ++featIdx) {
        TypedOSMID id = getFeatureOSMID(featIdx);
        std::string name = getFeatureName(featIdx);
        FeatureType type = getFeatureType(featIdx);
        int point_count = getFeaturePointCount(featIdx);
        std::vector<LatLon> pts;
        
        
        for (int count = 0; count < point_count; ++count) {
            pts.push_back(getFeaturePoint(count, featIdx));
        }
        
        MapFeature feature(id, name, type, pts, point_count);
        switch(type) {
            case Park:
                g_features_park.push_back(feature);
                break;
            case Beach:
                g_features_beach.push_back(feature);
                break;
            case Lake:
                g_features_lake.push_back(feature);
                break;
            case River:
                g_features_river.push_back(feature);
                break;
            case Island:
                g_features_island.push_back(feature);
                break;
            case Building:
                g_features_building.push_back(feature);
                break;
            case Greenspace:
                g_features_greenspace.push_back(feature);
                break;
            case Golfcourse:
                g_features_golfcourse.push_back(feature);
                break;
            case Stream:
                g_features_stream.push_back(feature);
                break;
            case Unknown:
                g_features_unknown.push_back(feature);
                break;
            default:
                g_features_unknown.push_back(feature);
                break;
                
        }
        
//        g_features.push_back(feature);
        
      
    }
}

void load_extra_features(){
    std::pair<std::string, std::string> info;
    unsigned total_nodes = getNumberOfNodes();
    
    for(unsigned i = 0; i<total_nodes; i++){
        const OSMNode* current = getNodeByIndex(i);
        
        for(unsigned j = 0; j<getTagCount(current); j++){
            info = getTagPair(current, j);
            //loading restaurants in specified map
            if(info.first == "amenity" && info.second == "restaurant")
                restaurants.push_back(current->coords());
            //loading clinics in specified map
            else if(info.first == "amenity" && info.second == "clinic")
                clinics.push_back(current->coords());
            //loading supermarkets
            else if(info.first == "shop" && info.second == "supermarket")
                supermarkets.push_back(current->coords());
            else if(info.first == "railway" && info.second == "station")
                stations.push_back(current->coords());
        }
        
    }
}
