#include "m3.h"
#include "m3_helper.h"
#include "m2.h"
#include "m1.h"
#include "MapData.h"
#include "ezgl/point.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/color.hpp"
#include "OSMDatabaseAPI.h"
#include "compare_pairs.h"
#include <cmath>
#include <cfloat>
#include <queue>
#include <iostream>
#include <vector>

double x_from_lon(double lon);
double y_from_lat(double lat);
double lon_from_x(double x);
double lat_from_y(double y);

bool get_if_street_intersects(unsigned street_segment1, unsigned street_segment2);
unsigned get_common_intersection (unsigned street_segment1, unsigned street_segment2);
double cross_product (double vec_one[], double vec_two[]);

#define NO_SEG -1
#define NO_EDGE -1

/**
 * determines if two street intersects
 * @param street_segment1
 * @param street_segment2
 * @return true if street_segment1 and street_segment2 intersects, false otherwise
 */
bool get_if_street_intersects(unsigned street_segment1, unsigned street_segment2){
    // common intersections
    if (g_street_segments[street_segment1].getToIntersectionId() == g_street_segments[street_segment2].getToIntersectionId())
        return true;
    if (g_street_segments[street_segment1].getToIntersectionId() == g_street_segments[street_segment2].getFromIntersectionId())
        return true;
    if (g_street_segments[street_segment1].getFromIntersectionId() == g_street_segments[street_segment2].getToIntersectionId())
        return true;
    if (g_street_segments[street_segment1].getFromIntersectionId() == g_street_segments[street_segment2].getFromIntersectionId())
        return true;
    
    return false; // have common intersections
}
/**
 * determine cross product of two vectors
 * @param vec_one
 * @param vec_two
 * @return z-component of vector cross product
 */
double cross_product(double vec_one[], double vec_two[]) {
    double cross_p[3];
    cross_p[0] = vec_one[1] * vec_two[2] - vec_one[2] * vec_two[1]; 
    cross_p[1] = vec_one[0] * vec_two[2] - vec_one[2] * vec_two[0]; 
    cross_p[2] = vec_one[0] * vec_two[1] - vec_one[1] * vec_two[0]; 
    
    return cross_p[2];   
}

/**
 * return the common intersection of two street segments
 * @param street_seg1
 * @param street_seg2
 * @return common intersection id
 */
unsigned get_common_intersection (unsigned street_segment1, unsigned street_segment2) {
    if (g_street_segments[street_segment1].getToIntersectionId() == g_street_segments[street_segment2].getToIntersectionId())
        return g_street_segments[street_segment1].getToIntersectionId();
    if (g_street_segments[street_segment1].getToIntersectionId() == g_street_segments[street_segment2].getFromIntersectionId())
        return g_street_segments[street_segment1].getToIntersectionId();
    if (g_street_segments[street_segment1].getFromIntersectionId() == g_street_segments[street_segment2].getToIntersectionId())
        return g_street_segments[street_segment1].getFromIntersectionId();
    if (g_street_segments[street_segment1].getFromIntersectionId() == g_street_segments[street_segment2].getFromIntersectionId())
        return g_street_segments[street_segment1].getFromIntersectionId();
    
    return -1; // no common intersections
}


