#include <string>
#include <map>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string.h>
#include <stdexcept>

#ifndef PROC_INFO_H_
#define PROC_INFO_H_
#include "rtop_logger.h"
#include "info.h"
#include "rtop_utils.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{

 //! interfaces with linux API to read process information and update ProcDb with it
 class ProcInfo
 {
   private:
     int uuid;
     //! variable to hold property name to be sorted. used by ProcInfo::sort to sort process values corresponding to sortkey  
     std::string sortkey;
     //! hashset for storing processs PIDs 
     std::unordered_set<int> pid_set;
     //! hashset for storing property names, whose corresponding vectors in ProcDb need to be updated
     std::unordered_set<std::string> prop_set;    
     //! sorts property value vector corresponding to sortkey, and stores resulting permutation vector in ProcDb::sorted_indices  
     void sort(); 
     //! reads property values specified in ProcInfo::prop_set for each process and stores result in ProcDb::database 
     void read();
     //! pointer to access ProcDb data structure
     ProcDb* proc_database;
   public:
     //! ProcInfo constructor. Instantiates ProcInfo object with provided unique identifier 
     ProcInfo(int id):uuid{id}{}
     //! \todo: not implemented. implement it
     void kill(){}
     //! invoked by Columns::read. reads process prop values, sorts them and then updates ProcDb
     void update(std::vector<std::string>& prop_vec, std::string sort_key);  
   friend class XMLTree;
 };


 void ProcInfo::update(std::vector<std::string>& prop_vec, std::string sort_key)
 {
   log_spacer.addSpace(); 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcInfo-"<<uuid<<"_update::, id: "<<uuid;
   proc_database->access.lock();       // acquire lock on ProcDb before changing it 
   sortkey = sort_key;
   for(auto p: prop_vec)
    prop_set.insert(p);
   read(); // reads the /proc database and updates proc_database based on supplied properties
   sort(); // reads a specific prop vector, and updates the proc_database.sorted_indices vector field. 
   proc_database->access.unlock();
   
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcInfo-"<<uuid<<"_update::, id: "<<uuid;
   log_spacer.delSpace();
 }


 void ProcInfo::read()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcInfo-"<<uuid<<"_read::";
  
  // parse all sub-directories corresponding to numeric pids from /proc directory 
  DIR* dirp = opendir("/proc");     // open /proc directory and returns handle to parse directory contents
  if (dirp == NULL)                 // if error in opening directory
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"ProcInfo-"<<uuid<<"_read::, failed to open directory";
   throw std::runtime_error("ProcInfo::read, failed to open directory\n");
  }
  
  struct dirent* dp;
  while(1)                          // iterate over all contents of /proc using directory pointer dirp obtained above
  {
   dp = readdir(dirp);              // returns object of type dirent, that contains sub-directory name
   if (dp == NULL)                  // reached end of /proc directory 
    break;
   std::string str(dp->d_name);     
   if (str.find_first_not_of("0123456789") == std::string::npos) // if sub-directory name is all numeric, insert into pid hashset
    pid_set.insert(std::stoi(str));
   
  } 
  if (closedir(dirp) == -1)         // finished reading /proc, close dir stream
  {
    BOOST_LOG_SEV(lg, fatal)<<log_spacer<<"ProcInfo-"<<uuid<<"_read::, failed to close /proc directory";
    throw std::runtime_error("ProcInfo::read, failed to closed /proc directory\n");
  }
  
  // update ProcDb::database 
  for(auto p=prop_set.begin(); p != prop_set.end(); p++)     // clear those property process vectors that need to be update
   proc_database->database[*p].clear();
  
  // read /proc/pid/stat, /proc/pid/status and /proc/pid/cmdline files to updates the property process vectors in ProcDb::database 
  std::stringstream ss(std::ios_base::out | std::ios_base::in | std::ios_base::ate);
  ss.exceptions(std::ios::failbit); 
  std::ifstream ifs_stat, ifs_cmdline, ifs_status; // file streams to open stat, status and cmdline files in /pid directory
  
  int d_val;                                       // temp variable to hold parsed value
  long l_val;                                      // temp variable to hold parsed value
  int proc_added = 0;                              // counter to track number of processes added

  // iterate over recently creted pid_set and parse cmdline, status and stat files for info
  for(auto pid: pid_set)                             
  {
    ss.str("");
    ss.clear();
    ss<<"/proc/"<<pid<<"/stat";
    ifs_stat.open(ss.str());                       // open file descriptor for /proc/pid/stat
    ss.str("");
    ss.clear();
    ss<<"/proc/"<<pid<<"/cmdline";
    ifs_cmdline.open(ss.str());                    // open file descriptor for /proc/pid/cmdline
    ss.str("");
    ss.clear();
    ss<<"/proc/"<<pid<<"/status";                  
    ifs_status.open(ss.str());                     // open file descriptor for /proc/pid/status
    
    if (!ifs_stat  || !ifs_cmdline || !ifs_status) // if process has been deleted already, one of the file opening could have failed. 
    {
     if (!ifs_stat)
      BOOST_LOG_SEV(lg, error)<<log_spacer<<"/proc/"<<pid<<"/stat "<<strerror(errno);
     if (!ifs_cmdline)
      BOOST_LOG_SEV(lg, error)<<log_spacer<<"/proc/"<<pid<<"/cmdline "<<strerror(errno);
     if (!ifs_status)
      BOOST_LOG_SEV(lg, error)<<log_spacer<<"/proc/"<<pid<<"/status "<<strerror(errno);
     
     ifs_stat.clear();                             // clear status before moving to next pid
     ifs_cmdline.clear();
     ifs_status.clear();
    }
    else                                            // if files were successfully opened
    {
     std::string line, cmdline_str, status_line;
     
     // read command line string
     std::getline(ifs_cmdline, cmdline_str);        // read from command line string for process from cmdline file
     ifs_cmdline.close();
     
     cmdline_str.clear();
     if (cmdline_str == "")                         // if cmdline file is empty, read command string info from status file
     {
      BOOST_LOG_SEV(lg, debug)<<log_spacer<<"/proc/"<<pid<<"/cmdline"<<" empty";
      std::string tmpstr = "";
      ss.str("");
      ss.clear();
      std::getline(ifs_status, status_line);
      ss<<status_line;
      ss>>tmpstr;
      while(ss.good())                              // concatenate all white-space separated content into command line string
      {  
       ss>>tmpstr;
       cmdline_str += tmpstr;
      }
     }
     ifs_status.close();
    
     // read other process properties 
     std::getline(ifs_stat, line);                  // read data string containing property field from stat file
     ifs_stat.close();

     std::string discard_str;
     ss.str("");
     ss.clear();
     ss<<line;                                      // read the /proc/PID/stat file with a single line
     int loc = 0;
     //! \todo: modify loop to read all properties from /proc/pid/stat file, then compute different property values by combining entries 
     while(ss.good())                               // parse the line containing white-spaced separated property values
     {
      loc++;
      if (proc_database->field_prop_dict.find(loc) != proc_database->field_prop_dict.end())  // if loc corresponds to property field location recognized by ProcDb
      {
       if (prop_set.find(proc_database->field_prop_dict[loc].first) == prop_set.end())     // if property is not in prop_set to be updated, skip
       {
        ss>> discard_str;
        continue;
       }
       std::string strval;
       if(proc_database->field_prop_dict[loc].second == "int")                            // if prop_type is integer type read into integer var
       {
        try{
         ss>>d_val;                                                                       //! \bug: throwing exception occasionaly. investigate
        }
        catch(std::runtime_error e)
        {
         BOOST_LOG_SEV(lg, error)<<log_spacer<<e.what();
         ss.clear();
        }
        strval = std::to_string(d_val);
        InfoProc<std::string> inf{strval};                                                // construct InfoProc object for storage into ProcDb database
        proc_database->database[proc_database->field_prop_dict[loc].first].push_back(inf);// put InfoProc object into approprirate property process vector 
        continue;
       }
       if(proc_database->field_prop_dict[loc].second == "long")                           // if prop_type is long type, read into long var
       {
        ss>>l_val;
        if (loc == 14 || loc == 15)                                                       // perform data computation for time calculation
          strval = msecToTimeStr(l_val*10);
        else
          strval = std::to_string(l_val);
        InfoProc<std::string> inf{strval};
        proc_database->database[proc_database->field_prop_dict[loc].first].push_back(inf); // put InfoProc object into appropriate property process vector 
        continue;
       }
      }                                     
      else                                                                                  // if loc does not correspond to any location recognoized ProcDb 
       ss>>discard_str;
     }
     InfoProc<std::string> inf{cmdline_str};
     //! \todo: hardcoded property name for Command. receive this value during initialization
     proc_database->database["Command"].push_back(inf);                                     // update "Command" property process vector 
     proc_added++;
    }
  }
  proc_database->dbsize = proc_added;                                                       // update ProcDb::dbsize variable by number of processes parsed 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcInfo-"<<uuid<<"_read::";
  log_spacer.delSpace();
 }

 //! \todo: long and int sorting handled separately. investigate and refactor? 
 void ProcInfo::sort()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcInfo-"<<uuid<<"_sort::, sortkey: "<<sortkey<<" sorted_indices size: "<<proc_database->sorted_indices.size()<<"database size: "<<proc_database->database[sortkey].size();
  
  proc_database->sorted_indices.clear();
  if (proc_database->prop_type_dict[sortkey] == "int")     // if property type is int
  {
    // copy property process vector corresponding to sortkey 
    std::vector<int> v;                                
    for(auto item: proc_database->database[sortkey])
     v.push_back(std::stoi(item.Val()));
    // sort the property process vector, store resulting permutation vector in ProcDb::sorted_indices
    proc_database->sorted_indices = sort_permutation(v, [](int const& a, int const& b){return a<b;});  
  }
  if (proc_database->prop_type_dict[sortkey] == "long")    // if property type is long
  {
    // copy property process vector corresponding to sortkey 
    std::vector<long> v;
    for(auto item: proc_database->database[sortkey])
     v.push_back(std::stol(item.Val()));
    // sort the property process vector, store resulting permutation vector in ProcDb::sorted_indices
    proc_database->sorted_indices = sort_permutation(v, [](const long& a, const long& b){return a<b;}); 

  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcInfo-"<<uuid<<"_sort:: sorted_indices size: "<<proc_database->sorted_indices.size()<<"database size: "<<proc_database->database[sortkey].size();
;
  log_spacer.delSpace();  
 }

} // rtop

#endif
