#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m3_helper.h"
#include "MapData.h"
#include "OSMDatabaseAPI.h"
#include "string"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/color.hpp"
#include "ezgl/control.hpp"
#include "Constants/zoom_level.h"
#include <gtk/gtk.h>
#include "trie_node.h"
#include <iostream>
#include <cmath>
#include <string.h>
#include <gtk/gtk.h>
#include <curl/curl.h>

//http request
void get_ttc_info();

/*----- DECLARE FUNCTIONS -----*/
/******* Initializing *******/
void initialize_gtk_elements(ezgl::application *application);
void initialize_application();
void load_new_map(std::string map_name, ezgl::application* application);
void clear_to_reload_map();
void destroy_gtk_elements();

/******* Callback functions for event handling *******/
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y);
void search_button_callback(GtkWidget * widget, ezgl::application *application);
void intersection_click_reset_button_callback(GtkWidget *, ezgl::application *application);
void clear_search_button_callback(GtkWidget *, ezgl::application *application);
void enter_time_button_callback(GtkWidget *, ezgl::application *application);

/******* Reset zoom_fit to current world for different maps *******/
void press_zoom_fit_register_new_callback(GtkWidget *, ezgl::application *application);


/**
 * Return the current selected map location
 * @param widget the GUI widget this event came from
 * @param application the current application
 */
/******* Return Current Selected Map Location *******/
void combobox_onchanged(GtkComboBox *widget, ezgl::application* application);

void register_new_call_back(ezgl::application *application);

/******* Observe Entries *******/
void preedit_changed(GtkWidget * , ezgl::application *application);
void preedit_changed_two(GtkWidget * , ezgl::application *application);
void preedit_changed_three(GtkWidget * , ezgl::application *application);
void preedit_changed_four(GtkWidget * , ezgl::application *application);
void preedit_changed_right(GtkWidget *, ezgl::application *application);
void preedit_changed_left(GtkWidget *, ezgl::application *application);

/******* Toggles on User Interface *******/
void toggle_restaurants(GtkToggleButton *, ezgl::application *application);
void toggle_clinics(GtkToggleButton *, ezgl::application *application);
void toggle_supermarkets(GtkToggleButton *, ezgl::application *application);
void toggle_ttc(GtkToggleButton *, ezgl::application *application);
//void toggle_default(GtkToggleButton *, ezgl::application *application);
//void toggle_custom(GtkToggleButton *, ezgl::application *application);
void toggle_poi(GtkToggleButton *, ezgl::application *application);
void radio_default(GtkToggleButton *, ezgl::application *application);
void radio_custom(GtkToggleButton *, ezgl::application *application);


/******* Draw Functions *******/
void draw_main_canvas(ezgl::renderer &g);
void draw_intersections(ezgl::renderer &g);
void draw_features(ezgl::renderer &g);
void draw_features_unknown(ezgl::renderer &g);
void draw_features_park(ezgl::renderer &g);
void draw_features_beach(ezgl::renderer &g);
void draw_features_river(ezgl::renderer &g);
void draw_features_island(ezgl::renderer &g);
void draw_features_building(ezgl::renderer &g);
void draw_features_greenspace(ezgl::renderer &g);
void draw_features_golfcourse(ezgl::renderer &g);
void draw_features_stream(ezgl::renderer &g);
void draw_features_lake(ezgl::renderer &g);
void draw_streets(ezgl::renderer &g);
void draw_city_name(ezgl::renderer &g);
void draw_extra_features(ezgl::renderer &g);
void remove_highlights();
void display_street_names(ezgl::renderer &g);
void display_one_way_street(ezgl::renderer &g);
void display_point_of_interest_name(ezgl::renderer &g);

void update_point(ezgl::renderer &g);
void update_city_name(gchar* new_city_name);
void update_search_results(GtkEntry* text_entry, ezgl::application *application, const char* text);
void get_curr_zoom(ezgl::renderer &g);
bool is_in_curr_view(double x, double y);

/******* Milestone 3 Graphics *******/
void draw_path(ezgl::renderer &g, std::vector<unsigned> street_segment_ids);
void draw_information_box(GtkWidget *, ezgl::application *application);
void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data);

/******* Functions to convert lat lon to x y *******/
void set_lat_avg();
double x_from_lon(double lon);
double y_from_lat(double lat);
double lon_from_x(double x);
double lat_from_y(double y);
std::vector<ezgl::point2d>& get_points_xy(std::vector<LatLon>& pts);

/*-------GLOBAL VARIABLES-------*/
double g_max_lat;
double g_min_lat;
double g_max_lon;
double g_min_lon;
double g_lat_avg;
double g_initial_area;

/******* Current zoom level *******/
double g_curr_zoom;
ezgl::point2d g_curr_top_left = {0,0};
ezgl::point2d g_curr_top_right = {0,0};
ezgl::point2d g_curr_bottom_left = {0,0};
ezgl::point2d g_curr_bottom_right = {0,0};

bool mouse_pressed;
ezgl::point2d user_location(-1000,-1000);  // point of user

/******* Intersection IDs of clicked intersections *******/
int temp_prev_intersection_id = -1; //first click
int temp_prev_intersection_id2 = -1; //second click

/******* Intersection IDs of input search *******/
int common_intersection_id;
int common_intersection_id2;

/******* Point of Interest clicked location *******/
int temp_prev_poi = -1;

/******* Mouse Click *******/
bool reset_clicked = false;
int count_mouse_clicks = 0;
int count_intersection_click = 0;

bool m_initialized = false;
bool user_location_exists = false;
std::string curr_city_name = "toronto_canada"; // default is toronto

/******* GTK Elements *******/
GtkTextBuffer* buffer;
GtkTextBuffer* buffer2;
GtkTextBuffer* display_hint_message;
GtkTextBuffer* display_travel_time;

/******* Display Extra Features *******/
bool display_clinics = false;
bool display_restaurants = false;
bool display_supermarkets = false;
bool display_ttc = false;
bool display_poi = false;

/******* Display shortest route *******/
bool determine_draw_path = false;
std::vector<unsigned> draw_path_street_segments; //street segment vector to draw
bool error = true;
bool invalid_path = true;

/******* Time Penalty *******/
double right_penalty_time;
double left_penalty_time;
bool enter_time_error = false;
bool default_penalty = true;
bool custom_penalty = false;

/******* Draws the map which has been loaded with load_map() *******/
void draw_map(){
    g_max_lat = g_intersections[0].getPosition().lat();
    g_min_lat = g_max_lat;
    g_max_lon = g_intersections[0].getPosition().lon();
    g_min_lon = g_max_lon;
    set_lat_avg();
        
    for (size_t i = 0; i < g_intersections.size(); ++i) {
        g_max_lat = std::max(g_max_lat, g_intersections[i].getPosition().lat());
        g_min_lat = std::min(g_min_lat, g_intersections[i].getPosition().lat());
        g_max_lon = std::max(g_max_lon, g_intersections[i].getPosition().lon());
        g_min_lon = std::min(g_min_lon, g_intersections[i].getPosition().lon());
    }  
    initialize_application();
}

void initialize_application(){
        
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    
    ezgl::application application(settings);
  
    ezgl::rectangle initial_world({x_from_lon(g_min_lon),y_from_lat(g_min_lat)}, 
                                    {x_from_lon(g_max_lon),y_from_lat(g_max_lat)});
    
    g_initial_area = initial_world.area(); // get initial area bound
    
    application.add_canvas("MainCanvas",
        draw_main_canvas, 
        initial_world);       
    application.run(initialize_gtk_elements, act_on_mouse_press, nullptr, nullptr);
    
}

/******* Initializing GTK Elements *******/
void initialize_gtk_elements(ezgl::application *application){
    
    GtkWindow* mainWindow = (GtkWindow*)application->get_object("MainWindow");
    gtk_window_set_resizable(GTK_WINDOW(mainWindow), TRUE);

    // initialize list store for drop down menu to select maps
    if (! m_initialized) {
        GtkComboBoxText *combo_box;
        combo_box = (GtkComboBoxText*)application->get_object("ComboBox");
        const char *map_locations[] = {"select location","beijing_china", "cairo_egypt",
            "cape-town_south-africa", "golden-horseshoe_canada", 
            "hamilton_canada", "hong-kong_china", "iceland", 
            "interlaken_switzerland", "london_england", "moscow_russia",
             "new-delhi_india", "new-york_usa", "rio-de-janeiro_brazil",
            "saint-helena", "singapore", "sydney_australia", "tehran_iran", 
            "tokyo_japan", "toronto_canada"};

        for(unsigned i = 0; i < G_N_ELEMENTS(map_locations); i ++) {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), map_locations[i]);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box), 0);
        g_signal_connect(combo_box, "changed",
            G_CALLBACK(combobox_onchanged), application);
        m_initialized = true;
    }
    
    /******* set up text box *******/
    buffer = gtk_text_buffer_new(NULL); 
    buffer2 = gtk_text_buffer_new(NULL);
    display_hint_message = gtk_text_buffer_new(NULL);
    display_travel_time = gtk_text_buffer_new(NULL);
    
    /******* Set Up Buttons *******/
    
    //help button
    GtkButton* helpButton = (GtkButton*)application->get_object("HelpButton");
    g_signal_connect(helpButton, "clicked", G_CALLBACK(draw_information_box), application);
    
    //search button
    GtkButton* searchButton = (GtkButton*)application->get_object("SearchButton");
    if (NULL == searchButton) 
        std::cout << "Unable to find object SearchButton"<<std::endl;
    g_signal_connect(searchButton, "clicked", G_CALLBACK(search_button_callback), application);
    
    //intersection reset button
    GtkButton* intersectionResetButton = (GtkButton*)application->get_object("IntersectionResetButton");
    if (NULL == intersectionResetButton) 
        std::cout << "Unable to find object IntersectionResetButton"<<std::endl;
    g_signal_connect(intersectionResetButton, "clicked", G_CALLBACK(intersection_click_reset_button_callback), application);
    
    //clear search button
    GtkButton* clearSearchButton = (GtkButton*)application->get_object("ClearSearchButton");
    if (NULL == clearSearchButton) 
        std::cout << "Unable to find object ClearSearchButton"<<std::endl;
    g_signal_connect(clearSearchButton, "clicked", G_CALLBACK(clear_search_button_callback), application);
    
    //enter time penalty button
    GtkButton* enterTimeButton = (GtkButton*)application->get_object("EnterTimeButton");
    if (NULL == enterTimeButton) 
        std::cout << "Unable to find object EnterTimeButton"<<std::endl;
    g_signal_connect(enterTimeButton, "clicked", G_CALLBACK(enter_time_button_callback), application);
    
    //google map button
    GtkButton* googleMapButton = (GtkButton*)application->get_object("GoogleMap");
    std::string message = "Google Maps";
    int message_length = message.length();

    // convert string to char array
    char *message_char_arr = new char[message_length + 1];
    strcpy(message_char_arr, message.c_str());

    gtk_button_set_label(googleMapButton,message_char_arr);
    
    delete [] message_char_arr;
    
    
    
    /******* Set up Text Box *******/
    //Information 1 (intersection 1)
    GtkTextView* infoTextView = (GtkTextView*)application->get_object("InfoTextView");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(infoTextView), GTK_WRAP_WORD);
    gtk_text_view_set_buffer(infoTextView, buffer);
    
    //Information 2 (intersection 2)
    GtkTextView* infoTextView2 = (GtkTextView*)application->get_object("InfoTextView2");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(infoTextView2), GTK_WRAP_WORD);
    gtk_text_view_set_buffer(infoTextView2, buffer2);
    
    //Hint Message
    GtkTextView* hintInfoTextView = (GtkTextView*)application->get_object("HintInfoTextView");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(hintInfoTextView), GTK_WRAP_WORD);
    gtk_text_view_set_buffer(hintInfoTextView, display_hint_message);
    
    //Minimum Travel Time
    GtkTextView* travelTimeInfoTextView = (GtkTextView*)application->get_object("TravelTimeTextView");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(travelTimeInfoTextView), GTK_WRAP_WORD);
    gtk_text_view_set_buffer(travelTimeInfoTextView, display_travel_time);
    
    /******* Search Input *******/
    //Find common intersection 1
    GtkEntry* text_entry = (GtkEntry*) application->get_object("SearchInput");
    g_signal_connect(text_entry, "changed", G_CALLBACK(preedit_changed), application);
    
    GtkEntry* text_entry_second = (GtkEntry*) application->get_object("SearchInputTwo");
    g_signal_connect(text_entry_second, "changed", G_CALLBACK(preedit_changed_two), application);
    
    //Find common intersection 2
    GtkEntry* text_entry_third = (GtkEntry*) application->get_object("SearchInputThree");
    g_signal_connect(text_entry_third, "changed", G_CALLBACK(preedit_changed_three), application);
    
    GtkEntry* text_entry_four = (GtkEntry*) application->get_object("SearchInputFour");
    g_signal_connect(text_entry_four, "changed", G_CALLBACK(preedit_changed_four), application);
    
    //penalty time
    GtkEntry* text_entry_right = (GtkEntry*) application->get_object("RightTurnPenalty");
    g_signal_connect(text_entry_right, "changed", G_CALLBACK(preedit_changed_right), application);
    
    GtkEntry* text_entry_left = (GtkEntry*) application->get_object("LeftTurnPenalty");
    g_signal_connect(text_entry_left, "changed", G_CALLBACK(preedit_changed_left), application);

    /******* Check Box *******/
    //for User to select which extra features to display
    //restaurant
    GtkCheckButton* restaurant_checkbutton = (GtkCheckButton*) application->get_object("CheckButtonRestaurants");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (restaurant_checkbutton), FALSE);
    g_signal_connect(restaurant_checkbutton, "toggled", G_CALLBACK(toggle_restaurants), application);
    
    //supermarkets
    GtkCheckButton* supermarket_checkbutton = (GtkCheckButton*) application->get_object("CheckButtonSuperMarkets");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (supermarket_checkbutton), FALSE);
    g_signal_connect(supermarket_checkbutton, "toggled", G_CALLBACK(toggle_supermarkets), application);
    
    //clinics
    GtkCheckButton* clinics_checkbutton = (GtkCheckButton*) application->get_object("CheckButtonClinics");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (clinics_checkbutton), FALSE);
    g_signal_connect(clinics_checkbutton, "toggled", G_CALLBACK(toggle_clinics), application);
    
    //TTC stations
    GtkCheckButton* ttc_checkbutton = (GtkCheckButton*) application->get_object("CheckButtonTTC");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (ttc_checkbutton), FALSE);
    g_signal_connect(ttc_checkbutton, "toggled", G_CALLBACK(toggle_ttc), application);

    //point of interest
    GtkCheckButton* poi_checkbutton = (GtkCheckButton*) application->get_object("PointOfInterest");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (poi_checkbutton), FALSE);
    g_signal_connect(poi_checkbutton, "toggled", G_CALLBACK(toggle_poi), application);
    
    /***** Radio Button *****/
    //default button
    GtkRadioButton* default_radiobutton = (GtkRadioButton*) application->get_object("DefaultRadioButton");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (default_radiobutton), TRUE);
    g_signal_connect(default_radiobutton, "toggled", G_CALLBACK(radio_default), application);
    
    //custom button
    GtkRadioButton* custom_radiobutton = (GtkRadioButton*) application->get_object("CustomRadioButton");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (custom_radiobutton), FALSE);
    g_signal_connect(custom_radiobutton, "toggled", G_CALLBACK(radio_custom), application);
    
    /******* Display initial hint message *******/
    std::string hint_message = "Please enter / click on starting location and destination";
    int hint_message_length = hint_message.length();

    // convert string to char array
    char *hint_message_char_arr = new char[hint_message_length + 1];
    strcpy(hint_message_char_arr, hint_message.c_str());
    gtk_text_buffer_set_text(display_hint_message, hint_message_char_arr, -1);

    delete [] hint_message_char_arr;
    
    /***** Default Penalty Time Display *****/
    if(default_penalty){
        std::string right = std::to_string(g_right_turn_penalty);
        int right_length = right.length();

        // convert string to char array
        char *right_char_arr = new char[right_length + 1];
        strcpy(right_char_arr, right.c_str());

        std::string left = std::to_string(g_left_turn_penalty);
        int left_length = left.length();

        // convert string to char array
        char *left_char_arr = new char[left_length + 1];
        strcpy(left_char_arr, left.c_str());

        gtk_entry_set_text(text_entry_right,right_char_arr);
        gtk_entry_set_text(text_entry_left,left_char_arr);

        delete [] right_char_arr;
        delete [] left_char_arr;

    }
    
}

