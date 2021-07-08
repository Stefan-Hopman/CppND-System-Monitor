#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    //Active Jiffies / Total Jiffies
    long a = LinuxParser::ActiveJiffies();
    long b = LinuxParser::IdleJiffies();
    long c = a + b;
    float totalUtilization = (float) a/c;
    return totalUtilization;
 }