// Returns the turn type between two given segments.
// street_segment1 is the incoming segment and street_segment2 is the outgoing
// one.
// If the two street segments do not intersect, turn type is NONE.
// Otherwise if the two segments have the same street ID, turn type is 
// STRAIGHT.  
// If the two segments have different street ids, turn type is LEFT if 
// going from street_segment1 to street_segment2 involves a LEFT turn 
// and RIGHT otherwise.  Note that this means that even a 0-degree turn
// (same direction) is considered a RIGHT turn when the two street segments
// have different street IDs.
TurnType find_turn_type(unsigned street_segment1, unsigned street_segment2){
    
    // does not intersect
    if (!get_if_street_intersects(street_segment1, street_segment2))
        return TurnType::NONE;
    // the same street ID
    if (g_street_segments[street_segment1].getStreetId() == g_street_segments[street_segment2].getStreetId()) 
        return TurnType::STRAIGHT;
    // same segment ID
    if (street_segment1 == street_segment2)
        return TurnType::STRAIGHT;
    
    // determine left or right turn
    // determine z-component of cross product of two vectors
    // if negative, right, if positive, left
    int common_intersection_id = get_common_intersection(street_segment1,street_segment2);
    
    // no common intersections
    if (common_intersection_id == -1)
        return TurnType::NONE; 
    
    // common intersection found
    double common_x = x_from_lon(g_intersections.at(common_intersection_id).getPosition().lon());
    double common_y = y_from_lat(g_intersections.at(common_intersection_id).getPosition().lat());

    unsigned street_segment1_intersection_id = common_intersection_id;
    double street_seg1_x = x_from_lon(g_intersections.at(street_segment1_intersection_id).getPosition().lon());
    double street_seg1_y = y_from_lat(g_intersections.at(street_segment1_intersection_id).getPosition().lat());
    // check street_segment1 the other intersection besides common_intersection_id
    if ((int) g_street_segments[street_segment1].getToIntersectionId() == common_intersection_id) {
        int dest_curve_point = g_street_segments[street_segment1].getNumCurvePoints();
        
        if (dest_curve_point > 0) {
            street_seg1_x = x_from_lon(getStreetSegmentCurvePoint(dest_curve_point-1, street_segment1).lon());
            street_seg1_y = y_from_lat(getStreetSegmentCurvePoint(dest_curve_point-1, street_segment1).lat());
        } else {
            street_seg1_x = x_from_lon(g_street_segments[street_segment1].getFromIntersection().lon());
            street_seg1_y = y_from_lat(g_street_segments[street_segment1].getFromIntersection().lat());
        }
    }
    
    else if ((int)g_street_segments[street_segment1].getToIntersectionId() != common_intersection_id) {
        int dest_curve_point = g_street_segments[street_segment1].getNumCurvePoints();
        
        if (dest_curve_point > 0) {
            street_seg1_x = x_from_lon(getStreetSegmentCurvePoint(0, street_segment1).lon());
            street_seg1_y = y_from_lat(getStreetSegmentCurvePoint(0, street_segment1).lat());
        } else {
            street_seg1_x = x_from_lon(g_street_segments[street_segment1].getToIntersection().lon());
            street_seg1_y = y_from_lat(g_street_segments[street_segment1].getToIntersection().lat());
        }
    }

    unsigned street_segment2_intersection_id = common_intersection_id;
    double street_seg2_x = x_from_lon(g_intersections.at(street_segment2_intersection_id).getPosition().lon());
    double street_seg2_y = y_from_lat(g_intersections.at(street_segment2_intersection_id).getPosition().lat());
    // check street_segment2 the other intersection besides common_intersection_id
    
    if ((int) g_street_segments[street_segment2].getToIntersectionId() == common_intersection_id) {
        int dest_curve_point = g_street_segments[street_segment2].getNumCurvePoints();
        if (dest_curve_point > 0) {
            street_seg2_x = x_from_lon(getStreetSegmentCurvePoint(dest_curve_point-1, street_segment2).lon());
            street_seg2_y = y_from_lat(getStreetSegmentCurvePoint(dest_curve_point-1, street_segment2).lat());
        } else {
            street_seg2_x = x_from_lon(g_street_segments[street_segment2].getFromIntersection().lon());
            street_seg2_y = y_from_lat(g_street_segments[street_segment2].getFromIntersection().lat());
        }
    }
    
    else if ((int)g_street_segments[street_segment2].getToIntersectionId() != common_intersection_id) {
        int dest_curve_point = g_street_segments[street_segment2].getNumCurvePoints();
        if (dest_curve_point > 0) {
            street_seg2_x = x_from_lon(getStreetSegmentCurvePoint(0, street_segment2).lon());
            street_seg2_y = y_from_lat(getStreetSegmentCurvePoint(0, street_segment2).lat());
        } else {
            street_seg2_x = x_from_lon(g_street_segments[street_segment2].getToIntersection().lon());
            street_seg2_y = y_from_lat(g_street_segments[street_segment2].getToIntersection().lat());
        }
    }
    
    // vector of street segment one with respect to common intersection
    double vect_A[] = { street_seg1_x - common_x, street_seg1_y - common_y, 0 }; 
    // vector of street segment two with respect to common intersection
    double vect_B[] = { common_x - street_seg2_x, common_y - street_seg2_y, 0}; 
    
    if (cross_product(vect_A, vect_B) <= 0)
        return TurnType::RIGHT;
    
    else if (cross_product(vect_A, vect_B) > 0)
        return TurnType::LEFT;

    return TurnType::NONE;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
}