void destroy_gtk_elements() {
//    destroy_button();
}

/******* Current zoom level *******/
void get_curr_zoom(ezgl::renderer &g) {
    g_curr_zoom = g.get_visible_world().area() / g_initial_area;
    g_curr_top_left = g.get_visible_world().top_left();
    g_curr_top_right = g.get_visible_world().top_right();
    g_curr_bottom_left = g.get_visible_world().bottom_left();
    g_curr_bottom_right = g.get_visible_world().bottom_right();

}

bool is_in_curr_view(double x, double y) {
    if (x >= g_curr_top_left.x && x <= g_curr_top_right.x
                && y <= g_curr_top_left.y && y >= g_curr_bottom_left.y) {
            return true;
    }
    
        return false;
}

/******* Drawing the map *******/
void draw_main_canvas(ezgl::renderer &g) {
    
    //background color
    g.set_color({240,240,240,255}); //grey 
    g.fill_rectangle(g.get_visible_world());
    g.draw_rectangle({0,0}, {1000, 1000});
    
    get_curr_zoom(g);
    g.draw_rectangle(g_curr_top_left, g_curr_bottom_right);

//    draw_features(g);
    draw_streets(g);
    
    if(determine_draw_path){
        draw_path(g,draw_path_street_segments);
    }

//    if(determine_draw_path && !error){
//        draw_path(g,draw_path_street_segments);
//    }
    
//    if (g_curr_zoom <= LEVEL_3_ZOOM){
//        display_street_names(g);
//    }
    
    if (g_curr_zoom <= LEVEL_3_ZOOM){
        draw_intersections(g);
    }

    if (g_curr_zoom <= LEVEL_3_ZOOM){
        display_one_way_street(g);
        
    } 

    
    if (g_curr_zoom <= LEVEL_6_ZOOM){
        if(display_poi)
            display_point_of_interest_name(g);
    } 

    

    if (g_curr_zoom <= LEVEL_5_ZOOM){
        draw_extra_features(g);
    }
    draw_city_name(g);
//    
//    if (g_curr_zoom <= LEVEL_6_ZOOM){
//        if(display_poi)
//            display_point_of_interest_name(g);
//    } 
//
//    draw_city_name(g);
//
//    if (g_curr_zoom <= LEVEL_5_ZOOM){
//        draw_extra_features(g);
//    }

    update_point(g);

}

/******* display city name on the map *******/
void draw_city_name(ezgl::renderer &g){
    
    double mid_x = x_from_lon((g_max_lon + g_min_lon) / 2.);
    double mid_y = y_from_lat((g_max_lat + g_min_lat) / 2.);
    
    if (g_curr_zoom >= DEFAULT_ZOOM) {
        g.set_color(ezgl::BLACK);
        g.set_font_size(24);
        g.draw_text({mid_x, mid_y}, curr_city_name);
    }
}

/******* Draw intersection *******/
void draw_intersections(ezgl::renderer &g) {
    get_curr_zoom(g);
    for (size_t i = 0; i < g_intersections.size(); ++i) { 
        float x = x_from_lon(g_intersections[i].getPosition().lon());
        float y = y_from_lat(g_intersections[i].getPosition().lat());
        
        float radius = 0.00005;

        if (g_intersections[i].highlight){
            g.set_color(ezgl::RED);
            g.fill_arc({x,y}, radius, 0, 360);
        }else if(g_intersections[i].highlight2){
            g.set_color(ezgl::GREEN);
            g.fill_arc({x,y}, radius, 0, 360);
        }
        else{
            g.set_color({252,220,220,255});
            g.fill_arc({x,y}, radius, 0, 360);
        }
    }
}

/******* draw restaurants, clinic, supermarket, and subway stations icons on the map *******/
void draw_extra_features(ezgl::renderer &g){
    double x,y;
    unsigned i = 0;
    int r, c, s, t=0;
    
    ezgl::surface* restaurant_icon = ezgl::renderer::load_png("libstreetmap/resources/restaurant.png");
    ezgl::surface* clinic_icon = ezgl::renderer::load_png("libstreetmap/resources/clinic.png");
    ezgl::surface* supermarket_icon = ezgl::renderer::load_png("libstreetmap/resources/supermarket.png");
    ezgl::surface* subway_icon = ezgl::renderer::load_png("libstreetmap/resources/subway.png");

    //sets the number of icons to be drawn on the map
    if (g_curr_zoom > DEFAULT_ZOOM ){
        r = restaurants.size() - (int)sqrt(restaurants.size())/2;
        c = 2;
        s = supermarkets.size() - (int)sqrt(supermarkets.size());
        t = 151;
    }
    else if (g_curr_zoom > LEVEL_1_ZOOM){
        r = restaurants.size() - (int)sqrt(restaurants.size()*4); 
        c = 1;
        s = supermarkets.size() - (int)sqrt(supermarkets.size()*10); 
        t = 141;
    }
    else if (g_curr_zoom > LEVEL_2_ZOOM || g_curr_zoom > LEVEL_3_ZOOM){
        r = restaurants.size() - (int)sqrt(restaurants.size()*5); 
        c = 1;
        s = supermarkets.size() - (int)sqrt(supermarkets.size()*20);
        t = 100;
    }
    else if(g_curr_zoom > LEVEL_5_ZOOM){
        r = restaurants.size() - (int)sqrt(restaurants.size()*8); 
        c = 60;
        s = supermarkets.size() - (int)sqrt(supermarkets.size()*40); 
    }
    else if(g_curr_zoom > LEVEL_6_ZOOM || g_curr_zoom > LEVEL_7_ZOOM){
        r = restaurants.size()/2;
        c = 1;
        s = supermarkets.size()/2;
    }
    else{
        r = 0;
        c = 1;
        s = 0;
    }
    
    /******* Draw icons *******/
    //restaurant
    if (display_restaurants) {
        for(i = r; i<restaurants.size(); i++){
                x = x_from_lon(restaurants[i].lon());
                y = y_from_lat(restaurants[i].lat());
                g.draw_surface(restaurant_icon, {x,y});
        }
    }
    
    //clinics
    if (display_clinics) {
        for(i = (int)clinics.size()/c; i<clinics.size(); i++){
            x = x_from_lon(clinics[i].lon());
            y = y_from_lat(clinics[i].lat());
            g.draw_surface(clinic_icon, {x,y});
        }
    }
    
    //super markets
    if (display_supermarkets) {
        for(i = s; i<supermarkets.size(); i++){
            x = x_from_lon(supermarkets[i].lon());
            y = y_from_lat(supermarkets[i].lat());
            g.draw_surface(supermarket_icon, {x,y});
        }
    }
    
    //TTC stations
    if (display_ttc) {
        for(i=0; i<(unsigned)stations.size()-t;i++){
            x = x_from_lon(stations[i+t].lon());
            y = y_from_lat(stations[i+t].lat());
            g.draw_surface(subway_icon, {x,y});
        }
    }
    
    ezgl::renderer::free_surface(restaurant_icon);
    ezgl::renderer::free_surface(clinic_icon);
    ezgl::renderer::free_surface(supermarket_icon);
    ezgl::renderer::free_surface(subway_icon);
}

