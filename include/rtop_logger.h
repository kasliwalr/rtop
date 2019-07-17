/*
 *          Copyright Andrey Semashev 2007 - 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#include <string>
#include <map>
#include <fstream>
#include <thread>
#include <mutex>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>

#ifndef _LOGGER_H_
#define _LOGGER_H_
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;

//! sets up logging using <a href="https://www.boost.org/doc/libs/1_63_0/libs/log/doc/html/index.html">Boost.Log</a> library
void init_logging(std::string sev_level)
{
 // set log filtering level, fatal > error > warning > info > debug 
 boost::shared_ptr< logging::core > core = logging::core::get();
 if (sev_level == "debug")
 {
  core->set_filter
  (
       logging::trivial::severity >= logging::trivial::debug
  );
 }
 if (sev_level == "info")
 {
  core->set_filter
  (
       logging::trivial::severity >= logging::trivial::info
  );
 }
 if (sev_level == "warning")
 {
  core->set_filter
  (
       logging::trivial::severity >= logging::trivial::warning
  );
 }
 if (sev_level == "error")
 {
  core->set_filter
  (
       logging::trivial::severity >= logging::trivial::error
  );
 }
 if (sev_level == "fatal")
 {
  core->set_filter
  (
       logging::trivial::severity >= logging::trivial::fatal
  );
 }  

 // set up sink as text file 
 boost::shared_ptr< sinks::text_file_backend > backend =
        boost::make_shared< sinks::text_file_backend >(
            keywords::file_name = "sample_%5N.log",                                          
            keywords::rotation_size = 5 * 1024 * 1024,                                     
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0)
        );


 backend->auto_flush(true); // flushes buffers of attached streams after each log record is written 

 typedef sinks::synchronous_sink<sinks::text_file_backend> sink_t;
 boost::shared_ptr<sink_t> sink(new sink_t(backend));

 // set up format for message, data(min:sec.frac) [thread_id] message
 logging::formatter formatter = expr::stream
                               <<expr::format_date_time<boost::posix_time::ptime >("TimeStamp", "%-%M:%S.%f")
                               <<" ["<<logging::expressions::attr<logging::attributes::current_thread_id::value_type>("ThreadID")<<"]:"
                               <<expr::message;
 sink->set_formatter(formatter);
 core->add_sink(sink);
 logging::add_common_attributes(); 
}


using namespace logging::trivial;
src::severity_logger< severity_level > lg;

//! enables indentation of logs for easy viewing

//!
//! developer can pass this object to BOOST log stream and it will add appropriate space (corresponing to invoking thread) to the stream. 
//! This class has methods for adding/deleting white spaces for each thread.<br/>
//! *Note*: many member methods are under mutex control as a precaution - mutex use is not backed by solid scenario. Methods do access a common 
//! container `spacers`, but it is not clear whether any data-races are generated because of that. 
//!
class logSpacer
{
 private:
  std::mutex lgmutex; 
  std::map<std::thread::id, std::string> spacers; 
 public:
  void addSpace();
  void delSpace();
  std::string spacer();
  friend std::ostream& operator<<(std::ostream& ofs, logSpacer& lgSpacer);
};

//! adds one white-space to white-space string corresponding to invoking thread

//! 
//! function body is under mutex control.
//! 
void logSpacer::addSpace()
{
  //std::lock_guard<std::mutex> lg1(lgmutex);            // acquire mutex, released when lg1 goes out of scope
  // Critical Section Starts //
  std::thread::id t_id = std::this_thread::get_id();   
  if (spacers.find(t_id) == spacers.end())
    spacers[t_id] = ""; 
  spacers[t_id] += "  ";
  // Critical Section Ends //
}

//! deletes one white-space from white-space string corresponding to invoking thread

//! 
//! function body is under mutex control
//! 
void logSpacer::delSpace()
{
 // std::lock_guard<std::mutex> lg1(lgmutex);          // acquire mutex, released when lg1 goes out of scope
  // Critical Section Starts // 
  std::thread::id t_id = std::this_thread::get_id();
  if (spacers.find(t_id) == spacers.end())
    return;
  if (spacers[t_id].empty())
   return;
  spacers[t_id].erase(0,2);
  // Critical Section Ends //
}

//! returns white-space string corresponding to invoking thread
std::string logSpacer::spacer()
{
 
 std::thread::id t_id = std::this_thread::get_id();
 if (spacers.find(t_id) == spacers.end())
   return "";
 return spacers[t_id];
}

//! pushes white-space string corresponding to invoking thread to std::ostream

//! 
//! function body is under mutex control
//! 
std::ostream& operator<<(std::ostream& ofs, logSpacer& lgSpacer)
{
  std::lock_guard<std::mutex> lg1(lgSpacer.lgmutex);
  ofs<<lgSpacer.spacer();
  return ofs;
} 

logSpacer log_spacer;
#endif