// Returns the time required to travel along the path specified, in seconds.
// The path is given as a vector of street segment ids, and this function can
// assume the vector either forms a legal path or has size == 0.  The travel
// time is the sum of the length/speed-limit of each street segment, plus the
// given right_turn_penalty and left_turn_penalty (in seconds) per turn implied
// by the path.  If the turn type is STRAIGHT, then there is no penalty
double compute_path_travel_time(const std::vector<unsigned>& path, 
                                const double right_turn_penalty, 
                                const double left_turn_penalty){
    
    double total_time = 0;
    TurnType temp;
    
    //if the path vector is empty
    if (path.size()==0)
        return 0;
    
    //getting travel time of segments
    for (auto it = path.begin(); it!=path.end(); it++)
        total_time = total_time + find_street_segment_travel_time(*it);
    
    for(auto it = path.begin(); it!=path.end()-1; it++){
        temp = find_turn_type(*(it), *(it+1));
        if (temp == TurnType::RIGHT){
            total_time = total_time + right_turn_penalty;
        }
            
        else if(temp == TurnType::LEFT){
            total_time = total_time + left_turn_penalty;
        }
    }
    
    return total_time;
}

// Returns a path (route) between the start intersection and the end
// intersection, if one exists. This routine should return the shortest path
// between the given intersections, where the time penalties to turn right and
// left are given by right_turn_penalty and left_turn_penalty, respectively (in
// seconds).  If no path exists, this routine returns an empty (size == 0)
// vector.  If more than one path exists, the path with the shortest travel
// time is returned. The path is returned as a vector of street segment ids;
// traversing these street segments, in the returned order, would take one from
// the start to the end intersection.
std::vector<unsigned> find_path_between_intersections(
		  const unsigned intersect_id_start, 
                  const unsigned intersect_id_end,
                  const double right_turn_penalty, 
                  const double left_turn_penalty){
    
    int start = intersect_id_start;
    int end = intersect_id_end;
    
    std::vector<unsigned> empty_path;
    
    if (start == end)
        return empty_path;
    
    std::vector<unsigned> result;
    
    if(!bfs_path(&(g_intersections[start]),end,right_turn_penalty,left_turn_penalty)){
        return result;
    }
    
    result = bfs_trace_back(end);
   
    return result;
}

// find best segment from intersection id 1 to intersection id 2
// return best street seg id
unsigned find_best_seg (unsigned intersection_id1, unsigned intersection_id2) {
    unsigned best_seg  = -1;
    if (are_directly_connected(intersection_id1, intersection_id2)) {
        std::vector<unsigned> connected_segment_IDs_1 = find_intersection_street_segments(intersection_id1);
        std::vector<unsigned> connected_segment_IDs_2 = find_intersection_street_segments(intersection_id2);
        std::vector<unsigned> result;
        std::vector<unsigned> common_segments;
       
        std::set_intersection(connected_segment_IDs_1.begin(), connected_segment_IDs_1.end(),
            connected_segment_IDs_2.begin(), connected_segment_IDs_2.end(),
            std::back_inserter(result));

        if(result.size() ==  1){
            return result.front();
        }
       
        for (auto it = result.begin(); it != result.end(); ++it) {
            if (g_street_segments.at(*it).getIsOneWay()) {
                if(g_street_segments.at(*it).getFromIntersectionId() == intersection_id1 &&
                    g_street_segments.at(*it).getToIntersectionId() == intersection_id2){
                   
                    common_segments.push_back(g_street_segments.at(*it).getId());
                }
            } else {
                common_segments.push_back(g_street_segments.at(*it).getId());
            }
        }
       
        if (common_segments.empty()) {
            return best_seg;
        } else
            if (common_segments.size() == 1) {
            return common_segments[0];
        } else if (common_segments.size() > 1){
            double best_time = g_street_segments[common_segments[0]].getTravelTime();
            best_seg = common_segments[0];
            for (auto it = common_segments.begin(); it != common_segments.end(); it ++) {
                if (g_street_segments[*it].getTravelTime() < best_time) {
                    best_time = g_street_segments[*it].getTravelTime();
                    best_seg = *it;
                }
            }
            return best_seg;
        }
       
    }
    return best_seg;
}


