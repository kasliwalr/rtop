#include <map>
#include <vector>
#include <string>

#ifndef MEM_FUNC_H_
#define MEM_FUNC_H_
#include "rtop_logger.h"
#include "columns.h"
#include "action.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
 
 class ScreenManager; 
 class View;
 class SimplePanelData;
 class ColumnsPanelData;
 class StateMachine;
 class Columns;
 
 //! stores references to UI and non-UI objects and their callbacks.

 //! 
 //! provides methods for registering, accessing objects and registering and invoking object method callbacks of<br/> 
 //! various types used in rtop. Currently, only methods with following signatures are supported <br/>
 //! - `void(Classname::method_name*)()`
 //!  
 class MemFuncPDict
 {
 private:
  std::map<int, ScreenManager*> screen_objs;
  std::map<int, View*> view_objs;
  std::map<int, SimplePanelData*> panel_objs; 
  std::map<int, StateMachine*> sm_objs;
  std::map<int, ProcDb*> db_objs;
  std::map<int, ProcInfo*> pinfo_objs;
  std::map<int, Columns*> clm_objs;
    
  std::map<std::string, void(ScreenManager::*)()> funcp_void_screen;
  std::map<std::string, void(View::*)()> funcp_void_view; 
  std::map<std::string, void(SimplePanelData::*)()> funcp_void_panel;
  std::map<std::string, void(StateMachine::*)()> funcp_void_sm;
  std::map<std::string, void(ProcInfo::*)()> funcp_void_pinfo;
  std::map<std::string, void(Columns::*)()> funcp_void_clms;
 
 public:
  
  // API which registers object member methods as callbacks for various types
  void registerFunc(std::string func_name, void(ScreenManager::*)());
  void registerFunc(std::string func_name, void(View::*)());
  void registerFunc(std::string func_name, void(SimplePanelData::*)());
  void registerFunc(std::string func_name, void(StateMachine::*)());
  void registerFunc(std::string func_name, void(ProcInfo::*)());
  void registerFunc(std::string func_name, void(Columns::*)());
 
  // API which invokes appropriate object's method using information in action object 
  void invoke(Action);
  void invoke(Action, ScreenManager*);
  void invoke(Action, View*);
  void invoke(Action, SimplePanelData*);
  void invoke(Action, ProcInfo*);
  void invoke(Action, Columns*);
 
  // API which returns pointer to object given its uuid 
  View* getView(int);
  ScreenManager* getScreen(int);
  SimplePanelData* getPanel(int);
  StateMachine* getSm(int);
  Columns* getColumn(int);
  ProcDb* getDatabase(int);
  ProcInfo* getProcInfo(int);
 
  // API which registers an object and assigns a uuid to it 
  void addScreen(int, ScreenManager*);
  void addView(int, View*);
  void addPanel(int, SimplePanelData*);
  void addSm(int, StateMachine*);
  void addDatabase(int, ProcDb*);
  void addProcInfo(int, ProcInfo*);
  void addColumn(int, Columns*);

};

 //! registers ScreenManager class member methods 
 void MemFuncPDict::registerFunc(std::string func_name, void(ScreenManager::*pfunc)())
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::registerFunc_Screen::"; 
   funcp_void_screen[func_name] = pfunc;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<func_name; 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::registerFunc_Screen::"; 
   log_spacer.delSpace(); 
 }
 
 //! registers View class member methods 
 void MemFuncPDict::registerFunc(std::string func_name, void(View::*pfunc)())
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::registerFunc_View::"; 
   funcp_void_view[func_name] = pfunc;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<func_name; 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::registerFunc_View::"; 
   log_spacer.delSpace(); 
 }
 
 //! registers SimplePanelData class member methods 
 void MemFuncPDict::registerFunc(std::string func_name, void(SimplePanelData::*pfunc)())
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::registerFunc_SPData::"; 
   funcp_void_panel[func_name] = pfunc;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<func_name; 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::registerFunc_SPData::"; 
   log_spacer.delSpace(); 
 }

 //! registers StateMachine class member methods 
 void MemFuncPDict::registerFunc(std::string func_name, void(StateMachine::*pfunc)())
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::registerFunc_SM::"; 
   funcp_void_sm[func_name] = pfunc;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<func_name; 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::registerFunc_SM::"; 
   log_spacer.delSpace(); 
 }

 //! registers Columns  class member methods 
 void MemFuncPDict::registerFunc(std::string func_name, void(Columns::*pfunc)())
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::registerFunc_Columns::"; 
   funcp_void_clms[func_name] = pfunc;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<func_name; 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::registerFunc_Columns::"; 
   log_spacer.delSpace(); 
 }

 //! registers ProcInfo class member methods 
 void MemFuncPDict::registerFunc(std::string func_name, void(ProcInfo::*pfunc)())
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::registerFunc_PInfo::"; 
   funcp_void_pinfo[func_name] = pfunc;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<func_name; 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::registerFunc_PInfo::"; 
   log_spacer.delSpace(); 
 }

 //! returns pointer to ScreenManager object of given uuid 
 ScreenManager* MemFuncPDict::getScreen(int uuid)
 {
   if (screen_objs.find(uuid) != screen_objs.end())
     return screen_objs[uuid];
   else
     return NULL;
 }
 
 //! returns pointer to ProcDb object of given uuid 
 ProcDb* MemFuncPDict::getDatabase(int uuid=0)
 {
   if (db_objs.find(uuid) != db_objs.end())
    return db_objs[uuid];
   else
    return NULL;
 }

 //! returns pointer to Columns object of given uuid 
 Columns* MemFuncPDict::getColumn(int uuid=0)
 {
   if (clm_objs.find(uuid) != clm_objs.end())
    return clm_objs[uuid];
   else
    return NULL;
 }

 //! returns pointer to ProcInfo object of given uuid 
 ProcInfo* MemFuncPDict::getProcInfo(int uuid=0)
 {
   if (pinfo_objs.find(uuid) != pinfo_objs.end())
    return pinfo_objs[uuid];
   else
    return NULL;
 }

 //! returns pointer to View object of given uuid 
 View* MemFuncPDict::getView(int uuid)
 {
   if (view_objs.find(uuid) != view_objs.end())
     return view_objs[uuid];
   else
     return NULL;
 }

 //! returns pointer to SimplePanelData object of given uuid 
 SimplePanelData* MemFuncPDict::getPanel(int uuid)
 {
   if (panel_objs.find(uuid) != panel_objs.end())
     return panel_objs[uuid];
   else
     return NULL;
 }

 //! returns pointer to StateMachine object of given uuid 
 StateMachine* MemFuncPDict::getSm(int uuid)
 {
   if (sm_objs.find(uuid) != sm_objs.end())
     return sm_objs[uuid];
   else
     return NULL;
 }  

 //! adds ScreenManager object pointer to private object_type dictionary 
 void MemFuncPDict::addScreen(int uuid, ScreenManager* scr)
 {
   screen_objs[uuid] = scr;
 }
 
 //! adds ScreenManager object pointer to private object_type dictionary 
 void MemFuncPDict::addView(int uuid, View* view)
 {
   view_objs[uuid] = view;
 }

 //! adds SimplePanelData object pointer to private object_type dictionary 
 void MemFuncPDict::addPanel(int uuid, SimplePanelData* panel)
 {
   panel_objs[uuid] = panel;
 }

 //! adds StateMachine object pointer to private object_type dictionary 
 void MemFuncPDict::addSm(int uuid, StateMachine* sm)
 {
   sm_objs[uuid] = sm;
 }

 //! adds Columns object pointer to private object_type dictionary 
 void MemFuncPDict::addColumn(int uuid, Columns* clm)
 {
   clm_objs[uuid] = clm;
 }
 
 //! adds ProcInfo object pointer to private object_type dictionary 
 void MemFuncPDict::addProcInfo(int uuid, ProcInfo* pinfo)
 {
   pinfo_objs[uuid] = pinfo;
 }

 //! adds ProcDb object pointer to private object_type dictionary 
 void MemFuncPDict::addDatabase(int uuid, ProcDb* db)
 {
   db_objs[uuid] = db;
 }

 //! invokes ScreenManager member method based on supplied action  
 void MemFuncPDict::invoke(Action action, ScreenManager* pscreen)
 {
  if (funcp_void_screen.find(action.func_name) != funcp_void_screen.end())
  {
    log_spacer.addSpace();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::invoke-screen::";
    (pscreen->*funcp_void_screen[action.func_name])();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::invoke-screeni::";
    log_spacer.delSpace();
  }
 }
 
 //! invokes View member method based on supplied action  
 void MemFuncPDict::invoke(Action action, View* pview)
 {
  if (funcp_void_view.find(action.func_name) != funcp_void_view.end())
  {
    log_spacer.addSpace();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::invoke-view::";
    (pview->*funcp_void_view[action.func_name])();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::invoke-view::";
    log_spacer.delSpace();
  }
 }

 //! invokes SimplePanelData member method based on supplied action  
 void MemFuncPDict::invoke(Action action, SimplePanelData* ppanel)
 {
  if (funcp_void_panel.find(action.func_name) != funcp_void_panel.end())
  {
    log_spacer.addSpace(); 
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::invoke-panel::";
    (ppanel->*funcp_void_panel[action.func_name])();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict::invoke-panel::";
    log_spacer.delSpace();
  }
 }

 //! invokes ProcInfo member method based on supplied action  
 void MemFuncPDict::invoke(Action action, ProcInfo* pinfo)
 {
  if (funcp_void_pinfo.find(action.func_name) != funcp_void_pinfo.end())
  {
    log_spacer.addSpace(); 
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::invoke-pinfo::";
    (pinfo->*funcp_void_pinfo[action.func_name])();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::invoke-pinfo::";
    log_spacer.delSpace();
  }
 }

 //! invokes Columns member method based on supplied action  
 void MemFuncPDict::invoke(Action action, Columns* pclm)
 {
  
  if (funcp_void_clms.find(action.func_name) != funcp_void_clms.end())
  {
    log_spacer.addSpace();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::invoke-clm::";
    (pclm->*funcp_void_clms[action.func_name])();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict::invoke-clm::";
    log_spacer.delSpace();
  }
 }

 //! invokes member method based on supplied action using object_uuid and method_name  
 void MemFuncPDict::invoke(Action action)
 {
  log_spacer.addSpace();

  if (screen_objs.find(action.object_uuid) != screen_objs.end())
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict_invoke-screen_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   (screen_objs[action.object_uuid]->*funcp_void_screen[action.func_name])();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict_invoke-screen_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   log_spacer.delSpace();
   return;
  }
  
  if (view_objs.find(action.object_uuid) != view_objs.end())
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict_invoke-view_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   (view_objs[action.object_uuid]->*funcp_void_view[action.func_name])();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict_invoke-view_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   log_spacer.delSpace();
   return;
  }
  
  if (panel_objs.find(action.object_uuid) != panel_objs.end())
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict_invoke-panel_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   (panel_objs[action.object_uuid]->*funcp_void_panel[action.func_name])();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict_invoke-panel_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   log_spacer.delSpace();
   return;
  }
  
  if (sm_objs.find(action.object_uuid) != sm_objs.end())
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict_invoke-sm_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   (sm_objs[action.object_uuid]->*funcp_void_sm[action.func_name])();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict_invoke-sm_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   log_spacer.delSpace();
   return;
  }
  
  if (clm_objs.find(action.object_uuid) != clm_objs.end())
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict_invoke-clm_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   (clm_objs[action.object_uuid]->*funcp_void_clms[action.func_name])();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict_invoke-clm_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   log_spacer.delSpace();
   return;
  }
  
  if (pinfo_objs.find(action.object_uuid) != pinfo_objs.end())
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MemFuncPDict_invoke-pinfo_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   (pinfo_objs[action.object_uuid]->*funcp_void_pinfo[action.func_name])();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MemFuncPDict_invoke-pinfo_"<<action.object_uuid<<"_"<<action.func_name<<"::";
   log_spacer.delSpace();
   return;
  }
  log_spacer.delSpace();
 }

 //!
 //! MemFuncPDict global for allowing any object to invoke any other object's registered callbacks
 //! 
 MemFuncPDict funcPDict;

} // namespace rtop
#endif



