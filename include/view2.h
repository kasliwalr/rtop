#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <vector>
#include <utility>
#include <mutex>

#ifndef _VIEW_H_
#define _VIEW_H_
#include "rtop_logger.h"
#include "panel2.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
  
 extern MapOfKeyDicts mapKeyDict;
 extern MemFuncPDict funcPDict;

 //! 2nd level UI object. Responsible for switching between panels and resolving key input

 //!
 //! Tasks <br/>
 //! 1. switch between panels <br/>
 //! 2. captures key input and send it for processing down the UI object heirarchy <br/>
 //! 3. assigning position and neighbours of panels within it <br/>
 //!
 class View
 {
   private:
    int uuid;
    //! pointer to window associated with the view, contained panels' windows are derived from it 
    WINDOW* win;
    //! pointer to current panel's (Browse/Edit Panel) data structure 
    SimplePanelData* curr_panel;
    //! vector of pointers to panel data structures corresponding to panels contained in the view
    std::vector<SimplePanelData*> panels;
    //! vector of contained panels' window positions within view window
    std::vector<std::pair<int, int>> panels_yx;
    //! Keys statemachine
    StateMachine smKeys;
    //! Panel statemachine
    StateMachine smPanels;
    //! indicates if view is hidden/visible
    bool hidden;
    //! index of color_pair used for selected menu item in edit mode
    int editing_color_pair = 0;
    //! index of color_pair used for selected menu item in non-edit mode 
    int selection_color_pair = 0;
    //! index of color_pair used for non-selected menu items
    int basic_color_pair = 0;
    //! index of color_pair used for panel header
    int header_color_pair = 0;
    //! dictionary storing pair of pointers to  panel data structures corresponding to left/right neighbour panels of each panel in the view 
    std::map<SimplePanelData*, std::pair<SimplePanelData*, SimplePanelData*>> neighbourhood;
    //! mutex for accessing View::refresh 
   std::mutex view_mutex;
   public:
    View(int);
    int uUid() const{return uuid;}
    void resolveKey(int);
    void refresh();
    void show();
    void hide();
    friend class XMLTree;
 };     

 //! View Constructor 
 View::View(int id): uuid{id}
 {
   
 } 
 
 //! resolves the key input at View level

 //!
 //! passes key to View's Key statemachine <br/>
 //! key is passed to current and previous view for further processing <br/>
 //! view is refreshed
 //! 
 void View::resolveKey(int ch)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> View-"<<uuid<<"_resolveKey::";
   (mapKeyDict.getKeyDict(smKeys.currState())).resolve(ch); // carried out actions may update View's Panel Statemachine and Key StateMachine
   
   // if Panel SM report panel change
   if(smPanels.isChanged())
   {
     SimplePanelData* prev_panel = funcPDict.getPanel(smPanels.prevState()); 
     prev_panel->selectionOff();   // set previous panel's selected item color to basic_color_pair
     prev_panel->resolveKey(ch);   // pass on key input to previous panel for processing 
     curr_panel = funcPDict.getPanel(smPanels.currState());  // update the pointer to current panel
     curr_panel->selectionOn(); // set new (current) panel's selected item color to selection_color_pair 
   }
   curr_panel->resolveKey(ch);  // pass on key input to current panel (potentially new) for processing
   refresh(); // refreshes the view, and makes any children panels' changes visible on screen 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- View-"<<uuid<<"_resolveKey::";
   log_spacer.delSpace();
 }
 
 //! refreshes view

 //!
 //! invokes contained panels' refresh functions
 //! 
 void View::refresh()
 {
  log_spacer.addSpace(); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> View-"<<uuid<<"_refresh::";
  view_mutex.lock();   // acquire lock
  
  // perform actions to hide/show panels contained in the view
  if (!hidden)   // to show 
  {
    // iterate over all contained panel data strutures
    for(int i=0; i<panels.size(); i++)
    {
      panels[i]->show();                                          // show the panel
      panels[i]->moveTo(panels_yx[i].first, panels_yx[i].second); // move panel's window to its location in view's window
      panels[i]->refresh();                                       // refresh panel to update menu contents
 
      // set panels left/right neighbours in the view
      for(auto pos: neighbourhood)
      {
        (pos.first)->setLeftNbr((pos.second).first);
        (pos.first)->setRightNbr((pos.second).second);
      }
    } 
    // set current panel's to be selected 
    curr_panel->selectionOn(); 
  }
  else          // to hide
  {
    // iterate over all contained panel data structures
    for(auto panel: panels)
    {
      panel->hide();                 // hide the panel
      panel->selectionOff();         // de-select all the panels
      panel->Top();                  // reset selected item to top of menu
    }
    smKeys.reset();                  // reset view's Key Statemachine
    smPanels.reset();                // reset view's Panel Statemachine
    curr_panel = funcPDict.getPanel(smPanels.currState());  // update the current panel based on start state of Panel statemachine
  }
  wrefresh(win);                 // refresh view's window for changes to take effect in all children windows
  view_mutex.unlock();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- View-"<<uuid<<"_refresh::";
  log_spacer.delSpace();
 }  

 //! sets view to visible
 void View::show()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> View-"<<uuid<<"_show::";
   hidden = false;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- View-"<<uuid<<"_show::";
   log_spacer.delSpace();
 }

 //! sets view to hidden
 void View::hide()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> View-"<<uuid<<"_hide::";
   hidden = true;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- View-"<<uuid<<"_hide::";
   log_spacer.delSpace();
 }

} // namespace rtop

#endif // _VIEW_H_