/******* draw streets of the map *******/
void draw_streets(ezgl::renderer &g){    
    get_curr_zoom(g);

    for(size_t i = 0; i < g_streets.size(); ++i){
        for(auto it = g_street_street_segment_ids[i].begin(); it != g_street_street_segment_ids[i].end(); ++it){
            
            //draw street segments with 0 curvepoints
            if(g_street_segments.at(*it).getNumCurvePoints() == 0){
                float x_from_intersection = x_from_lon(g_street_segments.at(*it).getFromIntersection().lon());
                float y_from_intersection = y_from_lat(g_street_segments.at(*it).getFromIntersection().lat());
                float x_to_intersection = x_from_lon(g_street_segments.at(*it).getToIntersection().lon());
                float y_to_intersection = y_from_lat(g_street_segments.at(*it).getToIntersection().lat());
                
                if(g_curr_zoom <= LEVEL_3_ZOOM){
                    if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(4);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_to_intersection, y_to_intersection});

                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(6);
                            g.draw_line({x_from_intersection,y_from_intersection}, {x_to_intersection, y_to_intersection});

                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(8);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_to_intersection, y_to_intersection});
                    }
                     
                 }
                if(g_curr_zoom <= LEVEL_6_ZOOM){
                    if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(7);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_to_intersection, y_to_intersection});
                        
                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(9);
                            g.draw_line({x_from_intersection,y_from_intersection}, {x_to_intersection, y_to_intersection});
                            
                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(11);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_to_intersection, y_to_intersection});
                    }
                 }
            }
            //corner case -- draw street segments with 1 curve point
            else if(g_street_segments.at(*it).getNumCurvePoints() == 1){
                
            
                float x_from_intersection = x_from_lon(g_street_segments.at(*it).getFromIntersection().lon());
                float y_from_intersection = y_from_lat(g_street_segments.at(*it).getFromIntersection().lat());
                float x_first_curve_point = x_from_lon(getStreetSegmentCurvePoint(0, *it).lon());
                float y_first_curve_point = y_from_lat(getStreetSegmentCurvePoint(0, *it).lat());
                float x_to_intersection = x_from_lon(g_street_segments.at(*it).getToIntersection().lon());
                float y_to_intersection = y_from_lat(g_street_segments.at(*it).getToIntersection().lat());
                
                if(g_curr_zoom <= LEVEL_3_ZOOM){
                    if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(4);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                        g.draw_line({x_first_curve_point,y_first_curve_point}, {x_to_intersection, y_to_intersection});

                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(6);
                            g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                            g.draw_line({x_first_curve_point,y_first_curve_point}, {x_to_intersection, y_to_intersection});

                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(8);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                        g.draw_line({x_first_curve_point,y_first_curve_point}, {x_to_intersection, y_to_intersection});
                    }
                     
                 }
                if(g_curr_zoom <= LEVEL_6_ZOOM){
                    if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(7);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                        g.draw_line({x_first_curve_point,y_first_curve_point}, {x_to_intersection, y_to_intersection});
                        
                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(9);
                            g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                            g.draw_line({x_first_curve_point,y_first_curve_point}, {x_to_intersection, y_to_intersection});
                            
                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(11);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                        g.draw_line({x_first_curve_point,y_first_curve_point}, {x_to_intersection, y_to_intersection});
                    }
                 }

            }
            //draw street segments with more than one curve points
            else{
                float x_from_intersection = x_from_lon(g_street_segments.at(*it).getFromIntersection().lon());
                float y_from_intersection = y_from_lat(g_street_segments.at(*it).getFromIntersection().lat());
                float x_first_curve_point = x_from_lon(getStreetSegmentCurvePoint(0, *it).lon());
                float y_first_curve_point = y_from_lat(getStreetSegmentCurvePoint(0, *it).lat());

                if(g_curr_zoom <= LEVEL_3_ZOOM){
                    if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(4);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});

                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(6);
                            g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(8);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                    }
                 }
                if(g_curr_zoom <= LEVEL_6_ZOOM){
                     if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(7);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                       
                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(9);
                            g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(11);
                        g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
                    }
                 }

                for(int j = 0; j < g_street_segments.at(*it).getNumCurvePoints()-1; ++j){
                    int k = j+1;
                    
                    float x_from_curve_point = x_from_lon(getStreetSegmentCurvePoint(j, *it).lon());
                    float y_from_curve_point = y_from_lat(getStreetSegmentCurvePoint(j, *it).lat());
                    float x_next_curve_point = x_from_lon(getStreetSegmentCurvePoint(k, *it).lon());
                    float y_next_curve_point = y_from_lat(getStreetSegmentCurvePoint(k, *it).lat());
                
                    if(g_curr_zoom <= LEVEL_3_ZOOM){
                        if(g_street_segments.at(*it).getSpeedLimit() < 70){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(4);
                            g.draw_line({x_from_curve_point,y_from_curve_point}, {x_next_curve_point, y_next_curve_point});

                            if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                                g.set_color(ezgl::WHITE);
                                g.set_line_width(6);
                                g.draw_line({x_from_curve_point,y_from_curve_point}, {x_next_curve_point, y_next_curve_point});

                            }
                        }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                            g.set_color(ezgl::YELLOW);
                            g.set_line_width(8);
                            g.draw_line({x_from_curve_point,y_from_curve_point}, {x_next_curve_point, y_next_curve_point});
                        }

                    }
                    if(g_curr_zoom <= LEVEL_6_ZOOM){
                        if(g_street_segments.at(*it).getSpeedLimit() < 70){
                           g.set_color(ezgl::WHITE);
                           g.set_line_width(7);
                           g.draw_line({x_from_curve_point,y_from_curve_point}, {x_next_curve_point, y_next_curve_point});
                           
                           if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                               g.set_color(ezgl::WHITE);
                               g.set_line_width(9);
                               g.draw_line({x_from_curve_point,y_from_curve_point}, {x_next_curve_point, y_next_curve_point});
                               
                           }
                       }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                           g.set_color(ezgl::YELLOW);
                           g.set_line_width(11);
                           g.draw_line({x_from_curve_point,y_from_curve_point}, {x_next_curve_point, y_next_curve_point});
                       }
                    }

                int curve_point_count = g_street_segments.at(*it).getNumCurvePoints()-1;
                
                float x_last_curve_point = x_from_lon(getStreetSegmentCurvePoint(curve_point_count, *it).lon());
                float y_last_curve_point = y_from_lat(getStreetSegmentCurvePoint(curve_point_count, *it).lat());
                float x_to_intersection = x_from_lon(g_street_segments.at(*it).getToIntersection().lon());
                float y_to_intersection = y_from_lat(g_street_segments.at(*it).getToIntersection().lat());
                
                if(g_curr_zoom <= LEVEL_3_ZOOM){
                    if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(4);
                        g.draw_line({x_last_curve_point,y_last_curve_point}, {x_to_intersection, y_to_intersection});

                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(6);
                            g.draw_line({x_last_curve_point,y_last_curve_point}, {x_to_intersection, y_to_intersection});
                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(8);
                        g.draw_line({x_last_curve_point,y_last_curve_point}, {x_to_intersection, y_to_intersection});
                    }
                     
                 }
                if(g_curr_zoom <= LEVEL_6_ZOOM){
                    if(g_street_segments.at(*it).getSpeedLimit() < 70){
                        g.set_color(ezgl::WHITE);
                        g.set_line_width(7);
                        g.draw_line({x_last_curve_point,y_last_curve_point}, {x_to_intersection, y_to_intersection});
                        
                        if((g_street_street_segment_ids[i].size() >= 100) || (g_streets[i].getStreetLength() > 50000)){
                            g.set_color(ezgl::WHITE);
                            g.set_line_width(9);
                            g.draw_line({x_last_curve_point,y_last_curve_point}, {x_to_intersection, y_to_intersection});
                            
                        }
                    }else if(g_street_segments.at(*it).getSpeedLimit() >= 70){
                        g.set_color(ezgl::YELLOW);
                        g.set_line_width(11);
                        g.draw_line({x_last_curve_point,y_last_curve_point}, {x_to_intersection, y_to_intersection});
                    }
                 }
                }
            }
        }
    }
}

/******* display street names on the map *******/
void display_street_names(ezgl::renderer &g){
    
    for(size_t i = 0; i < g_streets.size(); ++i){
        std::string street_name = g_streets[i].getName();
        
        if (street_name.compare("<unknown>") != 0) {
        for(auto it = g_street_street_segment_ids[i].begin(); it != g_street_street_segment_ids[i].end(); ++it){

            if(g_streets[i].getStreetLength() < 30){
            
                float front = g_street_street_segment_ids[i].front();
                float back = g_street_street_segment_ids[i].back();
                
                float front_x = x_from_lon(g_street_segments[front].getFromIntersection().lon());
                float front_y = y_from_lat(g_street_segments[front].getFromIntersection().lat());
                float back_x = x_from_lon(g_street_segments[back].getToIntersection().lon());
                float back_y = y_from_lat(g_street_segments[back].getToIntersection().lat());
                
                float x = (front_x + back_x)/2;
                float y = (front_y + back_y)/2;
                
                float degree = atan((back_y - front_y) / (back_x - front_x)) * 180 / M_PI;

                double x_bound = fabs(back_x - front_x);
                double y_bound = fabs(back_y - front_y);

                g.set_color(ezgl::BLACK);
                g.set_text_rotation(degree);
                
                if(g_curr_zoom <= LEVEL_6_ZOOM){
                    g.set_font_size(8);
                    g.draw_text({x,y}, street_name,x_bound, y_bound);
                }else if(g_curr_zoom <= LEVEL_3_ZOOM){
                    g.set_font_size(6);
                    g.draw_text({x,y}, street_name,x_bound, y_bound);
                }
                
                
                
            }else if(g_street_segments.at(*it).getStreetSegmentLength() >= 30){
                float start_x = x_from_lon(g_street_segments[*it].getFromIntersection().lon());
                float start_y = y_from_lat(g_street_segments[*it].getFromIntersection().lat());
                float end_x = x_from_lon(g_street_segments[*it].getToIntersection().lon());
                float end_y = y_from_lat(g_street_segments[*it].getToIntersection().lat());

                float x = (start_x + end_x)/2;
                float y = (start_y + end_y)/2;

                float degree = atan((end_y - start_y) / (end_x - start_x)) * 180 / M_PI;

                double x_bound = fabs(end_x - start_x);
                double y_bound = fabs(end_y - start_y);

                g.set_color(ezgl::BLACK);
                g.set_text_rotation(degree);
                
                if(g_curr_zoom <= LEVEL_6_ZOOM){
                    g.set_font_size(8);
                    g.draw_text({x,y}, street_name,x_bound, y_bound); 
                }else if(g_curr_zoom <= LEVEL_3_ZOOM){
                    g.set_font_size(6);
                    g.draw_text({x,y}, street_name,x_bound, y_bound); 
                }
                
                
                

            }
        }
        }
    }
}

