#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
    long a = LinuxParser::ActiveJiffies();
    long b = LinuxParser::IdleJiffies();
    long c = a + b; //total jiffies
    float systemCpuUtilization = (float) a/c;  //Active Jiffies / Total Jiffies
    return systemCpuUtilization;
 }