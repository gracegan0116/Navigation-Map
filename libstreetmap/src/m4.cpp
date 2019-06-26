#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m3_helper.h"
#include "m4.h"
#include "m4_helper.h"
#include "MapData.h"
#include "OSMDatabaseAPI.h"
#include <cmath>
#include <cfloat>
#include <map>
#include <unordered_map>
#include <algorithm>

double x_from_lon(double lon);
double y_from_lat(double lat);
double lon_from_x(double x);
double lat_from_y(double y);

std::vector<CourierSubpath> traveling_courier(
		const std::vector<DeliveryInfo>& deliveries,
	       	const std::vector<unsigned>& depots, 
		const float right_turn_penalty, 
		const float left_turn_penalty, 
		const float truck_capacity){

    //vector for returning the function
    std::vector<CourierSubpath> result;
    
    //keep track of pick ups and drop offs
    std::vector<bool> pick_up(deliveries.size(), false);
    std::vector<bool> drop_off(deliveries.size(), false);
    std::vector<bool> pick_up_indices_tracker(deliveries.size(), false);
  
    
    //all pick up locations to faster use search_closest_pick_up
    std::vector<unsigned> all_pick_up_locations;
    
    //erase duplicates for all pick_up locations
    all_pick_up_locations.erase(std::unique(all_pick_up_locations.begin(),all_pick_up_locations.end()),all_pick_up_locations.end());
   
    
    //all drop off locations to faster use search_closest_pick_up
    std::vector<unsigned> all_drop_off_locations;
    
    //erase duplicates for all drop off locations
    all_drop_off_locations.erase(std::unique(all_drop_off_locations.begin(),all_drop_off_locations.end()),all_drop_off_locations.end());
    
    //pick-up locations for setting true and false
    std::multimap<unsigned,unsigned> pick_up_packages;
    
    //pick-up locations for setting true and false
    std::multimap<unsigned,unsigned> drop_off_packages;
    
    //load pick up intersections and drop off into vector
    for(unsigned i = 0; i < deliveries.size(); i++){
            
        unsigned pick_up_intersection = deliveries[i].pickUp;
        unsigned drop_off_intersection = deliveries[i].dropOff;
        
        all_pick_up_locations.push_back(pick_up_intersection);
        all_drop_off_locations.push_back(drop_off_intersection);
        
        pick_up_packages.insert(std::make_pair(pick_up_intersection,i));
        drop_off_packages.insert(std::make_pair(drop_off_intersection,i));
    }
    
    
    //******************************************DETERMINE START DEPOT TO FIRST PICK UP*******************
    unsigned destination = -1;
    float weight_identifier_pu = 0;
    float weight_identifier_do;
    
    unsigned closest_depot_start = -1;

    //find the closest first pick up point from all pick locations and depots
    destination = search_first_pick_up(all_pick_up_locations,
                            weight_identifier_pu, closest_depot_start,
                            deliveries,depots);
    
    //the start depot
    unsigned source = closest_depot_start;
    
    //empty vector
    std::vector<unsigned> empty;
    
    //initialize a CourierSubpath and add to result(depot->first pick up)
    CourierSubpath depot_to_start;
    depot_to_start.start_intersection = closest_depot_start;
    depot_to_start.end_intersection = destination;
    depot_to_start.subpath = find_path_between_intersections(closest_depot_start,destination,right_turn_penalty,left_turn_penalty);
    depot_to_start.pickUp_indices = empty;
    result.push_back(depot_to_start);
    
    //update source point
    source = destination;
    
    //initialize current truck weight
    float current_truck_weight = 0.0;
    
    //update the pick up packages
    pick_up_update(deliveries,pick_up_packages,source,pick_up,current_truck_weight,weight_identifier_pu);

    //determine source point is pick up or not
    bool source_is_pick_up = true;
     
    //if there's still packages that haven't been delivered
    while(!check_all_delivered(deliveries,pick_up,drop_off)){
        std::vector<unsigned> pick_up_indices;
        //set the pickUp_indices for the CourierSubpath
        if(source_is_pick_up){ 
            auto it = pick_up_packages.find(source);
            while(deliveries[it->second].itemWeight!=weight_identifier_pu || !pick_up[it->second]||pick_up_indices_tracker[it->second]==true)
                it = std::next(it,1);
            pick_up_indices.push_back(it->second);
            pick_up_indices_tracker[it->second] = true;
            
        }else{
            pick_up_indices = empty;
        }
      
        
        unsigned closest_pick_up = -1;
        float pick_up_weight = 0.0;
        unsigned closest_drop_off = search_closest_drop_off(all_drop_off_locations,pick_up,source,drop_off,weight_identifier_do,deliveries);
        
        closest_pick_up = search_closest_pick_up(all_pick_up_locations,pick_up,source,weight_identifier_pu,deliveries);
        pick_up_weight = weight_identifier_pu;
        
        
        //******************************************DETERMINE NEXT STEP TO PICK UP OR DROP OFF*******************
        //if truck is empty then go pick up
        if(current_truck_weight < 1){
            destination = closest_pick_up;
            source_is_pick_up = true;
            pick_up_update(deliveries,pick_up_packages,destination,pick_up,current_truck_weight,weight_identifier_pu);
        }
        //if truck haven't reach its max capacity and not all deliveries have been picked up
        else if(check_truck_capacity(current_truck_weight,truck_capacity,pick_up_weight) && !check_all_pick_up(pick_up)){
            if(((int)closest_pick_up != -1) && ((int)closest_drop_off != -1)){
                //if truck can pick up the package then go pick up and pick up is closer
                if(pick_up_or_drop_off(closest_pick_up,closest_drop_off,source)){
                    //pick up is closer then go pick up
                    destination = closest_pick_up;
                    source_is_pick_up = true;
                    pick_up_update(deliveries,pick_up_packages,destination,pick_up,current_truck_weight,weight_identifier_pu);

                }
                else{
                    //drop off is closer then go drop off
                    destination = closest_drop_off;
                    source_is_pick_up = false;
                    drop_off_update(deliveries,drop_off_packages,destination,drop_off,current_truck_weight,weight_identifier_do);
                }
                
            }
            else{
                //special case 
                destination = closest_drop_off;
                source_is_pick_up = false;
                drop_off_update(deliveries,drop_off_packages,destination,drop_off,current_truck_weight,weight_identifier_do);
                
                }
        }
        else{
            //special case
            destination = closest_drop_off;
            source_is_pick_up = false;
            drop_off_update(deliveries,drop_off_packages,destination,drop_off,current_truck_weight, weight_identifier_do);
        }
        
        //update CourierSubpath and add to result
        CourierSubpath sub_path;
        sub_path.start_intersection = source;
        sub_path.end_intersection = destination;        
        sub_path.subpath = find_path_between_intersections(source,destination,right_turn_penalty,left_turn_penalty);
        sub_path.pickUp_indices = pick_up_indices;
        result.push_back(sub_path);
        
        //update the source point
        source = destination;
        

    }

    //******************************************DETERMINE LAST DROP OFF TO END DEPOT*******************
    unsigned closest_depot_end = -1;
    double shortest_distance_end;
    
    if(depots.size() == 1){
        closest_depot_end = depots.front();
    }else{
        //from last drop off to end depot
        shortest_distance_end = DBL_MAX;

        for(auto it = depots.begin(); it != depots.end(); it++){
            double distance = find_distance_between_two_points(g_intersections[*it].getPosition(), g_intersections[destination].getPosition());
            if(distance <= shortest_distance_end){
                shortest_distance_end = distance;
                closest_depot_end = *it;
            }
        }
    }
    //update CourierPath for last drop off point to end depot
    CourierSubpath end_to_depot;
    end_to_depot.start_intersection = destination;
    end_to_depot.end_intersection = closest_depot_end;
    end_to_depot.subpath = find_path_between_intersections(destination,closest_depot_end,right_turn_penalty,left_turn_penalty);
    end_to_depot.pickUp_indices = empty;
    result.push_back(end_to_depot);
    
    return result;
    
}