/******* display one way street on the map *******/
void display_one_way_street(ezgl::renderer &g){
    
    for(unsigned i = 0; i < g_streets.size(); ++i){
        for(auto it = g_street_street_segment_ids[i].begin(); it != g_street_street_segment_ids[i].end(); ++it){
                        
            float x_from_intersection = x_from_lon(g_street_segments.at(*it).getFromIntersection().lon());
            float y_from_intersection = y_from_lat(g_street_segments.at(*it).getFromIntersection().lat());
            float x_to_intersection = x_from_lon(g_street_segments.at(*it).getToIntersection().lon());
            float y_to_intersection = y_from_lat(g_street_segments.at(*it).getToIntersection().lat());
            
            int curve_point_count = g_street_segments.at(*it).getNumCurvePoints();
                
            g.set_color({255,153,51,255});
            g.set_font_size(8);
                        
            //streets without curve points
            if(g_street_segments.at(*it).getNumCurvePoints() == 0){ 
                if(g_street_segments.at(*it).getIsOneWay()){

                    //distance between start and end of segment
                    float length = sqrt(pow((x_to_intersection - x_from_intersection),2) + pow((y_to_intersection - y_from_intersection), 2));

                    float midpoint_x = (x_from_intersection + x_to_intersection)/2;
                    float midpoint_y = (y_from_intersection + y_to_intersection)/2;
                        
                    //only one > on that segment
                    if(length <= 0.002){
                        if(((x_from_intersection <= x_to_intersection) && (y_from_intersection >= y_to_intersection)) ||
                           ((x_from_intersection >= x_to_intersection) && (y_from_intersection >= y_to_intersection))){
                            
                            float degree = atan((y_to_intersection - y_from_intersection) / (x_to_intersection - x_from_intersection)) * 180 / M_PI;
                            g.set_text_rotation(degree);
                            g.draw_text({midpoint_x,midpoint_y}, ">");
                        }else if(((x_from_intersection <= x_to_intersection) && (y_from_intersection <= y_to_intersection)) ||
                           ((x_from_intersection >= x_to_intersection) && (y_from_intersection <= y_to_intersection))){
                            
                            float degree = atan((y_to_intersection - y_from_intersection) / (x_to_intersection - x_from_intersection)) * 180 / M_PI;
                            g.set_text_rotation(degree + 180);
                            g.draw_text({midpoint_x,midpoint_y}, ">");
                        }

                    }else{
                        int num_of_symbol = length / 0.002;

                        for(int symbol = 0; symbol < num_of_symbol; ++symbol){

                            if(((x_from_intersection <= x_to_intersection) && (y_from_intersection >= y_to_intersection)) ||
                                     ((x_from_intersection >= x_to_intersection) && (y_from_intersection >= y_to_intersection))){
                                
                                float angle = atan((y_to_intersection - y_from_intersection) / (x_to_intersection - x_from_intersection)) * 180 / M_PI;
                                float trig_x = 0.002*cos(angle);
                                float trig_y = 0.002*sin(angle);

                                float new_x = x_from_intersection + 0.001 + trig_x*symbol;
                                float new_y = y_from_intersection + 0.001 + trig_y*symbol;

                                g.set_text_rotation(angle);
                                g.draw_text({new_x,new_y}, ">");
                                
                            }else if(((x_from_intersection <= x_to_intersection) && (y_from_intersection <= y_to_intersection)) ||
                               ((x_from_intersection >= x_to_intersection) && (y_from_intersection <= y_to_intersection))){
                                
                                float angle = atan((y_to_intersection - y_from_intersection) / (x_to_intersection - x_from_intersection)) * 180 / M_PI;
                                float trig_x = 0.002*cos(angle + 180);
                                float trig_y = 0.002*sin(angle + 180);

                                float new_x = x_from_intersection + 0.001 + trig_x*symbol;
                                float new_y = y_from_intersection + 0.001 + trig_y*symbol;
                                
                                g.set_text_rotation(angle + 180);
                                g.draw_text({new_x,new_y}, ">");
                            }
                        }
                    }
                }
            }else if(curve_point_count == 1){
                if(g_street_segments.at(*it).getIsOneWay()){

                    float x_first_curve_point = x_from_lon(getStreetSegmentCurvePoint(0, *it).lon());
                    float y_first_curve_point = y_from_lat(getStreetSegmentCurvePoint(0, *it).lat());

                    if(!((x_from_intersection == x_first_curve_point) && (y_from_intersection == y_first_curve_point))){

                        //distance between first curve point and start
                        float length = sqrt(pow((x_first_curve_point - x_from_intersection),2) + pow((y_first_curve_point - y_from_intersection), 2));
                        
                        float midpoint_x = (x_from_intersection + x_first_curve_point)/2;
                        float midpoint_y = (y_from_intersection + y_first_curve_point)/2;
                            
                        if(length <= 0.002){

                            if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection >= y_first_curve_point)) ||
                                     ((x_from_intersection >= x_first_curve_point) && (y_from_intersection >= y_first_curve_point))){
                                
                                float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                g.set_text_rotation(degree);
                                g.draw_text({midpoint_x,midpoint_y}, ">");
                            }else if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection <= y_first_curve_point)) ||
                               ((x_from_intersection >= x_first_curve_point) && (y_from_intersection <= y_first_curve_point))){
                                
                                float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                g.set_text_rotation(180 + degree);
                                g.draw_text({midpoint_x,midpoint_y}, ">");
                            }
                            
                        }else{
                            int num_of_symbol = length / 0.002;

                            for(int symbol = 0; symbol < num_of_symbol; ++symbol){
    
                                if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection >= y_first_curve_point)) ||
                                         ((x_from_intersection >= x_first_curve_point) && (y_from_intersection >= y_first_curve_point))){

                                    float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                    float trig_x = 0.002*cos(degree);
                                    float trig_y = 0.002*sin(degree);
                                    g.set_text_rotation(degree);

                                    float new_x = x_from_intersection + 0.001 + trig_x*symbol;
                                    float new_y = y_from_intersection + 0.001 + trig_y*symbol;
                                    g.draw_text({new_x,new_y}, ">");

                                }else if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection <= y_first_curve_point)) ||
                                         ((x_from_intersection >= x_first_curve_point) && (y_from_intersection <= y_first_curve_point))){
                                    
                                    float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                    float trig_x = 0.002*cos(degree + 180);
                                    float trig_y = 0.002*sin(degree + 180);
                                    g.set_text_rotation(degree + 180);

                                    float new_x = x_from_intersection + 0.001 + trig_x*symbol;
                                    float new_y = y_from_intersection + 0.001 + trig_y*symbol;
                                    g.draw_text({new_x,new_y}, ">");
                                }
                            }

                        }
                    }
                }
            }else{
                if(g_street_segments.at(*it).getIsOneWay()){
                    float x_first_curve_point = x_from_lon(getStreetSegmentCurvePoint(0, *it).lon());
                    float y_first_curve_point = y_from_lat(getStreetSegmentCurvePoint(0, *it).lat());
                    
                    if(!((x_from_intersection == x_first_curve_point) && (y_from_intersection == y_first_curve_point))){
                        
                        float length = sqrt(pow((x_first_curve_point - x_from_intersection),2) + pow((y_first_curve_point - y_from_intersection), 2));

                        float midpoint_x = (x_from_intersection + x_first_curve_point)/2;
                        float midpoint_y = (y_from_intersection + y_first_curve_point)/2;
                        
                        if(length <= 0.002){

                            if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection >= y_first_curve_point)) ||
                               ((x_from_intersection >= x_first_curve_point) && (y_from_intersection >= y_first_curve_point))){

                                float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                g.set_text_rotation(degree);
                                g.draw_text({midpoint_x,midpoint_y}, ">");

                            }else if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection <= y_first_curve_point)) ||
                               ((x_from_intersection >= x_first_curve_point) && (y_from_intersection <= y_first_curve_point))){
                                
                                float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                g.set_text_rotation(degree + 180);
                                g.draw_text({midpoint_x,midpoint_y}, ">");
                            }
  
                        }else{
                            int num_of_symbol = length / 0.002;

                            for(int symbol = 0; symbol < num_of_symbol; ++symbol){
                                if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection >= y_first_curve_point)) ||
                                         ((x_from_intersection >= x_first_curve_point) && (y_from_intersection >= y_first_curve_point))){
                                    
                                    float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                    float trig_x = 0.002*cos(degree);
                                    float trig_y = 0.002*sin(degree);
                                    g.set_text_rotation(degree);

                                    float new_x = x_from_intersection + 0.001 + trig_x*symbol;
                                    float new_y = y_from_intersection + 0.001 + trig_y*symbol;
                                    g.draw_text({new_x,new_y}, ">");

                                }else if(((x_from_intersection <= x_first_curve_point) && (y_from_intersection <= y_first_curve_point)) ||
                                   ((x_from_intersection >= x_first_curve_point) && (y_from_intersection <= y_first_curve_point))){
                                    
                                    float degree = atan((y_first_curve_point - y_from_intersection) / (x_first_curve_point - x_from_intersection)) * 180 / M_PI;
                                    float trig_x = 0.002*cos(degree + 180);
                                    float trig_y = 0.002*sin(degree + 180);
                                    g.set_text_rotation(degree + 180);

                                    float new_x = x_from_intersection + 0.001 + trig_x*symbol;
                                    float new_y = y_from_intersection + 0.001 + trig_y*symbol;
                                    g.draw_text({new_x,new_y}, ">");
                                } 
                            }
                        }
                    }
                    
                    //curved parts
//                    for(int i = 0; i < curve_point_count - 1; ++i){
//                        int j = i + 1;
//                        float x_first_curve_point = x_from_lon(getStreetSegmentCurvePoint(i, *it).lon());
//                        float y_first_curve_point = y_from_lat(getStreetSegmentCurvePoint(i, *it).lat());
//                        float x_next_curve_point = x_from_lon(getStreetSegmentCurvePoint(j, *it).lon());
//                        float y_next_curve_point = y_from_lat(getStreetSegmentCurvePoint(j, *it).lat());
//                        
//                        float degree = atan((y_next_curve_point - y_first_curve_point) / (x_next_curve_point - x_first_curve_point)) * 180 / M_PI;
//                        
//                        g.set_color(ezgl::FIRE_BRICK);
//                        g.set_font_size(8);
//                        g.set_text_rotation(degree);
//
//                        g.draw_text({x_first_curve_point,y_first_curve_point}, ">");
//                        
//                        
//                    }
                    
                    //last curve point to end of intersection
                    float x_last_curve_point = x_from_lon(getStreetSegmentCurvePoint(curve_point_count -1 , *it).lon());
                    float y_last_curve_point = y_from_lat(getStreetSegmentCurvePoint(curve_point_count -1 , *it).lat());
                    
                    if(!((x_last_curve_point == x_to_intersection) && (y_last_curve_point == y_to_intersection))){ //
                        
                        float length = sqrt(pow((x_to_intersection - x_last_curve_point),2) + pow((y_to_intersection - y_last_curve_point), 2));
                        
                        float midpoint_x = (x_last_curve_point + x_to_intersection)/2;
                        float midpoint_y = (y_last_curve_point + y_to_intersection)/2;
                            
                        if(length <= 0.002){

                            if(((x_last_curve_point <= x_to_intersection) && (y_last_curve_point >= y_to_intersection)) ||
                                     ((x_last_curve_point >= x_to_intersection) && (y_last_curve_point >= y_to_intersection))){
                                
                                float degree = atan((y_to_intersection - y_last_curve_point) / (x_to_intersection - x_last_curve_point)) * 180 / M_PI;
                                g.set_text_rotation(degree);
                                g.draw_text({midpoint_x,midpoint_y}, ">");

                            }else if(((x_last_curve_point <= x_to_intersection) && (y_last_curve_point <= y_to_intersection)) ||
                               ((x_last_curve_point >= x_to_intersection) && (y_last_curve_point <= y_to_intersection))){
                                
                                float degree = atan((y_to_intersection - y_last_curve_point) / (x_to_intersection - x_last_curve_point)) * 180 / M_PI;
                                g.set_text_rotation(degree + 180);
                                g.draw_text({midpoint_x,midpoint_y}, ">");
                            }
    
                        }else{
                            int num_of_symbol = length / 0.002;

                            for(int symbol = 0; symbol < num_of_symbol; ++symbol){
                                        
                                if(((x_last_curve_point <= x_to_intersection) && (y_last_curve_point >= y_to_intersection)) ||
                                   ((x_last_curve_point >= x_to_intersection) && (y_last_curve_point >= y_to_intersection))){

                                    float degree = atan((y_to_intersection - y_last_curve_point) / (x_to_intersection - x_last_curve_point)) * 180 / M_PI;
                                    float trig_x = 0.002*cos(degree);
                                    float trig_y = 0.002*sin(degree);
                                    g.set_text_rotation(degree);

                                    float new_x = x_last_curve_point + 0.001 + trig_x*symbol;
                                    float new_y = y_last_curve_point + 0.001 + trig_y*symbol;
                                    g.draw_text({new_x,new_y}, ">");

                                }else if(((x_last_curve_point <= x_to_intersection) && (y_last_curve_point <= y_to_intersection)) ||
                                         ((x_last_curve_point >= x_to_intersection) && (y_last_curve_point <= y_to_intersection))){
                                    
                                    float degree = atan((y_to_intersection - y_last_curve_point) / (x_to_intersection - x_last_curve_point)) * 180 / M_PI;
                                    float trig_x = 0.002*cos(degree + 180);
                                    float trig_y = 0.002*sin(degree + 180);
                                    g.set_text_rotation(degree + 180);

                                    float new_x = x_last_curve_point + 0.001 + trig_x*symbol;
                                    float new_y = y_last_curve_point + 0.001 + trig_y*symbol;
                                    g.draw_text({new_x,new_y}, ">");
                                }
                            }
                        }
                    } 
                } 
            }
        } 
    }
}

/******* Display Point of Interest *******/
void display_point_of_interest_name(ezgl::renderer &g){    
    
    get_curr_zoom(g);

    for(auto it = g_pointOfInterests.begin(); it != g_pointOfInterests.end(); it++){
        
        std::string poi_name = it->second.getName();
        
        float x = x_from_lon(it->second.getPosition().lon());
        float y = y_from_lat(it->second.getPosition().lat());

        float radius = 0.00005;

        if (g_curr_zoom <= LEVEL_9_ZOOM) {
            radius = 0.000005;
        }
        
        if (it->second.highlight)
            g.set_color(ezgl::ORANGE);
        else
            g.set_color({255,255,204,255});

        g.fill_arc({x,y}, radius, 0, 360);
    }
}

/******* Draw Features *******/
void draw_features(ezgl::renderer &g) {
    get_curr_zoom(g);
    draw_features_park(g);
    draw_features_lake(g);    
    draw_features_island(g);
    draw_features_greenspace(g);
    draw_features_building(g);
}

//draw unknown features
void draw_features_unknown(ezgl::renderer &g){
    g.set_line_width(1);
    for (size_t i = 0; i < g_features_unknown.size(); ++i) {
        std::vector<ezgl::point2d> point_xy;
        
        for (size_t j = 0; j < g_features_unknown[i].getPointsLatLon().size(); ++j) {
            double x = x_from_lon(g_features_unknown[i].getPointsLatLon()[j].lon());
            double y = y_from_lat(g_features_unknown[i].getPointsLatLon()[j].lat());
            ezgl::point2d point(x, y);
            point_xy.push_back(point);
        }
        g.set_color(ezgl:: color(255, 255, 204, 100));
        
        if (g_features_unknown[i].getIsClosed() && point_xy.size() > 1)
                g.fill_poly(point_xy);
        else if (point_xy.size() == 1) {
                g.fill_arc(point_xy[0], 0.001, 0, 360);
        }else {
            for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                if (it + 1 != point_xy.end())
                    g.draw_line(*it, *(it+1));
            }

        }
                
    }
}

//draw parks
void draw_features_park(ezgl::renderer &g) {
    get_curr_zoom(g);
    if (g_curr_zoom <= LEVEL_6_ZOOM) {
        g.set_color(ezgl::color (175,239,111,255));
        for (size_t i = 0; i < g_features_park.size(); ++i) {
            std::vector<ezgl::point2d> point_xy;

            for (size_t j = 0; j < g_features_park[i].getPointsLatLon().size(); ++j) {
                double x = x_from_lon(g_features_park[i].getPointsLatLon()[j].lon());
                double y = y_from_lat(g_features_park[i].getPointsLatLon()[j].lat());
                ezgl::point2d point(x, y);
                point_xy.push_back(point);
            }
            if (point_xy.size() >= 1) {
                    if (g_features_park[i].getIsClosed() && point_xy.size() > 1)
                        g.fill_poly(point_xy);
                    else if (point_xy.size() == 1) {
                        g.fill_arc(point_xy[0], 0.001, 0, 360);
                    
//                    else {
//                        for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
//                            if (it + 1 != point_xy.end())
//                                g.draw_line(*it, *(it+1));
//                        }
//
//                    }
                }
            }
        }
    }
             
}