//**************************************************************Finding shortest path****************************************

void printout(std::vector<unsigned> const &input)
{
    for (unsigned i = 0; i < input.size(); i++) {
        std::cout << input.at(i) << std::endl;
    }
}

bool bfs_path(Intersection* source, unsigned destination, double right_turn_penalty, double left_turn_penalty){
    
    reset_flags();
    
    std::priority_queue<my_inter_elem> wavefront;
    wavefront.push(my_inter_elem(source,NO_EDGE,0.0));
    
    bool found = false;
    double best_travel_time = FLT_MAX;
    double least_entry_time = 0.0;
    
    while((!wavefront.empty()) && (least_entry_time < best_travel_time)){
        
        //get the next node
        my_inter_elem current = wavefront.top();
        unsigned current_id = current.inter_ptr->getIntersectionId();
                
        //remove node from the front of wavefront
        wavefront.pop();
        
        //check if wavefront travel time for current path is better than older path
        if(least_entry_time < current.inter_ptr->travel_time){
            
            current.inter_ptr->reaching_edge = current.street_seg_id;
            current.inter_ptr->travel_time = current.travel_time;
            
            if(current_id == destination){
                //reach destination
                found = true;
                best_travel_time = current.inter_ptr->travel_time;
            }
            
            std::vector<unsigned> connected_street_segments = find_intersection_street_segments(current_id);
            
            unsigned to_intersecion_id;
            
            for(unsigned i = 0; i < connected_street_segments.size(); i++){
                
                //check which side of street segment is adjacent
                if(current_id == g_street_segments[connected_street_segments[i]].getToIntersectionId()){
                    
                    to_intersecion_id = g_street_segments[connected_street_segments[i]].getFromIntersectionId();
                
                }else{
                    
                    to_intersecion_id = g_street_segments[connected_street_segments[i]].getToIntersectionId();

                }
                
                //check if current node can reach this node
                if((!g_street_segments[connected_street_segments[i]].getIsOneWay()) ||
                        (current_id == g_street_segments[connected_street_segments[i]].getFromIntersectionId())){
                    
                    //set up turn penalty
                    double turn_penalty = 0.0; 
                    // prev outgoing street seg, curr outgoing street seg
                    if ((int) current.street_seg_id != NO_SEG) {
                        TurnType type = find_turn_type (current.street_seg_id,connected_street_segments[i]); 
                        switch(type) {
                            case TurnType::LEFT:
                                turn_penalty = left_turn_penalty;
                                break;
                            case TurnType::RIGHT:
                                turn_penalty = right_turn_penalty;
                                break;
                            default:
                                turn_penalty = 0.0;
                                break;
                        }
                    }
                    
                    
                    //initialize travel time and add current time
                    double path_travel_time = g_street_segments[connected_street_segments[i]].getTravelTime();
                    path_travel_time += current.travel_time + turn_penalty;
                    
                    //add new intersection
                    wavefront.push(my_inter_elem(&(g_intersections[to_intersecion_id]),connected_street_segments[i],path_travel_time));
                    
                }
                
            }
            
            if(!wavefront.empty()){
                least_entry_time =  wavefront.top().travel_time;
            }
            
        }
        
        
    }
    
    return found;
}

std::vector<unsigned> bfs_trace_back(unsigned destination){
    
    std::vector<unsigned> path;
    Intersection *current_node = &(g_intersections[destination]);
    unsigned previous_edge = current_node->reaching_edge;
    unsigned previous_node;
    
    while((int)previous_edge != NO_EDGE){
        path.push_back(previous_edge);
        
        if(g_street_segments[previous_edge].getFromIntersectionId() == current_node->getIntersectionId()){
            previous_node = g_street_segments[previous_edge].getToIntersectionId();
        }else{
            previous_node = g_street_segments[previous_edge].getFromIntersectionId();
        }
        
        current_node = &(g_intersections[previous_node]);
        previous_edge = current_node->reaching_edge;
    }
    
    std::reverse(path.begin(),path.end());
    
    reset_flags();
    
    return path;
    
}

void reset_flags(){
    
    for(unsigned i = 0; i < g_intersections.size(); i++){
        g_intersections[i].reaching_edge = NO_EDGE;
        g_intersections[i].travel_time = FLT_MAX;
    }
    
}


