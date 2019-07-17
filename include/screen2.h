#include <ncurses.h>
#include <panel.h>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>

#ifndef _RTOP_
#define _RTOP_
#include "rtop_logger.h"
#include "rtop_utils.h"
#include "view2.h"
#include "sm.h"
#include "params.h"

extern src::severity_logger< severity_level > lg;
extern logSpacer log_spacer;

namespace rtop
{
 extern MapOfKeyDicts mapKeyDict;
 extern MemFuncPDict funcPDict;
 
 //! Top Level UI object. Responsible to switching between views and capturing key input 

 //! 
 //! Performs three major tasks <br/>
 //! 1. initializes ncurses and refreshes the screen <br/>
 //! 2. captures key input and send it for processing down the UI object heirarchy <br/>
 //! 3. switch between views <br/>
 //! 
 class ScreenManager
 {
   public:
     ScreenManager(int);
     ~ScreenManager();
     void start();  
     void refresh();
     void resolveKey(int ch);
     int CurrentView() const{return smViews.currState();}
     int uUid() const {return uuid;}
   private:
     int uuid;
     //! pointer to current view object
     View *currView;
     //! collection of view object pointers
     std::vector<View*> views;
     //! Keys statemachine
     StateMachine smKeys;
     //! Views statemachine
     StateMachine smViews;
     friend class XMLTree;
 };
 
  
 //! ScreenManager constructor. Initializes ncurses  
 ScreenManager::ScreenManager(int id): uuid{id}
 {
   // initialize ncurses
   initscr();                            // initialize terminal in curses mode
   raw();                                // disable line buffering, interpret each keypress - don't pass directly to terminal
   noecho();                             // switch off echoing
   keypad(stdscr, TRUE);                 // enable function keys and arrow key for the stdscr. get key input from stdscr
   set_escdelay(100);                    // esc delay = 100msec, so that ESC is processed without any perceptible delay
   curs_set(0);                          // prevent cursor blinking in menu
   start_color();
   for(auto pos: colors)                 // initialize color pairs
    init_pair(pos.first, (pos.second).first, (pos.second).second);
 }

 //! resolves the key input at ScreenManager level

 //!
 //! passes keys to ScreenManager's Key StateMachine <br/>
 //! key is passed to previous and current views for further processing <br/>
 //! screen is refreshed
 //!
 void ScreenManager::resolveKey(int ch)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ScreenManager-"<<uuid<<"_resolvekey::";
   // set KEY_INPUT global to received key. 
   KEY_INPUT = ch;                                           // KEY_INPUT will be used by GUI objects downstream
   // resolve key with ScreenManager's key dictionary 
   (mapKeyDict.getKeyDict(smKeys.currState())).resolve(ch);  // carried out actions may update ScreenManager's View StateMachine and Key StateMachine
   
   // if EXIT global set to true, return immediately
   if (EXIT)
   {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ScreenManager-"<<uuid<<"_resolvekey::EXIT";
    log_spacer.delSpace();
    return;
   }
   
   // if View SM reports view change 
   if (smViews.isChanged())  
   {   
     (funcPDict.getView(smViews.prevState()))->hide();            // hide prev view
     currView = funcPDict.getView(smViews.currState());           // update the current view for the ScreenManager
     (funcPDict.getView(smViews.prevState()))->refresh();         // refresh prev view to actually be hidden
     currView->show();                                            // show the new current view to actually be visible
     (funcPDict.getView(smViews.prevState()))->resolveKey(ch);    // pass on key to prev view for processing
   }
   currView->resolveKey(ch);                                      // pass on the key current view for processing 
   refresh();                                                  // refreshes current view and makes it any changes visible on screen
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ScreenManager-"<<uuid<<"_resolvekey::";
   log_spacer.delSpace();
 }
  
 //! Captures key input from screen.   
 void ScreenManager::start()
 {
  log_spacer.addSpace();  
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ScreenManager-"<<uuid<<"_start::";
  int ch;
  while(1)
  {
    ch = getch();    // wait until key input
    BOOST_LOG_SEV(lg, info)<<log_spacer<<"ScreenManager-"<<uuid<<"_start::, KEYINPUT="<<ch;
    resolveKey(ch);  // resolve key
    if (EXIT)        // if EXIT set to true, exit loop
      break;
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ScreenManager-"<<uuid<<"_start::";
  log_spacer.delSpace();
 }   

 //! refreshes screen
 
 //!
 //! invokes current view's refresh function
 //! 
 void ScreenManager::refresh()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ScreenManager-"<<uuid<<"_refresh::";
   currView->refresh();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ScreenManager-"<<uuid<<"_refresh::";
   log_spacer.delSpace();
 }
 
 //! ScreenManager destructor. exit curses mode, thus releasing all resources  
 ScreenManager::~ScreenManager()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ScreenManager-"<<uuid<<"_~::";
  endwin();   // ends the curses mode. frees memory taken up by ncurses and puts the terminal in the normal mode
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ScreenManager-"<<uuid<<"_~::";
  log_spacer.delSpace();
 } 
 
}  //namespace rtop
#endif // _RTOP_