//draw beaches
void draw_features_beach(ezgl::renderer &g) {
        g.set_color(ezgl::BISQUE);
    for (size_t i = 0; i < g_features_beach.size(); ++i) {
        std::vector<ezgl::point2d> point_xy;
        
        for (size_t j = 0; j < g_features_beach[i].getPointsLatLon().size(); ++j) {
            double x = x_from_lon(g_features_beach[i].getPointsLatLon()[j].lon());
            double y = y_from_lat(g_features_beach[i].getPointsLatLon()[j].lat());
            ezgl::point2d point(x, y);
            point_xy.push_back(point);
        }
        if (g_features_beach[i].getIsClosed() && point_xy.size() > 1)
            g.fill_poly(point_xy);
        else if (point_xy.size() == 1) {
            g.fill_arc(point_xy[0], 0.001, 0, 360);
        }
        else {
            for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                if (it + 1 != point_xy.end())
                    g.draw_line(*it, *(it+1));
            }

        }
    }
}

//draw rivers
void draw_features_river(ezgl::renderer &g) {
    g.set_color(ezgl::LIGHT_MEDIUM_BLUE);
    for (size_t i = 0; i < g_features_river.size(); ++i) {
        std::vector<ezgl::point2d> point_xy;
        
        for (size_t j = 0; j < g_features_river[i].getPointsLatLon().size(); ++j) {
            double x = x_from_lon(g_features_river[i].getPointsLatLon()[j].lon());
            double y = y_from_lat(g_features_river[i].getPointsLatLon()[j].lat());
            ezgl::point2d point(x, y);
            point_xy.push_back(point);
        }
        if (g_features_river[i].getIsClosed() && point_xy.size() > 1)
            g.fill_poly(point_xy);
        else if (point_xy.size() == 1) {
            g.fill_arc(point_xy[0], 0.001, 0, 360);
        }
        else {
            for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                if (it + 1 != point_xy.end())
                    g.draw_line(*it, *(it+1));
            }

        }
    }
}

//draw island
void draw_features_island(ezgl::renderer &g){
    get_curr_zoom(g);
    if (g_curr_zoom >= LEVEL_1_ZOOM) {
        g.set_color(ezgl::color (237,227,204,255));
        for (size_t i = 0; i < g_features_island.size(); ++i) {
            std::vector<ezgl::point2d> point_xy;

            for (size_t j = 0; j < g_features_island[i].getPointsLatLon().size(); ++j) {
                double x = x_from_lon(g_features_island[i].getPointsLatLon()[j].lon());
                double y = y_from_lat(g_features_island[i].getPointsLatLon()[j].lat());
                ezgl::point2d point(x, y);
                point_xy.push_back(point);
            }
            if (g_features_island[i].getIsClosed() && point_xy.size() > 1)
                g.fill_poly(point_xy);
            else if (point_xy.size() == 1) {
                g.fill_arc(point_xy[0], 0.001, 0, 360);
            }
            else {
                for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                    if (it + 1 != point_xy.end())
                        g.draw_line(*it, *(it+1));
                }

            }
        }
    }
}

//draw buildings
void draw_features_building(ezgl::renderer &g) {
    get_curr_zoom(g);
    
    if (g_curr_zoom <= LEVEL_6_ZOOM) {
        g.set_color(ezgl::GREY_75);
        for (size_t i = 0; i < g_features_building.size(); ++i) {
            std::vector<ezgl::point2d> point_xy;

            for (size_t j = 0; j < g_features_building[i].getPointsLatLon().size(); ++j) {
                double x = x_from_lon(g_features_building[i].getPointsLatLon()[j].lon());
                double y = y_from_lat(g_features_building[i].getPointsLatLon()[j].lat());
                ezgl::point2d point(x, y);
                point_xy.push_back(point);
            }

            if (point_xy.size() >= 1) {
                    if (g_features_building[i].getIsClosed() && point_xy.size() > 1)
                        g.fill_poly(point_xy);
                    else if (point_xy.size() == 1) {
                        g.fill_arc(point_xy[0], 0.001, 0, 360);
                    }
                    for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                        if (it + 1 != point_xy.end())
                            g.draw_line(*it, *(it+1));
                    }

               
            }
            
        }
    }
}

//draw green space
void draw_features_greenspace(ezgl::renderer &g){
    g.set_color(ezgl::DARK_GREEN);
    for (size_t i = 0; i < g_features_greenspace.size(); ++i) {
        std::vector<ezgl::point2d> point_xy;
        
        for (size_t j = 0; j < g_features_greenspace[i].getPointsLatLon().size(); ++j) {
            double x = x_from_lon(g_features_greenspace[i].getPointsLatLon()[j].lon());
            double y = y_from_lat(g_features_greenspace[i].getPointsLatLon()[j].lat());
            ezgl::point2d point(x, y);
            point_xy.push_back(point);
        }
        if (g_features_greenspace[i].getIsClosed() && point_xy.size() > 1)
            g.fill_poly(point_xy);
        else if (point_xy.size() == 1) {
            g.fill_arc(point_xy[0], 0.001, 0, 360);
        }
        else {
            for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                if (it + 1 != point_xy.end())
                    g.draw_line(*it, *(it+1));
            }

        }
    }
}

//draw golf course
void draw_features_golfcourse(ezgl::renderer &g) {
    g.set_color(ezgl::LIME_GREEN);
    for (size_t i = 0; i < g_features_golfcourse.size(); ++i) {
        std::vector<ezgl::point2d> point_xy;
        
        for (size_t j = 0; j < g_features_golfcourse[i].getPointsLatLon().size(); ++j) {
            double x = x_from_lon(g_features_golfcourse[i].getPointsLatLon()[j].lon());
            double y = y_from_lat(g_features_golfcourse[i].getPointsLatLon()[j].lat());
            ezgl::point2d point(x, y);
            point_xy.push_back(point);
        }
        if (g_features_golfcourse[i].getIsClosed() && point_xy.size() > 1)
            g.fill_poly(point_xy);
        else if (point_xy.size() == 1) {
            g.fill_arc(point_xy[0], 0.001, 0, 360);
        }
        else {
            for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                if (it + 1 != point_xy.end())
                    g.draw_line(*it, *(it+1));
            }

        }
    }
}

//draw streams
void draw_features_stream(ezgl::renderer &g) {
    g.set_color(ezgl::LIGHT_SKY_BLUE);
    for (size_t i = 0; i < g_features_stream.size(); ++i) {
        std::vector<ezgl::point2d> point_xy;
        
        for (size_t j = 0; j < g_features_stream[i].getPointsLatLon().size(); ++j) {
            double x = x_from_lon(g_features_stream[i].getPointsLatLon()[j].lon());
            double y = y_from_lat(g_features_stream[i].getPointsLatLon()[j].lat());
            ezgl::point2d point(x, y);
            point_xy.push_back(point);
        }
        if (g_features_stream[i].getIsClosed() && point_xy.size() > 1)
            g.fill_poly(point_xy);
        else if (point_xy.size() == 1) {
            g.fill_arc(point_xy[0], 0.001, 0, 360);
        }
        else {
            for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                if (it + 1 != point_xy.end())
                    g.draw_line(*it, *(it+1));
            }

        }
    }
}

//draw lakes
void draw_features_lake(ezgl::renderer &g){
    g.set_color(ezgl::color(179,240,255,80));
    for (size_t i = 0; i < g_features_lake.size(); ++i) {
        std::vector<ezgl::point2d> point_xy;
        
        for (size_t j = 0; j < g_features_lake[i].getPointsLatLon().size(); ++j) {
            double x = x_from_lon(g_features_lake[i].getPointsLatLon()[j].lon());
            double y = y_from_lat(g_features_lake[i].getPointsLatLon()[j].lat());
            ezgl::point2d point(x, y);
            point_xy.push_back(point);
        }
        if (g_features_lake[i].getIsClosed() && point_xy.size() > 1)
            g.fill_poly(point_xy);
        else if (point_xy.size() == 1) {
            g.fill_arc(point_xy[0], 0.001, 0, 360);
        }
        else {
            for (auto it = point_xy.begin(); it != point_xy.end(); ++it) {
                if (it + 1 != point_xy.end())
                    g.draw_line(*it, *(it+1));
            }

        }
    }
}

/***** FUNCTIONS TO CONVERT LAT LON TO X Y AND VICE VERSA *****/
void set_lat_avg() {
    g_lat_avg = (g_max_lat + g_min_lat) * DEG_TO_RAD / 2.;
}

double x_from_lon(double lon) {
    double x = lon * cos(g_lat_avg);
    return x;
}
double y_from_lat(double lat) {
    double y = lat;
    return y;
}
double lon_from_x(double x){
    double lon = x / cos(g_lat_avg);
    return lon;
}
double lat_from_y(double y){
    double lat = y;
    return lat;
}

/******* Function to handle mouse press events *******/
void act_on_mouse_press(ezgl::application *application, GdkEventButton *event, double x, double y) {
    std::cout << "Your location " << "x: " << x << "," << "y: " << y << "\n";
    mouse_pressed = true;
    
    if (event->button == 1) { // left mouse click
        count_mouse_clicks++;
                
        //locate the clicked intersection
        LatLon pos = LatLon(lat_from_y(y), lon_from_x(x));
        int closest_intersection_id = find_closest_intersection(pos);
        
        //clicked on intersection
        if (find_distance_between_two_points(pos,g_intersections[closest_intersection_id].getPosition()) < 10){ // not clicked on an intersection
            
            if(reset_clicked){
                count_intersection_click = 0;
                reset_clicked = false;
            }

            count_intersection_click++; //keep track of mouse click counts

            if(!reset_clicked){
                if(count_intersection_click <= 2){
                    error = false;
                    //click on the first intersection
                    if(count_intersection_click == 1){
                        //update location
                        temp_prev_intersection_id = closest_intersection_id; 
                        g_intersections[closest_intersection_id].highlight = true;
                    }
                    //click on the second intersection
                    else if(count_intersection_click == 2){
                        //update location
                        temp_prev_intersection_id2 = closest_intersection_id; 
                        g_intersections[closest_intersection_id].highlight2 = true;
                        
                        /******* draw path between intersections *******/
                        int start_location = temp_prev_intersection_id;
                        int end_location = temp_prev_intersection_id2;
                        
                        std::vector<unsigned> result_path = find_path_between_intersections(start_location, end_location, right_penalty_time, left_penalty_time);
                        double travel_time = compute_path_travel_time(result_path, right_penalty_time, left_penalty_time);

                        
                        if(!result_path.empty()){
                            invalid_path = false;
                        }else{
                            invalid_path = true;
                        }
                        
                        //convert double to char array
                        std::string info_text = std::to_string(travel_time) + " seconds";
                        int length = info_text.length();

                        // convert string to char array
                        char *info_text_char_arr = new char[length + 1];
                        strcpy(info_text_char_arr, info_text.c_str());

                        gtk_text_buffer_set_text(display_travel_time, info_text_char_arr, -1);

                        delete [] info_text_char_arr;
                        
                        //tells draw_main_canvas to draw the path
                        determine_draw_path = true;
                        draw_path_street_segments = result_path;
                        
                        
                        /******* display hint message *******/       
                                                    
                        std::string hint_message = "Travel route is highlighted in blue on the map";
                        int message_length = hint_message.length();

                        // convert string to char array
                        char *hint_messege_char_arr = new char[message_length + 1];
                        strcpy(hint_messege_char_arr, hint_message.c_str());

                        std::string error_message = "Error: Invalid Street Name \n(Please re-enter street names) \n";
                        int error_message_length = error_message.length();

                        // convert string to char array
                        char *error_messege_char_arr = new char[error_message_length + 1];
                        strcpy(error_messege_char_arr, error_message.c_str());

                        
                        
                        if((!error) && (!invalid_path)){
                            gtk_text_buffer_set_text(display_hint_message, hint_messege_char_arr, -1);
                        }else{
                            gtk_text_buffer_set_text(display_hint_message, error_messege_char_arr, -1);
                        }

                        delete [] error_messege_char_arr;
                        delete [] hint_messege_char_arr;
                        


                    }
                    
                }else{
                    g_intersections[closest_intersection_id].highlight = false;
                    g_intersections[closest_intersection_id].highlight2 = false;
                }
                
            }
                        
            /******* Display intersection name in user interface (Information) *******/
            //display intersection 1 name
            if(count_intersection_click == 1){
                std::string info_text = "Intersection 1: " + g_intersections[closest_intersection_id].getName() + "\n";
                int length = info_text.length();

                // convert string to char array
                char *info_text_char_arr = new char[length + 1];
                strcpy(info_text_char_arr, info_text.c_str());

                gtk_text_buffer_set_text(buffer, info_text_char_arr, -1);

                delete [] info_text_char_arr;
            }
            //display intersection 2 name
            else if(count_intersection_click == 2){
                std::string info_text = "Intersection 2: " + g_intersections[closest_intersection_id].getName() + "\n";
                int length = info_text.length();

                // convert string to char array
                char *info_text_char_arr = new char[length + 1];
                strcpy(info_text_char_arr, info_text.c_str());

                gtk_text_buffer_set_text(buffer2, info_text_char_arr, -1);

                delete [] info_text_char_arr;
                
            }
        }
                
        /******* display point of interest name and highlight point of interest *******/
        if (temp_prev_poi != -1)
            g_pointOfInterests[temp_prev_poi].highlight = false;
        
        int closes_poi = find_closest_point_of_interest(pos);
        temp_prev_poi = closes_poi;
        
        if (find_distance_between_two_points(pos,g_pointOfInterests[closes_poi].getPosition()) < 10){

            g_pointOfInterests[closes_poi].highlight = true;
            std::string info_text_2 = "Point of Interest: " + g_pointOfInterests[closes_poi].getName() + "\n";
            int length = info_text_2.length();
            
            // convert string to char array
            char *info_text_2_char_arr = new char[length + 1];
            strcpy(info_text_2_char_arr, info_text_2.c_str());
        
//            gtk_text_buffer_set_text(buffer, info_text_2_char_arr, -1);

            delete [] info_text_2_char_arr;
        }

        application->refresh_drawing();
    }
    else if (event ->button == 2) 
        application->update_message("middle");
    
    else if (event -> button == 3) {         
            user_location_exists = true;
            user_location.x = x;
            user_location.y = y;
            application->refresh_drawing();
    }

}

