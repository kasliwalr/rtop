#include <string>
#include <vector>

#ifndef _COLUMNS_H_
#define _COLUMNS_H_
#include "rtop_logger.h"
#include "proc_info.h"
#include "screen2.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
 //! responsible for managing the flow of information between ProcViewPanel, ProcDb and ProcInfo
 class Columns
 {
   private:
    int uuid;
    //! pointer to ScreenManager object 
    ScreenManager* pscreen;
    //! pointer to ProcInfo object
    ProcInfo* p_procinfo;
    //! pointer to ProcViewPanel object
    ProcViewPanel* proc_view;
    //! list of View class object uuids
    std::vector<int> proc_views;
   public:
    //! Columns constructor. Initializes Columns object with provided unique identifier 
    Columns(int);
    //! \todo: not implemented. implement
    void markPid();
    //! invokes ProcDb update based on properties currently on display in ProcViewPanel 
    void read();
    //! sets view uuids. \todo: redundant. remove 
    void set(std::vector<int>);
    friend class XMLTree;
 };

 
 Columns::Columns(int id): uuid{id}
 {
 }
 
 void Columns::markPid()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> Columns-"<<uuid<<"_markPid::";
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- Columns-"<<uuid<<"_markPid::";
  log_spacer.delSpace();
 }
 
 void Columns::set(std::vector<int> proc_views_uuids)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> Columns-"<<uuid<<"_set::";
  proc_views.clear();
  proc_views.shrink_to_fit();
  for(auto elm: proc_views_uuids)
   proc_views.push_back(elm);
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- Columns-"<<uuid<<"_set::";
  log_spacer.delSpace();
 }
 
 void Columns::read()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> Columns-"<<uuid<<"_read::";
  int curr_view = pscreen->CurrentView();          // get current view's uuids 
  for(auto view: proc_views)
  {
   if (view == curr_view)                           // if curr_view contains processviewpanel, update once
   {
    std::vector<std::string> properties = proc_view->displayedKeys();   // parse property keys on display in ProcViewPanel
    std::string sort_key = proc_view->sortKey();                        // parse sortkey currently set in ProcViewPanel
    p_procinfo->update(properties, sort_key);                           // update ProcDb 
    break;
   }
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- Columns-"<<uuid<<"_read::";
  log_spacer.delSpace();
 }

}  // namespace rtop
#endif // _COLUMNS_H_
