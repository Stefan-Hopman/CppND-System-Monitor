#include <string>
#include <iomanip>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = seconds / 3600;
    int minutes = (seconds / 60) % 60;
    int secondsInt = seconds % 60; 

    std::ostringstream output;
    output << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2) << std::setfill('0') << minutes << std::setw(2) << std::setfill('0') << secondsInt;
    return output.str();
 }