//search closest pick up from all pick up locations and all depots
unsigned search_closest_pick_up(const std::vector<unsigned>& all_pick_up_locations,
                                const std::vector<bool>& pick_up,
                                unsigned source, float& weight_identifier_pu,
                                const std::vector<DeliveryInfo>& deliveries){
    
    double best_distance = DBL_MAX;
    unsigned closest_intersection = -1;
    
    for(unsigned i = 0; i < all_pick_up_locations.size(); i++){
        if(!pick_up[i]){
            double distance = find_distance_between_two_points(g_intersections[source].getPosition(), g_intersections[all_pick_up_locations[i]].getPosition());

            if(distance < best_distance){
                best_distance = distance;
                closest_intersection = all_pick_up_locations[i];
                weight_identifier_pu = deliveries[i].itemWeight;
            }
        }
    }
    
    return closest_intersection;
    
}

unsigned search_closest_drop_off(const std::vector<unsigned>& all_drop_off_locations,
                                const std::vector<bool>& pick_up,
                                const unsigned source,
                                const std::vector<bool>& drop_off,
                                float& weight_identifier_do,
                                const std::vector<DeliveryInfo>& deliveries){
    
    double best_distance = DBL_MAX;
    unsigned closest_intersection = -1;
    
    for(unsigned i = 0; i < all_drop_off_locations.size(); i++){
        if(pick_up[i] && !drop_off[i]){
            double distance = find_distance_between_two_points(g_intersections[source].getPosition(), g_intersections[all_drop_off_locations[i]].getPosition());
            
            if(distance < best_distance){
                best_distance = distance;
                closest_intersection = all_drop_off_locations[i];
                weight_identifier_do = deliveries[i].itemWeight;
            }
        }
    }
   
    return closest_intersection;
    
}