/******* Callback function of search button *******/
void search_button_callback(GtkWidget *, ezgl::application *application){
    
    //find common intersection 1
    GtkEntry* text_entry = (GtkEntry*) application->get_object("SearchInput");
    GtkEntry* text_entry_two = (GtkEntry*) application->get_object("SearchInputTwo");
    
    const char *text = gtk_entry_get_text(text_entry);
    const char *text_two = gtk_entry_get_text(text_entry_two);
    std::vector<unsigned> streets_with_prefix;
    std::vector<unsigned> streets_with_prefix_two; 
    std::vector<unsigned> intersection_ids;
    
    if (text != NULL && text_two != NULL) {
        streets_with_prefix = find_street_ids_from_partial_street_name (text);
        streets_with_prefix_two = find_street_ids_from_partial_street_name (text_two);
        if (streets_with_prefix.size() != 0 && streets_with_prefix_two.size() != 0)
            intersection_ids = find_intersection_ids_from_street_ids(streets_with_prefix.at(0), 
                streets_with_prefix_two.at(0));
        
        if (intersection_ids.size() > 0) {
            error = false;
            for (auto it = intersection_ids.begin(); it != intersection_ids.end(); ++it) {
                //display common intersection 1 in the text box
                g_intersections[*it].highlight = true;
                common_intersection_id = *it;
                std::string search_output = "Common intersection(s) 1: \n" + g_intersections[*it].getName() + "\n\n";
                int length = search_output.length();
                char* info_text_char_arr = new char[length + 1];
                if (length != 0) {
                    strcpy(info_text_char_arr, search_output.c_str());
                    gtk_text_buffer_set_text(buffer, info_text_char_arr, -1);
                } else {
                    gtk_text_buffer_set_text(buffer, "no common intersections found", -1);
                }
                free(info_text_char_arr);
            }
        }else{ //invalid street name input
            error = true;
            std::string error_message = "";
            int length = error_message.length();
            char* info_text_char_arr = new char[length + 1];
            strcpy(info_text_char_arr, error_message.c_str());
            gtk_text_buffer_set_text(buffer, info_text_char_arr, -1);
            free(info_text_char_arr);
        }
    }
        
    //find intersection 2 
    GtkEntry* text_entry_three = (GtkEntry*) application->get_object("SearchInputThree");
    GtkEntry* text_entry_four = (GtkEntry*) application->get_object("SearchInputFour");
    
    const char *text_three = gtk_entry_get_text(text_entry_three);
    const char *text_four = gtk_entry_get_text(text_entry_four);
    std::vector<unsigned> streets_with_prefix_three;
    std::vector<unsigned> streets_with_prefix_four; 
    std::vector<unsigned> intersection_ids_two;

    if (text_three != NULL && text_four != NULL) {
        streets_with_prefix_three = find_street_ids_from_partial_street_name (text_three);
        streets_with_prefix_four = find_street_ids_from_partial_street_name (text_four);
        if (streets_with_prefix_three.size() != 0 && streets_with_prefix_four.size() != 0)
            intersection_ids_two = find_intersection_ids_from_street_ids(streets_with_prefix_three.at(0), 
                streets_with_prefix_four.at(0));
        
        if (intersection_ids_two.size() > 0) {
            error = false;
            for (auto it = intersection_ids_two.begin(); it != intersection_ids_two.end(); ++it) {
                //display common intersection 2 in the text box
                g_intersections[*it].highlight2 = true;
                common_intersection_id2 = *it;
                std::string search_output = "Common intersection(s) 2: \n" + g_intersections[*it].getName() + "\n\n";
                int length = search_output.length();
                char* info_text_char_arr = new char[length + 1];
                if (length != 0) {
                    strcpy(info_text_char_arr, search_output.c_str());
                    gtk_text_buffer_set_text(buffer2, info_text_char_arr, -1);
                } else {
                    gtk_text_buffer_set_text(buffer2, "no common intersections found", -1);
                }
                free(info_text_char_arr);
            }
        }else{ //invalid intersection name input
            error = true;
            std::string error_message = "";
            int length = error_message.length();
            char* info_text_char_arr = new char[length + 1];
            strcpy(info_text_char_arr, error_message.c_str());
            gtk_text_buffer_set_text(buffer2, info_text_char_arr, -1);
            free(info_text_char_arr);
        }
    }
    

    /***** draw path between common intersection 1 and common intersection 2  *****/

    std::vector<unsigned> result_path = find_path_between_intersections(141, 297, right_penalty_time, left_penalty_time);
    double travel_time = compute_path_travel_time(result_path, right_penalty_time, left_penalty_time);
    
    if(!result_path.empty()){
        invalid_path = false;
    }else{
        invalid_path = true;
    }
    
    
    //convert double to char array
    std::string info_text = std::to_string(travel_time) + " seconds";
    int length = info_text.length();

    std::string empty_text = "";
    int empty_text_length = empty_text.length();
    
    // convert string to char array
    char *info_text_char_arr = new char[length + 1];
    strcpy(info_text_char_arr, info_text.c_str());
    
    char *empty_text_char_arr = new char[empty_text_length + 1];
    strcpy(empty_text_char_arr, empty_text.c_str());
    

    if((!error) && (!invalid_path)){
        gtk_text_buffer_set_text(display_travel_time, info_text_char_arr, -1);
    }else{
        gtk_text_buffer_set_text(display_travel_time, empty_text_char_arr, -1);
    }
  

    delete [] info_text_char_arr;
    delete [] empty_text_char_arr;

    //tells draw_main_canvas to draw the path
    determine_draw_path = true;
    draw_path_street_segments = result_path;


    /******* display hint message *******/
    //hint message
    std::string hint_message = "Travel route is highlighted in blue on the map";
    int message_length = hint_message.length();

    // convert string to char array
    char *hint_messege_char_arr = new char[message_length + 1];
    strcpy(hint_messege_char_arr, hint_message.c_str());


    //error message
    std::string error_message = "Error: Invalid Street Name \n(Please re-enter street names) \n";
    int error_message_length = error_message.length();

    // convert string to char array
    char *error_messege_char_arr = new char[error_message_length + 1];
    strcpy(error_messege_char_arr, error_message.c_str());
    
    if((!error) && (!invalid_path)){
        gtk_text_buffer_set_text(display_hint_message, hint_messege_char_arr, -1);
    }else{
        gtk_text_buffer_set_text(display_hint_message, error_messege_char_arr, -1);
    }
    
    

    delete [] error_messege_char_arr;
    delete [] hint_messege_char_arr;
    
    if (intersection_ids.size() == 1) {
        ezgl::zoom_in(application->get_canvas("MainCanvas"), 
        {x_from_lon(g_intersections[0].getPosition().lon()), y_from_lat(g_intersections[0].getPosition().lat())}, 1);
    }
    
    if (intersection_ids_two.size() == 1) {
        ezgl::zoom_in(application->get_canvas("MainCanvas"), 
        {x_from_lon(g_intersections[0].getPosition().lon()), y_from_lat(g_intersections[0].getPosition().lat())}, 1);
    }

    application->refresh_drawing();
}

/******* Callback function of reset intersection click *******/
void intersection_click_reset_button_callback(GtkWidget *, ezgl::application *application){
    reset_clicked = true;
    g_intersections[temp_prev_intersection_id].highlight = false;
    g_intersections[temp_prev_intersection_id2].highlight2 = false;
    determine_draw_path = false;
    
    std::cout << "Reset Clicked Intersections" << std::endl;
    
    std::string info_text = "\n";
    int length = info_text.length();

    // convert string to char array
    char *info_text_char_arr = new char[length + 1];
    strcpy(info_text_char_arr, info_text.c_str());

    gtk_text_buffer_set_text(buffer, info_text_char_arr, -1);
    gtk_text_buffer_set_text(buffer2, info_text_char_arr, -1);
    gtk_text_buffer_set_text(display_travel_time, info_text_char_arr, -1);
    gtk_text_buffer_set_text(display_hint_message, info_text_char_arr, -1);
    
    delete [] info_text_char_arr;
    
    //hint message
    std::string hint_message = "Please re-enter / click on starting location and destination";
    int hint_message_length = hint_message.length();

    // convert string to char array
    char *hint_message_char_arr = new char[hint_message_length + 1];
    strcpy(hint_message_char_arr, hint_message.c_str());
    gtk_text_buffer_set_text(display_hint_message, hint_message_char_arr, -1);
    
    delete [] hint_message_char_arr;
    
    application->refresh_drawing();
}

/******* Callback function of clear search button *******/
void clear_search_button_callback(GtkWidget *, ezgl::application *application){
    reset_clicked = true;
    g_intersections[common_intersection_id].highlight = false;
    g_intersections[common_intersection_id2].highlight2 = false;
    determine_draw_path = false;
    
    std::cout << "Clear Search" << std::endl;
    
    std::string info_text = "\n";
    int length = info_text.length();

    // convert string to char array
    char *info_text_char_arr = new char[length + 1];
    strcpy(info_text_char_arr, info_text.c_str());

    gtk_text_buffer_set_text(buffer, info_text_char_arr, -1);
    gtk_text_buffer_set_text(buffer2, info_text_char_arr, -1);
    gtk_text_buffer_set_text(display_travel_time, info_text_char_arr, -1);
    
    delete [] info_text_char_arr;
    
    //hint message
    std::string hint_message = "Please re-enter / click on starting location and destination";
    int hint_message_length = hint_message.length();

    // convert string to char array
    char *hint_message_char_arr = new char[hint_message_length + 1];
    strcpy(hint_message_char_arr, hint_message.c_str());
    gtk_text_buffer_set_text(display_hint_message, hint_message_char_arr, -1);
    
    delete [] hint_message_char_arr;
    
    /***** Clear entries *****/
    GtkEntry* text_entry1 = (GtkEntry*) application->get_object("SearchInput");
    GtkEntry* text_entry2 = (GtkEntry*) application->get_object("SearchInputTwo");
    GtkEntry* text_entry3 = (GtkEntry*) application->get_object("SearchInputThree");
    GtkEntry* text_entry4 = (GtkEntry*) application->get_object("SearchInputFour");
    
    std::string clear_text = "";
    int clear_text_length = clear_text.length();

    // convert string to char array
    char *clear_text_char_arr = new char[clear_text_length + 1];
    strcpy(clear_text_char_arr, clear_text.c_str());
    
    gtk_entry_set_text(text_entry1,clear_text_char_arr);
    gtk_entry_set_text(text_entry2,clear_text_char_arr);
    gtk_entry_set_text(text_entry3,clear_text_char_arr);
    gtk_entry_set_text(text_entry4,clear_text_char_arr);
    
    delete [] clear_text_char_arr;

    application->refresh_drawing();
}

/******* Callback function of custom penalty time button *******/
void enter_time_button_callback(GtkWidget *, ezgl::application *application){
    
    std::cout << "entered right turn penalty time: " << right_penalty_time << std::endl;
    
    GtkEntry* text_entry = (GtkEntry*) application->get_object("RightTurnPenalty");
    GtkEntry* text_entry_two = (GtkEntry*) application->get_object("LeftTurnPenalty");
    
    double text = atof(gtk_entry_get_text(text_entry));
    double text_two = atof(gtk_entry_get_text(text_entry_two));
    
    //if user didn't enter penalty time
    if(text == 0 || text_two == 0){
        std::string error_message = "Warning: Entered penalty time may be 0 or empty. If the penalty time desired is not 0 or empty, please re-enter.";
        int error_message_length = error_message.length();

        // convert string to char array
        char *error_message_char_arr = new char[error_message_length + 1];
        strcpy(error_message_char_arr, error_message.c_str());
        gtk_text_buffer_set_text(display_hint_message, error_message_char_arr, -1);

        delete [] error_message_char_arr;
        
    }else{
        enter_time_error = false;
        std::string error_message = "Entered Penalty Time";
        int error_message_length = error_message.length();

        // convert string to char array
        char *error_message_char_arr = new char[error_message_length + 1];
        strcpy(error_message_char_arr, error_message.c_str());
        gtk_text_buffer_set_text(display_hint_message, error_message_char_arr, -1);

        delete [] error_message_char_arr;
    }
    
    
//    //hint message
//    std::string hint_message = "Entered Custom Penalty Time";
//    int hint_message_length = hint_message.length();
//
//    // convert string to char array
//    char *hint_message_char_arr = new char[hint_message_length + 1];
//    strcpy(hint_message_char_arr, hint_message.c_str());
//    gtk_text_buffer_set_text(display_hint_message, hint_message_char_arr, -1);
//    
//    delete [] hint_message_char_arr;
    
    
}

