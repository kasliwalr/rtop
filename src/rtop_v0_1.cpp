#include <iostream>
#include <thread>
#include "rtop_logger.h"
#include "columns.h"
#include "mem_func.h"
#include <stdexcept>
#include "fileio.h"

extern rtop::MemFuncPDict rtop::funcPDict;
extern src::severity_logger< severity_level > lg;
extern logSpacer log_spacer;

//! rtop entry point. Invoked as `./rtop config_file.xml [log_level]`

//!
//! intializes logging and then spawns one additional thread, **key_input thread**
//! 
int main(int argc, char* argv[])
{
 // checks for correct number of arguments
 if (argc <2)
 {
   std::cerr<<"error!\n";
   std::cerr<<"usage: ./out confile_file or ./out config_file log_level\n";
   return 1;
 }

 // initialize logging. using boost_log library for logging 
 // if logging level not provided, set to "fatal"
 if (argc == 2)
 {
   std::cerr<<"setting log level to fatal\n";
   init_logging("fatal");
   BOOST_LOG_SEV(lg, debug)<<"logging setup complete, level: "<<"fatal"; 

 }
 if (argc == 3)
 {
   init_logging(argv[2]);
   BOOST_LOG_SEV(lg, debug)<<"logging setup complete, level: "<<argv[2]; 
 }  
 
 try
 {
  // initialize system by loading GUI details from the XML config file. 
  rtop::XMLTree object_tree(argv[1]);                         // 1st pass over XML, creates GUI and non-GUI objects
  rtop::ScreenManager* screen = object_tree.instantiate();    // 2nd pass over XML, populates objects with information 
  BOOST_LOG_SEV(lg, debug)<<"XML LOADED"; 
  rtop::Columns* pclms = rtop::funcPDict.getColumn(303);  // TODO: constant should not be here, handle it in getColumn
  screen->refresh();   /***DO NOT DELETE, REQUIRED FOR FIRST TIME LOADING OF PROCESS PROP FROM ACTIVEPROC PANEL***********/ 

  // start key_input thread
  BOOST_LOG_SEV(lg, debug)<<"Starting key capture"; 
  std::thread t_key_input(&rtop::ScreenManager::start, screen);  
 
  // main loop, responsible for refreshing the screen every POLL_INTVL milliseconds
  while(1)
  {
    if (rtop::EXIT)  // if EXIT global has been set to true, exit the loop
      break;
    pclms->read();          // read /proc database and store information in ProcDb
    screen->refresh();   // refreshes screem by forcing ProcViewPanel to important information from ProcDb into ProcViewPanel::columns
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"main, Screen refreshed"; 
    rtop::milliSleep(rtop::POLL_INTVL); // put main thread to sleep
    BOOST_LOG_SEV(lg, debug)<<"waking up after "<<rtop::POLL_INTVL<<" msec";
  } 
  t_key_input.join();   // wait for key input thread to finish running.
  delete(screen);       // invokes ScreenManager destructor 
 }
 catch(std::runtime_error e)
 {
   BOOST_LOG_SEV(lg, error)<<e.what();
   std::cerr<<e.what()<<std::endl;
 }

 return 0;
}


