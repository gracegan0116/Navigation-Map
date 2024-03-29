/*
 * Copyright 2019 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated
 * documentation files (the "Software") in course work at the University
 * of Toronto, or for personal use. Other uses are prohibited, in
 * particular the distribution of the Software either publicly or to third
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <string>
#include <algorithm> 
#include <unordered_map>
#include <iomanip>
#include "MapData.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m3_helper.h"
#include "m4.h"
#include <gtk/gtk.h>
#include "StreetsDatabaseAPI.h"
#include "courier_verify.h"

void print(std::vector<std::string> const &input);

//Program exit codes
constexpr int SUCCESS_EXIT_CODE = 0;        //Everyting went OK
constexpr int ERROR_EXIT_CODE = 1;          //An error occured
constexpr int BAD_ARGUMENTS_EXIT_CODE = 2;  //Invalid command-line usage

//The default map to load if none is specified
std::string default_map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";

int main(int argc, char** argv) {
    std::string map_path;
   
    if(argc == 1) {
        //Use a default map
        map_path = default_map_path;
    } else if (argc == 2) {
        //Get the map from the command line
        map_path = argv[1];
    } else {
        //Invalid arguments
        std::cerr << "Usage: " << argv[0] << " [map_file_path]\n";
        std::cerr << "  If no map_file_path is provided a default map is loaded.\n";
        return BAD_ARGUMENTS_EXIT_CODE;
    }

    //Load the map and related data structures
    bool load_success = load_map(default_map_path);
   
    if(!load_success) {
        std::cerr << "Failed to load map '" << default_map_path << "'\n";
        return ERROR_EXIT_CODE;
    } else {
        std::cout << "Successfully loaded map '" << default_map_path << "'\n";
        //draw_map();
    }
   

    //Clean-up the map data and related data structures
    std::cout << "Closing map\n";

    close_map();

    return SUCCESS_EXIT_CODE;
}


void print(std::vector<std::string> const &input)
{
    for (unsigned i = 0; i < input.size(); i++) {
        std::cout << input.at(i) << std::endl;
    }
}