/******* Observe text entry *******/
//street 1
void preedit_changed(GtkWidget * , ezgl::application *application) {
    
    GtkEntryCompletion *entry_completion = gtk_entry_completion_new();
    GtkListStore *store;
    GtkTreeIter iter;

    GtkEntry* text_entry = (GtkEntry*) application->get_object("SearchInput");
    
    store = gtk_list_store_new(1, G_TYPE_STRING);
    const char *text = gtk_entry_get_text(text_entry);
    std::vector<unsigned> streets_with_prefix = find_street_ids_from_partial_street_name (text);
    for (auto it = streets_with_prefix.begin(); it != streets_with_prefix.end(); it ++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, g_streets.at(*it).getName(), -1);
    }

    // set gtk_list_store to gtk_entry_completion
    gtk_entry_completion_set_model(entry_completion, GTK_TREE_MODEL(store));
    gtk_entry_completion_set_text_column(entry_completion, 0);
    
    gtk_entry_set_completion(text_entry, entry_completion);
    application->update_message(text);
    application->refresh_drawing();
}

//street 2
void preedit_changed_two(GtkWidget *, ezgl::application *application) {
    
    GtkEntryCompletion *entry_completion = gtk_entry_completion_new();
    GtkListStore *store;
    GtkTreeIter iter;
 
    GtkEntry* text_entry = (GtkEntry*) application->get_object("SearchInputTwo");
    store = gtk_list_store_new(1, G_TYPE_STRING);
    const char *text = gtk_entry_get_text(text_entry);
    
    std::vector<unsigned> streets_with_prefix = find_street_ids_from_partial_street_name (text);
 
    for (auto it = streets_with_prefix.begin(); it != streets_with_prefix.end(); it ++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, g_streets.at(*it).getName(), -1);
    }

    // set gtk_list_store to gtk_entry_completion
    gtk_entry_completion_set_model(entry_completion, GTK_TREE_MODEL(store));
    gtk_entry_completion_set_text_column(entry_completion, 0);

    gtk_entry_set_completion(text_entry, entry_completion);
    application->update_message(text);
    application->refresh_drawing();
}

//street 3
void preedit_changed_three(GtkWidget *, ezgl::application *application) {
    
    GtkEntryCompletion *entry_completion = gtk_entry_completion_new();
    GtkListStore *store;
    GtkTreeIter iter;
 
    GtkEntry* text_entry = (GtkEntry*) application->get_object("SearchInputThree");
    store = gtk_list_store_new(1, G_TYPE_STRING);
    const char *text = gtk_entry_get_text(text_entry);
    
    std::vector<unsigned> streets_with_prefix = find_street_ids_from_partial_street_name (text);
 
    for (auto it = streets_with_prefix.begin(); it != streets_with_prefix.end(); it ++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, g_streets.at(*it).getName(), -1);
    }

    // set gtk_list_store to gtk_entry_completion
    gtk_entry_completion_set_model(entry_completion, GTK_TREE_MODEL(store));
    gtk_entry_completion_set_text_column(entry_completion, 0);

    gtk_entry_set_completion(text_entry, entry_completion);
    application->update_message(text);
    application->refresh_drawing();
}

//street 4
void preedit_changed_four(GtkWidget *, ezgl::application *application) {
    
    GtkEntryCompletion *entry_completion = gtk_entry_completion_new();
    GtkListStore *store;
    GtkTreeIter iter;
 
    GtkEntry* text_entry = (GtkEntry*) application->get_object("SearchInputFour");
    store = gtk_list_store_new(1, G_TYPE_STRING);
    const char *text = gtk_entry_get_text(text_entry);
    
    std::vector<unsigned> streets_with_prefix = find_street_ids_from_partial_street_name (text);
 
    for (auto it = streets_with_prefix.begin(); it != streets_with_prefix.end(); it ++) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, g_streets.at(*it).getName(), -1);
    }

    // set gtk_list_store to gtk_entry_completion
    gtk_entry_completion_set_model(entry_completion, GTK_TREE_MODEL(store));
    gtk_entry_completion_set_text_column(entry_completion, 0);

    gtk_entry_set_completion(text_entry, entry_completion);
    application->update_message(text);
    application->refresh_drawing();
}

//right turn penalty
void preedit_changed_right(GtkWidget *, ezgl::application *application) {
    
    GtkEntry* text_entry = (GtkEntry*) application->get_object("RightTurnPenalty");
    double time = atof(gtk_entry_get_text(text_entry));
    right_penalty_time = time;

    application->refresh_drawing();

}

//left turn penalty
void preedit_changed_left(GtkWidget *, ezgl::application *application) {
    
    GtkEntry* text_entry = (GtkEntry*) application->get_object("LeftTurnPenalty");
    double time = atof(gtk_entry_get_text(text_entry));
    left_penalty_time = time;

    application->refresh_drawing();

}

/******* update points *******/
void update_point(ezgl::renderer &g) {
    
    g.set_color(ezgl::color(204, 153, 255, 150));
    if (g_curr_zoom <= LEVEL_8_ZOOM)
        g.fill_arc(user_location, 0.0001, 0, 360);   
    else if (g_curr_zoom > LEVEL_8_ZOOM && g_curr_zoom <= LEVEL_7_ZOOM)
        g.fill_arc(user_location, 0.0004, 0, 360);
    else if (g_curr_zoom > LEVEL_7_ZOOM && g_curr_zoom <= LEVEL_6_ZOOM)
        g.fill_arc(user_location, 0.0007, 0, 360);
    else if (g_curr_zoom > LEVEL_6_ZOOM && g_curr_zoom <= LEVEL_5_ZOOM)
        g.fill_arc(user_location, 0.001, 0, 360);
    else if (g_curr_zoom > LEVEL_5_ZOOM && g_curr_zoom <= LEVEL_4_ZOOM)
        g.fill_arc(user_location, 0.0013, 0, 360);
    else if (g_curr_zoom > LEVEL_4_ZOOM && g_curr_zoom <= LEVEL_3_ZOOM)
        g.fill_arc(user_location, 0.0016, 0, 360);
    else if (g_curr_zoom > LEVEL_3_ZOOM && g_curr_zoom <= LEVEL_2_ZOOM)
        g.fill_arc(user_location, 0.0019, 0, 360);
    else if (g_curr_zoom > LEVEL_2_ZOOM && g_curr_zoom <= LEVEL_1_ZOOM)
        g.fill_arc(user_location, 0.004, 0, 360);
    else
        g.fill_arc(user_location, 0.008, 0, 360);
    
    g.set_color(ezgl::BLACK);
    g.draw_text(user_location, "Your Location");
   
   
}

/******* Toggles on user interface *******/
//restaurants
void toggle_restaurants(GtkToggleButton *toggle_button, ezgl::application *application){
    if (gtk_toggle_button_get_active(toggle_button))
        display_restaurants = true;
    else
        display_restaurants = false;  
    application->refresh_drawing();
}

//clinics
void toggle_clinics(GtkToggleButton *toggle_button, ezgl::application *application) {
    if (gtk_toggle_button_get_active(toggle_button))
        display_clinics = true;
    else
        display_clinics = false;   
    application->refresh_drawing();
}

//super markets
void toggle_supermarkets(GtkToggleButton *toggle_button, ezgl::application *application) {
    if (gtk_toggle_button_get_active(toggle_button))
        display_supermarkets = true;
    else
        display_supermarkets = false;   
    application->refresh_drawing();
}

//TTC stations
void toggle_ttc(GtkToggleButton *toggle_button, ezgl::application *application) {
    if (gtk_toggle_button_get_active(toggle_button))
        display_ttc = true;
    else
        display_ttc = false;   
    application->refresh_drawing();
}

////default penalty time
//void toggle_default(GtkToggleButton *toggle_button, ezgl::application *application) {
//    
//    GtkEntry* text_entry_right = (GtkEntry*) application->get_object("RightTurnPenalty");
//    GtkEntry* text_entry_left = (GtkEntry*) application->get_object("LeftTurnPenalty");
//    
//    if (gtk_toggle_button_get_active(toggle_button)){
//        default_penalty_time = true;
//    
//        std::string right = std::to_string(g_right_turn_penalty);
//        int right_length = right.length();
//
//        // convert string to char array
//        char *right_char_arr = new char[right_length + 1];
//        strcpy(right_char_arr, right.c_str());
//
//        std::string left = std::to_string(g_left_turn_penalty);
//        int left_length = left.length();
//
//        // convert string to char array
//        char *left_char_arr = new char[left_length + 1];
//        strcpy(left_char_arr, left.c_str());
//
//        gtk_entry_set_text(text_entry_right,right_char_arr);
//        gtk_entry_set_text(text_entry_left,left_char_arr);
//
//        delete [] right_char_arr;
//        delete [] left_char_arr;
//    }else{
//        default_penalty_time = false;
//        
//        std::string info_text = "";
//        int length = info_text.length();
//
//        // convert string to char array
//        char *info_text_char_arr = new char[length + 1];
//        strcpy(info_text_char_arr, info_text.c_str());
//
//        gtk_entry_set_text(text_entry_right,info_text_char_arr);
//        gtk_entry_set_text(text_entry_left,info_text_char_arr);
//
//        delete [] info_text_char_arr;
//    }
//           
//
//    application->refresh_drawing();
//}
//
////custom penalty time
//void toggle_custom(GtkToggleButton *toggle_button, ezgl::application *application) {
//    GtkEntry* text_entry_right = (GtkEntry*) application->get_object("RightTurnPenalty");
//    GtkEntry* text_entry_left = (GtkEntry*) application->get_object("LeftTurnPenalty");
//    
//    if (gtk_toggle_button_get_active(toggle_button)){
//        custom_penalty_time = true;
//
//    }else{
//        custom_penalty_time = false;
//    }
//    
//    std::string info_text = "";
//    int length = info_text.length();
//
//    // convert string to char array
//    char *info_text_char_arr = new char[length + 1];
//    strcpy(info_text_char_arr, info_text.c_str());
//
//    gtk_entry_set_text(text_entry_right,info_text_char_arr);
//    gtk_entry_set_text(text_entry_left,info_text_char_arr);
//
//    delete [] info_text_char_arr;
//           
//    application->refresh_drawing();
//}

//point of interest
void toggle_poi(GtkToggleButton *toggle_button, ezgl::application *application) {
    if (gtk_toggle_button_get_active(toggle_button)){
        display_poi = true;

    }else{
        display_poi = false;
    }    
    application->refresh_drawing();

}

/***** Radio Buttons *****/
//default radio button
void radio_default(GtkToggleButton *toggle_button, ezgl::application *application){
    
    GtkEntry* text_entry_right = (GtkEntry*) application->get_object("RightTurnPenalty");
    GtkEntry* text_entry_left = (GtkEntry*) application->get_object("LeftTurnPenalty");
    
    if (gtk_toggle_button_get_active(toggle_button)){
        default_penalty = true;
        
        std::string right = std::to_string(g_right_turn_penalty);
        int right_length = right.length();

        // convert string to char array
        char *right_char_arr = new char[right_length + 1];
        strcpy(right_char_arr, right.c_str());

        std::string left = std::to_string(g_left_turn_penalty);
        int left_length = left.length();

        // convert string to char array
        char *left_char_arr = new char[left_length + 1];
        strcpy(left_char_arr, left.c_str());

        gtk_entry_set_text(text_entry_right,right_char_arr);
        gtk_entry_set_text(text_entry_left,left_char_arr);

        delete [] right_char_arr;
        delete [] left_char_arr;
    }else{
        display_restaurants = false; 
        
        std::string info_text = "";
        int length = info_text.length();

        // convert string to char array
        char *info_text_char_arr = new char[length + 1];
        strcpy(info_text_char_arr, info_text.c_str());

        gtk_entry_set_text(text_entry_right,info_text_char_arr);
        gtk_entry_set_text(text_entry_left,info_text_char_arr);

        delete [] info_text_char_arr;
    }
         
    application->refresh_drawing();
}

//custom radio button
void radio_custom(GtkToggleButton *toggle_button, ezgl::application *application){
    
    GtkEntry* text_entry_right = (GtkEntry*) application->get_object("RightTurnPenalty");
    GtkEntry* text_entry_left = (GtkEntry*) application->get_object("LeftTurnPenalty");
    
    if (gtk_toggle_button_get_active(toggle_button)){
        custom_penalty = true;
    }else{
        custom_penalty = false;  
    }
    
    std::string info_text = "";
    int length = info_text.length();

    // convert string to char array
    char *info_text_char_arr = new char[length + 1];
    strcpy(info_text_char_arr, info_text.c_str());

    gtk_entry_set_text(text_entry_right,info_text_char_arr);
    gtk_entry_set_text(text_entry_left,info_text_char_arr);

    delete [] info_text_char_arr;
        
    application->refresh_drawing();
}

