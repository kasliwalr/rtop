#define _BSD_SOURCE
#include <vector>
#include <map>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <ncurses.h>
#include <stdexcept>

#ifndef RTOP_UTILS_H_
#define RTOP_UTILS_H_
#include "params.h"
#include "info.h"
#include "rtop_logger.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
//! returns <a href="https://stackoverflow.com/questions/17074324/how-can-i-sort-two-vectors-in-the-same-way-with-criteria-that-uses-only-one-of">permutation vector</a> after sorting the given vector based on the provided compare function

//!
//! compare function passed to sort_permutation should follow <a href="https://stackoverflow.com/questions/979759/operator-and-strict-weak-ordering/981299#981299">strick weak ordering</a>
//! 
template <typename T, typename Compare>
std::vector<int> sort_permutation(const std::vector<T>& vec,Compare compare)
{
    std::vector<int> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),[&](unsigned int i, unsigned int j){return compare(vec[i], vec[j]); });
    return p;
}

//! converts string to its equivalent char value. \todo: clarify the problem, and improve solution. refactor 

char toChar(std::string str1)
{
  if (str1[0] == '\\' && str1[1] == 'n')
   return '\n';
  else if (str1 == " ")
   return ' ';
  else if (str1 == ",")
   return ',';
  else
   return ',';
}

//! responsible for parsing strings in a line, specfied by some character separator


//!
//! provides API to read in a line string, specify separator and accessing the parsed strings
//!
class StringParser
{
  private:
   //! character separator
   char sep;
   //! stores the parsed strings
   std::vector<std::string> words;
   //! size of words
   int sz;
  public:
   //! StringParser constructor. Instantiates a StringParser object with provided separator character
   StringParser(char c): sep{c}{sz=0;}
   //! reads in a line and stores parsed string into StringParser::words 
   void operator<<(std::string);
   //! returns a vector of parsed strings stored in StringParser::words
   std::vector<std::string> Words() const {return words;}
   //! returns size of StringParser::words vector
   int size() const {return sz;}
   //! clears StringParser::words vector
   void clear()
   {
    words.clear();
    sz = 0;
   }
   //! sets separator character
   void setSeparator(char c){sep = c;}
};

void StringParser::operator<<(std::string str1)
{
  int len=0, start=0; 
  for(int i=0; i<str1.size(); i++)               // iterate over each character of line
  {
    if (str1[i] == sep)                          // if character is separtor, parse substring from position start, of length len, and push into words vector
    {
     words.push_back(str1.substr(start, len));
     len = 0;
     start = i+1;
    }                                            // increment length of current substring
    else
     len++; 
  }
  words.push_back(str1.substr(start, len));      // push the last substring
  sz = words.size();
  //remove whitespace
  //
  for(auto& word: words)                         //! <a href="https://stackoverflow.com/questions/83439/remove-spaces-from-stdstring-in-c">remove white space</a> from parsed strings. \todo: no rational. rationalize
   word.erase(remove_if(word.begin(), word.end(), isspace), word.end());
}

//! makes calling thread sleep for specified milliseconds
void milliSleep(int msec)
{
 log_spacer.addSpace();
 BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> milliSleep::"<<msec;
 int r = 0;
 for(int i=0; i<(msec/20); i++)     // divide interval into 20ms parts
 {
   r = usleep(20000);               // sleep for 20 msec
   if (r == -1)
   {
    BOOST_LOG_SEV(lg, error)<<log_spacer<<"usleep: "<<strerror(errno);
    log_spacer.delSpace();
    throw std::runtime_error("usleep: ");
   }
   if (EXIT)                        // to improve responsivness to F10 press
   {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"milliSleep::rcvd EXIT, "<<msec-(i+1)*20<<" msec remaining";
    break;
   }
 }
 r = usleep((msec%20 )*1000);       // sleep for remaining usec
 if (r == -1)
 {
   BOOST_LOG_SEV(lg, error)<<log_spacer<<"usleep: "<<strerror(errno);
   log_spacer.delSpace();
   throw std::runtime_error("usleep: ");
 }
 BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- milliSleep::with "<<0<<" msec remaining";
 log_spacer.delSpace();
}

//! converts msec to TimerStr for display as min:sec.secs_fraction
std::string msecToTimeStr(unsigned long msec)
{
 unsigned long sec = msec/1000;        // seconds
 int msec_remainder = msec%1000;       // remaining msec
 unsigned long minutes = sec/60;       // minutes
 int sec_remainder = sec%60;           // remaining seconds
 std::stringstream ss{""};             // construct time string
 ss<<minutes<<":"<<sec_remainder<<"."<<msec_remainder;
 return ss.str();
} 

//! overloaded << operator for printing InfoProc contents
template<class T>
std::ostream& operator<<(std::ostream& ofs, const InfoProc<T>& obj)
{
  ofs<<obj.val;
  return ofs;
}

//! concatenates a vectors entries into a string

//! 
//! the supplied vector's type should have an overloaded << operator that returns a string
//! 
template<class T>
std::string vec_to_string(std::vector<T>& vec)
{
 std::stringstream ss;
 for(auto elem: vec)
   ss<<elem<<" ";
 return ss.str();
}

//! swaps supplied vectors' element corresponding to indices i and j 
template<class T>
void swap(std::vector<T>& v, int i, int j)
{
  T tmp = v[i];
  v[i] = v[j];
  v[j] = tmp;
}

//! swaps two variables
template<class T>
void swap(T&a, T&b)
{
 T tmp = a;
 a = b;
 b = tmp;
}

//! dictionary containing color_pair number corresponding to each color_name
std::map<std::string, int> color_map{{"BLACK", COLOR_BLACK}, {"RED", COLOR_RED}, {"GREEN", COLOR_GREEN}, {"YELLOW", COLOR_YELLOW}, {"BLUE", COLOR_BLUE}, {"MAGENTA", COLOR_MAGENTA}, {"CYAN", COLOR_CYAN}, {"WHITE", COLOR_WHITE}};

} // namespace rtop

#endif // RTOP_UTILS_H_
