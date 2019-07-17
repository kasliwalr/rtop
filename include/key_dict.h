#include <map>
#include <vector>
#include <string>

#ifndef KEY_H_
#define KEY_H_
#include "rtop_logger.h"
#include "mem_func.h"
#include "screen2.h"
#include "view2.h"
#include "panel2.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
 class ScreenManager;
 class View;
 class SimplePanelData;
 
 extern MemFuncPDict funcPDict;

 //! holds a dictionary of key values and corresponding Action list. Provides API to resolve the keys
 class KeyDict
 {
   private: 
    int key_dict_uuid;
    std::map<int, std::vector<Action>> key_actions_dict;
   public:
    KeyDict(int id): key_dict_uuid{id}{}
    KeyDict(): key_dict_uuid{-1}{}
    void setId(int id){key_dict_uuid = id;}
    int getId() const {return key_dict_uuid;}
    void resolve(int);
    void resolve(int, ScreenManager*);
    void resolve(int, View*);
    void resolve(int, SimplePanelData*);
    void addKeyActions(int, std::vector<Action>&);
 };

 //! resolves the key i.e. based on key input invokes the associated list of Action objects 
 void KeyDict::resolve(int key)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> KeyDict-"<<key_dict_uuid<<"_resolve::";
   if (key_actions_dict.find(key) != key_actions_dict.end()) // if key is found in dictionary 
   {
     for(auto action: key_actions_dict[key])                 // iterate over action list
      funcPDict.invoke(action);                              // pass action to callback database (MemFuncPDict) for invocation
   }
   else                                                      // if key not found in dictionary, invoke action list corresponding to key = -1 
   {
     for(auto action: key_actions_dict[-1])                  
      funcPDict.invoke(action);                              
   }
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- KeyDict-"<<key_dict_uuid<<"_resolve::";
   log_spacer.delSpace();
 } 

 //! resolves keys with Action lists corresponding to self
 void KeyDict::resolve(int key, ScreenManager* pscreen)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> KeyDict-"<<key_dict_uuid<<"_resolve-screen::";
  if (key_actions_dict.find(key) != key_actions_dict.end())
  {
    for(auto action: key_actions_dict[key])   // if key is found in dictionary 
    {
      if(action.object_uuid == -1)            // if object id is -1, invoke method for passed ScreenManager object pointer
        funcPDict.invoke(action, pscreen);
      else
       funcPDict.invoke(action);              // if object id is >0, invoke methods for by passing action info to callback database 
    }
  }
  else                                        // if key is not found in dictionary, invoke action list corresponding to key=-1 
  {
   for(auto action: key_actions_dict[-1])
     funcPDict.invoke(action);
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- KeyDict-"<<key_dict_uuid<<"_resolve-screen::";
  log_spacer.delSpace();
 }

 //! resolves keys with action list corresponding to self. \todo: not implemented. implement 
 void KeyDict::resolve(int key, View* pview)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> KeyDict-"<<key_dict_uuid<<"_resolve-view::";
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- KeyDict-"<<key_dict_uuid<<"_resolve-view::";
  log_spacer.delSpace();
 }

 //! resolves keys with Action list corresponding to self
 void KeyDict::resolve(int key, SimplePanelData* ppanel)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> KeyDict-"<<key_dict_uuid<<"_resolve-panel::";
  
  if (key_actions_dict.find(key) != key_actions_dict.end())
  {
    for(auto action: key_actions_dict[key])    // if key is found in dictionary
    {
      if(action.object_uuid == -1)             // if object id is -1, invoke method for passed panel data structure object pointer
        funcPDict.invoke(action, ppanel);
      else
        funcPDict.invoke(action);              // if object id is >0, invoke methods for by passing action info to callback database 
    }
  }
  else                                         // if key is not found in dictionary, invoke action list corresponding to key=-1 
 
  {
   for(auto action: key_actions_dict[-1])
     funcPDict.invoke(action);
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- KeyDict-"<<key_dict_uuid<<"_resolve-panel::";
  log_spacer.delSpace();
 }

 //! add key and corresponding action list to private key-action_list dictionary 
 void KeyDict::addKeyActions(int key, std::vector<Action>& actions)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> KeyDict-"<<key_dict_uuid<<"_addKeyActions::";
   if (key_actions_dict.find(key) != key_actions_dict.end()) // if key is already present, reset its action list
     key_actions_dict[key].clear();
   key_actions_dict[key] = actions;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- KeyDict-"<<key_dict_uuid<<"_addKeyActions::";
   log_spacer.delSpace();
 } 


 //! stores key dictionary index by key_dict_uuid
 class MapOfKeyDicts
 {
  private:
   std::map<int, KeyDict> map_of_keydict;
  public:
   MapOfKeyDicts(){}
   void addKeyDict(int, KeyDict);
   KeyDict getKeyDict(int);
 };

 //! adds key dictionary corresponding to the supplied key_dict_uuid
 void MapOfKeyDicts::addKeyDict(int key_uuid, KeyDict key_dict)
 {
  log_spacer.addSpace(); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MapOfKeyDicts_addKeyDict::";
  map_of_keydict[key_uuid] = key_dict;
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MapOfKeyDicts_addKeyDict::";
  log_spacer.delSpace();
 }

 KeyDict MapOfKeyDicts::getKeyDict(int key_uuid)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> MapOfKeyDicts_getKeyDict::";
   if (map_of_keydict.find(key_uuid) != map_of_keydict.end())  // if key_dict_uuid found, return the corresponding entry
   {
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MapOfKeyDicts_getKeyDict::";
     log_spacer.delSpace();
     return map_of_keydict[key_uuid];
   }
   else                                                        // if key_dict_uuid not found, return NULL
   {
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MapOfKeyDicts_getKeyDict::";
     log_spacer.delSpace();
     return NULL;
   } 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- MapOfKeyDicts_getKeyDict::";
   log_spacer.delSpace();
 } 

 //! global variable for accessing key dictionaries by their uuid 
 MapOfKeyDicts mapKeyDict;
  
} // namespace rtop
#endif