/******* zoom fit *******/
void press_zoom_fit_register_new_callback(GtkWidget *, ezgl::application *application) {
    ezgl::rectangle new_world({x_from_lon(g_min_lon), y_from_lat(g_min_lat)},
        {x_from_lon(g_max_lon), y_from_lat(g_max_lat)} );
        
    ezgl::zoom_fit(application->get_canvas("MainCanvas"), 
           new_world);
}

/******* Combo box for choose city name *******/
void combobox_onchanged(GtkComboBox *widget, ezgl::application* application) {
    GtkComboBox *combo_box = widget;
    
    if (gtk_combo_box_get_active(combo_box)!=0) {
    
        gchar *map_location = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
        update_city_name(map_location);
        load_new_map(map_location, application);
        
    }
}

/******* Update city name of the map *******/
void update_city_name(gchar* new_city_name) {
    curr_city_name = new_city_name;
}

/******* Load new map *******/
void load_new_map(std::string map_name, ezgl::application* application){
 
    closeStreetDatabase();
    closeOSMDatabase();
    
    /******* text box *******/
    gtk_text_buffer_set_text(buffer, " ", -1);
    gtk_text_buffer_set_text(buffer2, " ", -1);
    gtk_text_buffer_set_text(display_hint_message, " ", -1);
    gtk_text_buffer_set_text(display_travel_time, " ", -1);
        
    clear_to_reload_map();
    
    load_map("/cad2/ece297s/public/maps/" + map_name + ".streets.bin");
    
    g_max_lat = g_intersections[0].getPosition().lat();
    g_min_lat = g_max_lat;
    g_max_lon = g_intersections[0].getPosition().lon();
    g_min_lon = g_max_lon;
    set_lat_avg();
    
    for (size_t i = 0; i < g_intersections.size(); ++i) {
        g_max_lat = std::max(g_max_lat, g_intersections[i].getPosition().lat());
        g_min_lat = std::min(g_min_lat, g_intersections[i].getPosition().lat());
        g_max_lon = std::max(g_max_lon, g_intersections[i].getPosition().lon());
        g_min_lon = std::min(g_min_lon, g_intersections[i].getPosition().lon());
    }
    user_location_exists = false;
    
    ezgl::rectangle new_world({x_from_lon(g_min_lon), y_from_lat(g_min_lat)},
        {x_from_lon(g_max_lon), y_from_lat(g_max_lat)} );
        
    g_initial_area = new_world.area();
    application->get_canvas("MainCanvas")->get_camera().set_world(new_world);
    ezgl::zoom_fit(application->get_canvas("MainCanvas"), new_world);
    register_new_call_back(application);
    
    application->update_message("loaded new map");
    application->refresh_drawing();
        
}

void register_new_call_back(ezgl::application *application) {
    GObject *zoom_fit_button = application->get_object("ZoomFitButton");
    g_signal_connect(zoom_fit_button, "clicked", G_CALLBACK(press_zoom_fit_register_new_callback), application);
}

/******* Clear global variables to re-load map *******/
void clear_to_reload_map() {
    if (!g_intersection_street_segments.empty())
        g_intersection_street_segments.erase(g_intersection_street_segments.begin(), 
            g_intersection_street_segments.end());
    
    if (!g_intersection_street_name.empty())
        g_intersection_street_name.erase(g_intersection_street_name.begin(), g_intersection_street_name.end());
    
    if (!g_street_segments.empty())
        g_street_segments.erase(g_street_segments.begin(), g_street_segments.end());  
    
    if (!g_street_segment_travel_time.empty())
        g_street_segment_travel_time.erase(g_street_segment_travel_time.begin(), g_street_segment_travel_time.end());
    
    if (!g_streets.empty())
        g_streets.erase(g_streets.begin(), g_streets.end()); 
    
    if (!g_street_street_segment_ids.empty())
        g_street_street_segment_ids.erase(g_street_street_segment_ids.begin(), g_street_street_segment_ids.end());
    
    if (!g_street_intersection_ids.empty())
        g_street_intersection_ids.erase(g_street_intersection_ids.begin(), g_street_intersection_ids.end());
    
    if (!g_streets_by_name.empty())
        g_streets_by_name.erase(g_streets_by_name.begin(), g_streets_by_name.end()); 
    
    if (!g_intersections.empty())
        g_intersections.erase(g_intersections.begin(), g_intersections.end());
    
    if (!g_pointOfInterests.empty())
        g_pointOfInterests.erase(g_pointOfInterests.begin(), g_pointOfInterests.end()); 

    if (!g_features_park.empty())
        g_features_park.erase(g_features_park.begin(), g_features_park.end()); 
    
    if (!g_features_beach.empty())
        g_features_beach.erase(g_features_beach.begin(), g_features_beach.end()); 
    
    if (!g_features_lake.empty())
        g_features_lake.erase(g_features_lake.begin(), g_features_lake.end()); 
    
    if (!g_features_river.empty())
        g_features_river.erase(g_features_river.begin(), g_features_river.end()); 
    
    if (!g_features_island.empty())
        g_features_island.erase(g_features_island.begin(), g_features_island.end()); 
    
    if (!g_features_building.empty())
        g_features_building.erase(g_features_building.begin(), g_features_building.end());
    
    if (!g_features_greenspace.empty())
        g_features_greenspace.erase(g_features_greenspace.begin(), g_features_greenspace.end());
    
    if (!g_features_golfcourse.empty())
        g_features_golfcourse.erase(g_features_golfcourse.begin(), g_features_golfcourse.end());
    
    if (!g_features_stream.empty())
        g_features_stream.erase(g_features_stream.begin(), g_features_stream.end());
    
    if (!g_features_unknown.empty())
        g_features_unknown.erase(g_features_unknown.begin(), g_features_unknown.end());
    
    if (!restaurants.empty())
        restaurants.erase(restaurants.begin(), restaurants.end());
    if (!clinics.empty())
        clinics.erase(clinics.begin(), clinics.end());

}

/******* Updated TTC information *******/
void get_ttc_info(){
    
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK) {
        std::cout << "ERROR: Unable to initialize libcurl" << std::endl;
        std::cout << curl_easy_strerror(res) << std::endl;
        return;
    }
    CURL *curlHandle = curl_easy_init();
    if ( !curlHandle ) {
        std::cout << "ERROR: Unable to get easy handle" << std::endl;
        return;
    } 
    else {
        // Set web URL to fetch page/data from
        //res = curl_easy_setopt(curlHandle, CURLOPT_URL, url);
        res = curl_easy_setopt(curlHandle, CURLOPT_URL, "http://portal.cvst.ca/api/0.1/ttc/geojson");
        if (res != CURLE_OK) {
            std::cout << "ERROR: Unable to set libcurl option" << std::endl;
            std::cout << curl_easy_strerror(res) << std::endl;
        } 
        else {
            // Perform web transfer request
            res = curl_easy_perform(curlHandle);
        }   
        std::cout << std::endl << std::endl;
        if (res == CURLE_OK) {
            std::cout << "All good! res == CURLE_OK!" << std::endl;
        } 
        else {
            std::cout << "ERROR: res == " << res << std::endl;
            std::cout << curl_easy_strerror(res) << std::endl;
            std::cout << "See https://curl.haxx.se/libcurl/c/libcurl-errors.html "<<"for error codes" << std::endl;
        }
        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }
    curl_global_cleanup();
}

/******* Draw routes that user want *******/
void draw_path(ezgl::renderer &g, std::vector<unsigned> street_segment_ids){    
    get_curr_zoom(g);
    g.set_color(ezgl::BLUE);
    g.set_line_width(4);

    for(auto it = street_segment_ids.begin(); it != street_segment_ids.end(); ++it){
        if(g_street_segments[*it].getNumCurvePoints() == 0){
            float x_from_intersection = x_from_lon(g_street_segments[*it].getFromIntersection().lon());
            float y_from_intersection = y_from_lat(g_street_segments[*it].getFromIntersection().lat());
            float x_to_intersection = x_from_lon(g_street_segments[*it].getToIntersection().lon());
            float y_to_intersection = y_from_lat(g_street_segments[*it].getToIntersection().lat());
            
            g.draw_line({x_from_intersection,y_from_intersection}, {x_to_intersection, y_to_intersection});
            
        }else if(g_street_segments[*it].getNumCurvePoints() == 1){

            float x_from_intersection = x_from_lon(g_street_segments[*it].getFromIntersection().lon());
            float y_from_intersection = y_from_lat(g_street_segments[*it].getFromIntersection().lat());
            float x_first_curve_point = x_from_lon(getStreetSegmentCurvePoint(0, *it).lon());
            float y_first_curve_point = y_from_lat(getStreetSegmentCurvePoint(0, *it).lat());
            float x_to_intersection = x_from_lon(g_street_segments[*it].getToIntersection().lon());
            float y_to_intersection = y_from_lat(g_street_segments[*it].getToIntersection().lat());

            g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
            g.draw_line({x_first_curve_point,y_first_curve_point}, {x_to_intersection, y_to_intersection});

        }else{
            float x_from_intersection = x_from_lon(g_street_segments[*it].getFromIntersection().lon());
            float y_from_intersection = y_from_lat(g_street_segments[*it].getFromIntersection().lat());
            float x_first_curve_point = x_from_lon(getStreetSegmentCurvePoint(0, *it).lon());
            float y_first_curve_point = y_from_lat(getStreetSegmentCurvePoint(0, *it).lat());

            g.draw_line({x_from_intersection,y_from_intersection}, {x_first_curve_point, y_first_curve_point});
            
            for(int j = 0; j < g_street_segments[*it].getNumCurvePoints()-1; ++j){
                int k = j+1;

                float x_from_curve_point = x_from_lon(getStreetSegmentCurvePoint(j, *it).lon());
                float y_from_curve_point = y_from_lat(getStreetSegmentCurvePoint(j, *it).lat());
                float x_next_curve_point = x_from_lon(getStreetSegmentCurvePoint(k, *it).lon());
                float y_next_curve_point = y_from_lat(getStreetSegmentCurvePoint(k, *it).lat());

                g.draw_line({x_from_curve_point,y_from_curve_point}, {x_next_curve_point, y_next_curve_point});

                int curve_point_count = g_street_segments[*it].getNumCurvePoints()-1;

                float x_last_curve_point = x_from_lon(getStreetSegmentCurvePoint(curve_point_count, *it).lon());
                float y_last_curve_point = y_from_lat(getStreetSegmentCurvePoint(curve_point_count, *it).lat());
                float x_to_intersection = x_from_lon(g_street_segments[*it].getToIntersection().lon());
                float y_to_intersection = y_from_lat(g_street_segments[*it].getToIntersection().lat());

                g.draw_line({x_last_curve_point,y_last_curve_point}, {x_to_intersection, y_to_intersection});
            
            }
        }
    }
}

//call back function for the pop up window
void on_dialog_response(GtkDialog *dialog, gint , gpointer ){
    gtk_widget_destroy(GTK_WIDGET (dialog));
}

/******* Callback function for help button *******/
void draw_information_box(GtkWidget *, ezgl::application *application){
    application->refresh_drawing();
    
    GObject *window; 
    GtkWidget *content_area;    
    GtkWidget *text;         
    GtkWidget *dialog;          
    window = application->get_object(application->get_main_window_id().c_str());
    
    dialog = gtk_dialog_new_with_buttons("About", (GtkWindow*)window, GTK_DIALOG_MODAL, ("OK"), GTK_RESPONSE_REJECT, NULL);
    
    // Create a text and attach it to the content area of the dialog
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    text = gtk_label_new("****************SEARCH PATH****************\n\n1. By Entering Street Names\nEnter street names"
            " at the top left corner of the window to find the corresponding intersections,\n"
            "intersection 1 is the starting point and intersection 2 is the destination point. Once done, click\nSearch Path."
            " Results will be displayed at the right bottom corner under \"Travel Direction\"\n\n2. By Clicking on the Map \n"
            "Zoom in until you are able to see the intersection points. Click on the starting point intersection,\n (it will be highlighted in red color),"
            " then click on the destination intersection(it will be highlighted\nin green color). Results will be displayed at the right bottom corner under "
            "\"Travel Direction\".\nClick on the \"Reset Intersections\" button to reset.\n\n\n**********SEARCH POINT OF INTERESTS**********\n\n1. By Using Checklist\n"
            "Use the checklist on the right side of the window, click on the toggle bottons and the corresponding\nPOI icons will be displayed on the map.\n\n2.By Clicking on the Map\n"
            "Zoom in to the area of interest. The yellow color dots on the map represent the points of interest.\nClick on it, then its information will be displayed"
            "at the right side of the window under the label\n\"Information\"");
    gtk_container_add(GTK_CONTAINER(content_area), text);
    gtk_widget_show_all(dialog);
    
    // Connecting the signal from the user to the associated callback function
    g_signal_connect(GTK_DIALOG(dialog),"response",G_CALLBACK(on_dialog_response),NULL);
}