bool check_all_delivered(const std::vector<DeliveryInfo>& deliveries,
                        const std::vector<bool>& pick_up,
                        const std::vector<bool>& drop_off){
    
    
    for(unsigned i = 0; i < deliveries.size(); ++i) {
        if(!pick_up[i])
            return false;
        
        if(!drop_off[i])
            return false;
    }
    
    return true;
}

bool check_truck_capacity(float current_truck_weight,const float truck_capacity, float weight){
    
    if((current_truck_weight + weight) <= truck_capacity)
        return true;
    
    return false;
}


//update pick up deliveries and current truck weight
void pick_up_update(const std::vector<DeliveryInfo>& deliveries,
                    const std::multimap<unsigned,unsigned>& pick_up_packages,
                    unsigned source,
                    std::vector<bool>& pick_up,
                    float & total_weight, float weight_identifier){
    
    //auto it = pick_up_packages.find(source);
    auto it = pick_up_packages.lower_bound(source);
    //check if the weight matches
    while(deliveries[it->second].itemWeight!=weight_identifier)
        it = std::next(it,1);
    
    while(pick_up[it->second] == true)
        it = std::next(it,1);
    
    pick_up[it->second] = true;
    total_weight += weight_identifier; 

}

//update drop off deliveries and current truck weight
void drop_off_update(const std::vector<DeliveryInfo>& deliveries,
                    const std::multimap<unsigned,unsigned>& drop_off_packages,
                    unsigned destination,
                    std::vector<bool>& drop_off,
                    float & total_weight, float weight_identifier){
    
    auto it = drop_off_packages.lower_bound(destination);
    //auto it = drop_off_packages.find(destination);
    //check if the weight matches
    while(1)
        if(deliveries[it->second].itemWeight!=weight_identifier)
            it = std::next(it,1);
        else
            break;
    
    while(1)
        if(drop_off[it->second] == true || deliveries[it->second].itemWeight!=weight_identifier)
            it = std::next(it,1);
        else    
            break;

    drop_off[it->second] = true;
    total_weight -= weight_identifier; 
}

float get_pick_up_weight(const std::vector<DeliveryInfo>& deliveries,
                         const std::multimap<unsigned,unsigned>& pick_up_packages,
                         unsigned closest_pick_up, float weight_identifier_pu){
    
    auto it = pick_up_packages.find(closest_pick_up);
    while( deliveries[it->second].itemWeight!=weight_identifier_pu)
        it = std::next(it,1);
    float pick_up_weight = deliveries[it->second].itemWeight;
    
    return pick_up_weight;
}

bool pick_up_or_drop_off(unsigned closest_pick_up, unsigned closest_drop_off,unsigned source){
    
    double source_to_pick_up = find_distance_between_two_points(g_intersections[source].getPosition(), g_intersections[closest_pick_up].getPosition());
    double source_to_drop_off = find_distance_between_two_points(g_intersections[source].getPosition(), g_intersections[closest_drop_off].getPosition());
    
    if(source_to_pick_up <= source_to_drop_off)
        return true;
    
    return false;
}

//return true if all packages are picked up
bool check_all_pick_up(const std::vector<bool>& pick_up){
    
    for(unsigned i = 0; i < pick_up.size(); i++){
        if(!pick_up[i])
            return false;
    }
    
    return true;
    
}

unsigned search_first_pick_up(const std::vector<unsigned>& all_pick_up_locations,
                            float& weight_identifier_pu, unsigned& closest_depot,
                                const std::vector<DeliveryInfo>& deliveries,
                                const std::vector<unsigned>& depots){
    
    double best_distance = DBL_MAX;
    unsigned closest_intersection = -1;
    
    for(unsigned i = 0; i < depots.size(); i++){
       for(unsigned j = 0; j < all_pick_up_locations.size(); j++){
            
            double distance = find_distance_between_two_points(g_intersections[depots[i]].getPosition(), g_intersections[all_pick_up_locations[j]].getPosition());

            if(distance < best_distance){
                best_distance = distance;
                closest_intersection = all_pick_up_locations[j];
                weight_identifier_pu = deliveries[j].itemWeight;
                closest_depot = depots[i];
            }
            
        } 
    }
    
    return closest_intersection;
    
}
