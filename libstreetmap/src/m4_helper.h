/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m4_helper.h
 * Author: linkuanc
 *
 * Created on March 31, 2019, 1:12 PM
 */

#include <map>
#include <unordered_map>

#ifndef M4_HELPER_H
#define M4_HELPER_H

unsigned search_closest_pick_up(const std::vector<unsigned>& all_pick_up_locations,
                                const std::vector<bool>& pick_up,
                                unsigned source, float& weight_identifier_pu,
                                const std::vector<DeliveryInfo>& deliveries);

unsigned search_closest_drop_off(const std::vector<unsigned>& all_drop_off_locations,
                                const std::vector<bool>& pick_up,
                                const unsigned source,
                                const std::vector<bool>& drop_off,
                                float& weight_identifier_do,
                                const std::vector<DeliveryInfo>& deliveries);

bool check_all_delivered(const std::vector<DeliveryInfo>& deliveries,
                    const std::vector<bool>& pick_up,
                    const std::vector<bool>& drop_off);

bool check_truck_capacity(float current_truck_weight,const float truck_capacity, float weight);

void pick_up_update(const std::vector<DeliveryInfo>& deliveries,
                    const std::multimap<unsigned,unsigned>& pick_up_packages,
                    unsigned source,
                    std::vector<bool>& pick_up,
                    float & total_weight, float weight_identifier);

float get_pick_up_weight(const std::vector<DeliveryInfo>& deliveries,
                         const std::multimap<unsigned,unsigned>& pick_up_packages,
                         unsigned closest_pick_up, float weight_identifier_pu);

bool pick_up_or_drop_off(unsigned closest_pick_up, unsigned closest_drop_off,unsigned source);

void drop_off_update(const std::vector<DeliveryInfo>& deliveries,
                    const std::multimap<unsigned,unsigned>& drop_off_packages,
                    unsigned destination,
                    std::vector<bool>& drop_off,
                    float & total_weight, float weight_identifier);


bool check_truck_leftover(const std::vector<DeliveryInfo>& deliveries,
                          float current_truck_weight,
                          const std::vector<bool>& pick_up,
                          const std::vector<bool>& drop_off
                          );

bool check_all_pick_up(const std::vector<bool>& pick_up);

unsigned search_first_pick_up(const std::vector<unsigned>& all_pick_up_locations,
                            float& weight_identifier_pu, unsigned& closest_depot,
                                const std::vector<DeliveryInfo>& deliveries,
                                const std::vector<unsigned>& depots);

#endif /* M4_HELPER_H */

