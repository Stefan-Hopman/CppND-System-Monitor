#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Done: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key;
  float free_memory, total_memory, buffers, memory_utilization, value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
        if(key == "MemTotal"){
          total_memory = value;
        }
        else if(key ==  "Memfree"){
          free_memory = value;
        }
        else if(key == "Buffers")
          buffers = value;
    }
  }
  memory_utilization = 1.0 - (free_memory/(total_memory - buffers));
  return memory_utilization;
  return 0;
 }

// Done: Read and return the system uptime
long LinuxParser::UpTime() {
  long UPTime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> UPTime;
    }
  return UPTime;
 }

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long Jiffies{0};
  Jiffies = UpTime() * (sysconf(_SC_CLK_TCK));
  return Jiffies; }

// DONE: Read and return the number of active jiffies for a PID
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
long LinuxParser::ActiveJiffies(int pid) {
  long tTime, eTime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  string line, placeholder, utime, stime, cutime, cstime, junk, startTime;
  if(filestream.is_open()){
    getline(filestream, line);
    std::istringstream linestream(line);
    for(int i = 0; i < 13; i++){
      linestream >> placeholder;
    }
    linestream >> utime >> stime >> cutime >> cstime >> junk >> junk >> junk >> junk >> startTime;
  }
  tTime = std::stol(utime) + std::stol(stime) + std::stol(cutime) +std::stol(cstime);
  eTime  = Jiffies() - std::stol(startTime);
  float cpuUtilization = (float) tTime/eTime;
  return cpuUtilization;
 }

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long activeJiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int kUser, kNice, kSystem, kIRQ, kSoftIRQ, kSteal;
  int idleJiffie;
  string line, key;
  if(filestream.is_open()) {
    getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if(key == "cpu"){
      linestream >> kUser >> kNice >> kSystem >> idleJiffie >> idleJiffie >> kIRQ >> kSoftIRQ >> kSteal;
    }
    activeJiffies = kUser + kNice + kSystem + kIRQ + kSoftIRQ + kSteal;
    return activeJiffies;
  }
  return 0;
 }

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long iddleJiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  int kIdle, kIOWait;
  int activeJiffie;
  string line, key;
  if(filestream.is_open()) {
    getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if(key == "cpu"){
      linestream >> activeJiffie >> activeJiffie >> activeJiffie >> kIdle >> kIOWait;
    }
    iddleJiffies = kIdle + kIOWait;
    return iddleJiffies;
  }
  return 0;
 }

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, key;
  vector<string> CPUData{10};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if(key == "cpu"){
      linestream >> CPUData[0] >> CPUData[1] >> CPUData[2] >> CPUData[3] >> CPUData[4]  >> CPUData[5]  >> CPUData[6]  >> CPUData[7] >> CPUData[8] >> CPUData[9];
    }
  }
  return CPUData;
 }

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if (key == "processes"){
          return value;
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if (key == "procs_running"){
          return value;
        }
      }
    }
  }
  return 0;
 }

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename); 
  if (filestream.is_open()){
    std::getline(filestream,line);
  }
  return line;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value, ram{"0"};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line); 
      while(linestream >> key >> value){
        if(key == "VmSize"){
          ram = value;
        }
      }
    }
  }
  int ramInt = std::stoi(ram)/1024;
  string ramString = to_string(ramInt);
  return ramString;
  return 0;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key, line, value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
     while(std::getline(filestream, line)){
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line); 
      while(linestream >> key >> value){
        if(key == "Uid"){
           return value;
        }
      }
  }
 }
 return 0;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string key, line, value, x;
  std::ifstream filestream(kPasswordPath);
  string userId = Uid(pid);
  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> x >> value){
        if(value == userId){
          return key;
        }
      }
    }
  }
  return string();
 }

// DONE: Read and return the uptime of a process

long LinuxParser::UpTime(int pid) {
  std::string line, remove;
  long upTime;
  long pidUpTime;
  std::ifstream filestream(kProcDirectory+std::to_string(pid)+kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream linestream(line);
    linestream>>remove>>remove>>remove;
    for(int i=0;i<19;++i){
      linestream >>  upTime;
    }
  }
  pidUpTime = (upTime/sysconf(_SC_CLK_TCK));
  return pidUpTime;
}