#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <mutex>
#include <sstream>

#ifndef _INFO_
#define _INFO_
#include "rtop_logger.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
  //! data structure that defines type of entry from which panel menu entrires obtain their c-string data
 
  //!
  //! designed for holding string values with additional information. \todo: need to investigate usefulness and performance. investigate and refactor
  //! 
  class Info
  {
   private:
    //! header, longdesc and shortdesc of a panel entry 
    std::string header, longdesc, shortdesc;
    
    std::string units;
   public:
    Info(std::string hdr, std::string short_desc, std::string long_desc)
    {
     header = hdr;
     longdesc = long_desc;
     shortdesc = short_desc;
     units = "";
    }
  
   Info(std::string hdr, std::string short_desc, std::string long_desc, std::string unit_str)
   { 
    header = hdr;
    longdesc = long_desc;
    shortdesc = short_desc;
    units = unit_str;
   }
   
   Info(const Info& obj)
   {
    header = obj.header;
    longdesc = obj.longdesc;
    shortdesc = obj.shortdesc;
    units = obj.units;
   } 
 
    const char* Header(){return (const char*) header.c_str();}
    const char* longDesc(){return (const char*)longdesc.c_str();}
    const char* shortDesc(){return (const char*)shortdesc.c_str();}
    std::string Unit(){return units;}
  };

  //! data structure that defines type of entry from which panel menu entries obtain their c-string data

  //!
  //! template type and thus capable to holding any type - int, float, string etc
  //! provide API to access stored value as c-string
  //! \todo: seems a bloated way to hold data. investigate & refactor
  //!  
  template <class T=int>
  class InfoProc
  {
    private:
     T val;                       
     //! string representation of value 
     std::string shortdesc;                  
    public:
     //! InfoProc constructor. Initializes InfoProc with provided value 
     InfoProc(T v){val = v;}
     //! returns stored value
     T Val(){return val;}
     //! sets value
     void setVal(T v){val = v;}
     //! \todo: redundant. remove 
     const char* longDesc()
     {
       return ""; 
     }
     //! returns c-string corresponding to stored value. wasteful, as performs conversion everytime
     const char* shortDesc()
     {
      std::stringstream ss;
      ss<<val;
      shortdesc = ss.str();
      return (const char*)(shortdesc.c_str());
     } 
     friend bool operator<=(const InfoProc<std::string>&, const InfoProc<std::string>&);
     template<typename J>
     friend std::ostream& operator<<(std::ostream&, const InfoProc<J>&);
  }; 

  //! overloaded <= operator for comparing two InfoProc objects 
  bool operator<=(const InfoProc<std::string>& a, const InfoProc<std::string>& b)
  {
    return (std::stoi(a.val) <= std::stoi(b.val));
  } 
 
  //! data structure that hold property values for processes. contains facilities to enable their proper access and parsing for viewing 
  class ProcDb
  {
    private: 
     std::mutex access;
     int uuid;
     //! dictionary that holds vector of values corresponding to each property name string
     std::map<std::string, std::vector<InfoProc<std::string>>> database;
     //! vector that holds the permutation order to parse property vectors in sorted order 
     std::vector<int> sorted_indices;
     //! dictionary that holds property locations in /proc/pid/stat file, and correspondingg prop name and type
     std::map<int, std::pair<std::string, std::string>> field_prop_dict;
     //! dictionary hold property name and corresponding type
     std::map<std::string, std::string> prop_type_dict;
     //! size of property vector i.e. number of processes whose properties are being maintained
     int dbsize;
    public:
     //! ProcDb constructor. Instantiates object with provided unique identifier
     ProcDb(int id):uuid{id}{dbsize=0;} 
     friend class ProcInfo;
     friend class XMLTree;
     friend class ProcViewPanel;
  };
 
        
} // namespace rtop

#endif
  


