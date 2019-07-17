#include <vector>
#include <sstream>
#include <string>
#include <stdexcept>
#include "pugixml.hpp"

#ifndef _XML_H_
#define _XML_H_
#include "rtop_logger.h"
#include "columns.h" 

extern src::severity_logger<severity_level> lg;
//TODO: poorly written, make more understandabl by documentation and cleaning up logical flow
namespace rtop
{
 extern MapOfKeyDicts mapKeyDict;
 extern MemFuncPDict funcPDict;
 
 //! parses config file
 
 //!
 //! ```
 //! <Screen>
 //! <View></View>
 //! </Screen>
 //! ```
 //! 
 class XMLTree
 {
   private:
    pugi::xml_document doc;
    void createTreeFirstPass(pugi::xml_node node);
    void createPanel(int, int, std::string);
    pugi::xml_attribute getAttribute(pugi::xml_node, std::string);
    pugi::xml_node findNode(pugi::xml_node, int uuid);
    pugi::xml_node findNode(pugi::xml_node, std::string);
    
    void parseKeyDictionaries(pugi::xml_node);
    void parseCallbacks(pugi::xml_node);
    void parseColors(pugi::xml_node);
    void parseDatabase(pugi::xml_node);
    void parseProcInfo(pugi::xml_node);
    void parseColumn(pugi::xml_node);
      
    void updatePolymorphicPanel(BrowsePanelData<InfoProc<int>>*); 
    void updatePolymorphicPanel(BrowsePanelData<InfoProc<float>>*); 
    void updatePolymorphicPanel(BrowsePanelData<InfoProc<std::string>>*);
    void updatePolymorphicPanel(ProcViewPanel*);
    void updatePolymorphicPanel(BrowsePanelData<Info>*); 
 
    void updateObjFromAttrib(int, std::string, View*&);
    void updateObjFromAttrib(int, std::string, StateMachine&);
    void updateObjFromAttrib(int, std::string, SimplePanelData*&);
    void updateObjFromAttrib(int, std::string, int&, std::string, int&);
     
    void updateObjFromNodeVal(int, std::string, std::vector<InfoProc<std::string>*>&);
    void updateObjFromNodeVal(int, std::string, std::vector<InfoProc<int>*>&);
    void updateObjFromNodeVal(int, std::string, std::vector<InfoProc<float>*>&);
    void updateObjFromNodeVal(int, std::string, std::vector<Info*>&);
    void updateObjFromNodeVal(int, std::string, std::string, std::string, StateMachine&);
    void updateObjFromNodeVal(int, std::string, std::map<SimplePanelData*, std::pair<SimplePanelData*,SimplePanelData*>>&);
    void updateObjFromNodeVal(int, std::string, std::map<std::string, int>&, SimplePanelData*); 
 
    void updateViewPanelColors(int, std::string, View*); 
    //void updateCallbacks(pugi::xml_node, ScreenManager*); 
    void updateChildren(int, std::string, std::vector<View*> &);
    void updateChildren(int, std::string, std::vector<SimplePanelData*> &);
    void updateChildren(int, std::string, std::string, std::string, std::vector<std::pair<int, int>>&);

    //TODO: add dictionaries for data structures
   public:
    XMLTree(std::string);
    ScreenManager* instantiate();
 };
 
 XMLTree::XMLTree(std::string xmlfile)
 {
   doc.load_file(xmlfile.c_str());
   pugi::xml_node root_node;
   
   root_node = doc.child("Callbacks");
   parseCallbacks(root_node);
   
   root_node = doc.child("KeyDictionaries");
   parseKeyDictionaries(root_node);
  
   root_node = doc.child("Colors");
   parseColors(root_node);
  
   root_node = doc.child("ProcDb");
   parseDatabase(root_node);
 
   root_node = doc.child("Screen");
   createTreeFirstPass(root_node);
   
   root_node = doc.child("ProcInfo");
   parseProcInfo(root_node);
   
   root_node = doc.child("Column");
   parseColumn(root_node);
 }
 
 ScreenManager* XMLTree::instantiate()
 {
  ScreenManager* p_screen = funcPDict.getScreen(1);  // beware of constant, TODO: fix this
  updateObjFromAttrib(p_screen->uUid(), "currView", p_screen->currView);
  updateObjFromNodeVal(p_screen->uUid(), "StateMachine", "Type", "View", p_screen->smViews); 
  updateObjFromNodeVal(p_screen->uUid(), "StateMachine", "Type", "KeyDict", p_screen->smKeys); 
  updateChildren(p_screen->uUid(), "View", p_screen->views); 
  return p_screen; 
 }
 
 void XMLTree::parseColors(pugi::xml_node node)
 {
   StringParser parser{'\n'};
   pugi::xml_text txt_obj = node.text();
   parser<<txt_obj.get();
   std::vector<std::string> words = parser.Words();
   words.erase(words.begin());
   words.erase(words.end()-1);
   parser.setSeparator(',');
   for(auto word: words)
   {
    parser.clear();
    parser<<word;
    std::vector<std::string> colorstring = parser.Words();
    colors[std::stoi(colorstring[0])] = std::make_pair(color_map[colorstring[1]], color_map[colorstring[2]]); 
   }    
 }
 
 void XMLTree::parseDatabase(pugi::xml_node node)
 {
  pugi::xml_attribute attr = getAttribute(node, "Uuid");
  int uuid = std::stoi(attr.value()); 
  ProcDb* database = new ProcDb(uuid); 
  funcPDict.addDatabase(uuid, database);
  pugi::xml_node child_node = findNode(doc.child("ProcDb"), "FieldProperties");
  pugi::xml_text txt_obj = child_node.text();
  StringParser parser{'\n'};
  parser<<txt_obj.get();
  std::vector<std::string> words = parser.Words();
  words.erase(words.begin());
  words.erase(words.end()-1);
  parser.clear();
  parser.setSeparator(',');
  for(auto line: words)
  {
   parser<<line;
   int field = std::stoi((parser.Words()[0]));
   std::string prop = (parser.Words()[1]);
   std::string type = (parser.Words()[2]);
   database->field_prop_dict[field] = std::make_pair(prop, type);
   database->prop_type_dict[prop] = type; 
   parser.clear(); 
  }
  /* 
  parser.clear();
  parser.setSeparator('\n');  
  child_node = findNode(doc.child("ProcDb"), "PropType");
  txt_obj = child_node.text();
  parser<<txt_obj.get();
  words.clear();
  words = parser.Words();
  words.erase(words.begin());
  words.erase(words.end()-1);
  parser.clear();
  parser.setSeparator(',');
  for(auto line: words)
  {
   parser<<line;
   std::string prop = (parser.Words()[0]);
   std::string type = (parser.Words()[1]);
   database->prop_type_dict[prop] = type; 
   parser.clear(); 
  }
  */
 } 

 void XMLTree::parseProcInfo(pugi::xml_node node)
 {
  pugi::xml_attribute attr =  getAttribute(node, "Uuid");
  int uuid = std::stoi(attr.value());
  ProcInfo* pinfo = new ProcInfo(uuid);
  funcPDict.addProcInfo(uuid, pinfo);
  pinfo->sortkey = (getAttribute(node, "SortKey")).value();
  int db_uuid = std::stoi((getAttribute(node, "ProcDb")).value());
  pinfo->proc_database = funcPDict.getDatabase(db_uuid);
 }

 void XMLTree::parseColumn(pugi::xml_node node)
 {
  pugi::xml_attribute attr =  getAttribute(node, "Uuid");
  int uuid = std::stoi(attr.value());
  Columns* pclms = new Columns(uuid);
  funcPDict.addColumn(uuid, pclms);
  attr = getAttribute(node, "ProcInfo");
  int pinfo_uuid = std::stoi(attr.value()); 
  pclms->p_procinfo = funcPDict.getProcInfo(pinfo_uuid);
  attr = getAttribute(node, "ProcViewPanel");
  int viewp_uuid = std::stoi(attr.value());
  pclms->proc_view = dynamic_cast<ProcViewPanel*>(funcPDict.getPanel(viewp_uuid));
  pugi::xml_node child_node = findNode(node, "ProcViews");
  pugi::xml_text txt_obj = child_node.text();
  StringParser parser{','};
  parser<<txt_obj.get();
  std::vector<std::string> words;
  words = parser.Words();
  for(auto view_id: words)
   pclms->proc_views.push_back(std::stoi(view_id));
  pclms->pscreen = funcPDict.getScreen(1);
 }

   
 pugi::xml_attribute XMLTree::getAttribute(pugi::xml_node node, std::string attr_name)
 {
  pugi::xml_attribute attr; 
  for(attr=node.first_attribute(); attr; attr = attr.next_attribute())
  {
    if (strcmp(attr.name(), attr_name.c_str()) == 0)
      break;
  }
  return attr; 
 }

 void XMLTree::parseKeyDictionaries(pugi::xml_node root)
 {
  for(pugi::xml_node node_keydict=root.first_child(); node_keydict; node_keydict  = node_keydict.next_sibling()) // iterate over key dicts
  {
    int uuid = std::stoi((getAttribute(node_keydict, "Uuid").value()));
    KeyDict key_dict{uuid};
    StringParser parser{','};
    for(pugi::xml_node node_keyacts=node_keydict.first_child(); node_keyacts; node_keyacts = node_keyacts.next_sibling()) // iterate over key-actions
    {
       int key = std::stol((getAttribute(node_keyacts,"Key").value()), NULL, 0);
       std::vector<Action> vec_actions;  
       for(pugi::xml_node node_act=node_keyacts.first_child(); node_act; node_act = node_act.next_sibling())  // iterate over actions
       {
         
         parser<<(node_act.text()).get();
         std::vector<std::string> words = parser.Words();
         vec_actions.push_back(Action{std::stoi(words[0]), words[1]});
         parser.clear(); 
       }
       key_dict.addKeyActions(key, vec_actions);
       vec_actions.clear(); 
    }
    mapKeyDict.addKeyDict(uuid, key_dict); 
  }
 }
 
 void XMLTree::parseCallbacks(pugi::xml_node root)
 {
   StringParser parser{'\n'};
   pugi::xml_text txt_obj = root.text();
   parser<<txt_obj.get();
   std::vector<std::string> words = parser.Words();
   words.erase(words.begin());
   words.erase(words.end()-1);
   
   parser.clear();
   parser.setSeparator(',');
   for(auto word: words)
   {
     parser<<word;
     std::vector<std::string> obj_func_pair = parser.Words();
     if (obj_func_pair[0] == "Screen")
     {
       if (obj_func_pair[1] == "refresh") 
        funcPDict.registerFunc(obj_func_pair[1],&ScreenManager::refresh);
     }
     else if (obj_func_pair[0] == "View")
     {
     }
     else if (obj_func_pair[0] == "SimplePanelData")
     {
       if (obj_func_pair[1] == "Down") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::Down);
       if (obj_func_pair[1] == "Up") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::Up);
       if (obj_func_pair[1] == "editModeOn") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::editModeOn);
       if (obj_func_pair[1] == "editModeOff") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::editModeOff);
       if (obj_func_pair[1] == "insertIntoLeftNbr") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::insertIntoLeftNbr);
       if (obj_func_pair[1] == "insertIntoRightNbr") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::insertIntoRightNbr);
       if (obj_func_pair[1] == "addIntoLeftNbr") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::addIntoLeftNbr);
       if (obj_func_pair[1] == "addIntoRightNbr") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::addIntoRightNbr);
       if (obj_func_pair[1] == "moveIntoLeftNbr") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::moveIntoLeftNbr);
       if (obj_func_pair[1] == "moveIntoRightNbr") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::moveIntoRightNbr);
       if (obj_func_pair[1] == "toggleEditMode") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::toggleEditMode);
       if (obj_func_pair[1] == "deleteColumn") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::deleteColumn);
       if (obj_func_pair[1] == "insertColumn") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::insertColumn);
       if (obj_func_pair[1] == "moveLeft") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::moveLeft);
       if (obj_func_pair[1] == "moveRight") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::moveRight);
       if (obj_func_pair[1] == "refresh") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::refresh);
       if (obj_func_pair[1] == "remove") 
        funcPDict.registerFunc(obj_func_pair[1],&SimplePanelData::remove);

     }
     else if (obj_func_pair[0] == "Columns")
     {
       if(obj_func_pair[1] == "markPid")
        funcPDict.registerFunc(obj_func_pair[1], &Columns::markPid);
       if(obj_func_pair[1] == "read")
        funcPDict.registerFunc(obj_func_pair[1], &Columns::read);
     }
     else if(obj_func_pair[0] == "ProcInfo")
     {
       if(obj_func_pair[1] == "kill")
        funcPDict.registerFunc(obj_func_pair[1], &ProcInfo::kill);
     }
     else // statemachine 
     {
       if (obj_func_pair[1] == "next") 
        funcPDict.registerFunc(obj_func_pair[1],&StateMachine::next);
       if (obj_func_pair[1] == "stale") 
        funcPDict.registerFunc(obj_func_pair[1],&StateMachine::stale);

     }
     parser.clear();
   }    
 } 
 
 void XMLTree::createPanel(int uuid, int type, std::string container_type)
 {
   if(funcPDict.getPanel(uuid) == NULL) 
   {
     switch(type)
     {
       case 0:
        funcPDict.addPanel(uuid, new SimplePanelData(uuid));
        break;
       case 1:
        if (container_type == "inf_int")
          funcPDict.addPanel(uuid, new BrowsePanelData<InfoProc<int>>(uuid));
        else if (container_type == "inf_float")
          funcPDict.addPanel(uuid, new BrowsePanelData<InfoProc<float>>(uuid));
        else if (container_type == "inf_string")
          funcPDict.addPanel(uuid, new BrowsePanelData<InfoProc<std::string>>(uuid));
        else
          funcPDict.addPanel(uuid, new BrowsePanelData<Info>(uuid));
        break;
       case 2:
        if (container_type == "inf_int")
          funcPDict.addPanel(uuid, new EditPanelData<InfoProc<int>>(uuid));
        else if (container_type == "inf_float")
          funcPDict.addPanel(uuid, new EditPanelData<InfoProc<float>>(uuid));
        else if (container_type == "inf_string")
          funcPDict.addPanel(uuid, new EditPanelData<InfoProc<std::string>>(uuid));
        else
          funcPDict.addPanel(uuid, new EditPanelData<Info>(uuid));
        break;
       case 3:
         funcPDict.addPanel(uuid, new ProcViewPanel(uuid));
         break;
       default:
        break;
     }
     //funcPDict.addPanel(uuid, panel_dict[uuid]);
   } 
 }
 
 /* 
 depending on node type generate attributes. if node type is not one of screen, view or panel, do not create a new one, 
 instead treat them as properties. iterate over any child nodes and create each one recursively
 */ 
 void XMLTree::createTreeFirstPass(pugi::xml_node node)
 {
   // based on node's name, create a specific type and add to dictionary 
   if (strcmp(node.name(), "Screen") == 0)                       // Create Screen
   {
     if(pugi::xml_attribute attr = getAttribute(node, "Uuid"))
     {
       int uuid = std::stoi(attr.value());
       if (funcPDict.getScreen(uuid) == NULL)            // if object corresponding to uuid not already instantiated, proceed, else skip
       {
         funcPDict.addScreen(uuid, new ScreenManager(uuid));
       }
     }
     else
     {
       std::stringstream ss;
       ss<<"createTreeFirstPass: failed to find attribute Uuid in "<<node.name()<<"\n";
       throw std::runtime_error(ss.str());
     }
   }
   else if (strcmp((char*)node.name(), "View")==0)                  // Create View
   {
     if(pugi::xml_attribute attr = getAttribute(node, "Uuid"))
     {
       int uuid = std::stoi(attr.value());
       if (funcPDict.getView(uuid) == NULL)          // if object corresponding to uuid not already instantiated, proceed, else skip
       {
         funcPDict.addView(uuid, new View(uuid)); 
       }
     }
     else
     {
       std::stringstream ss;
       ss<<"createTreeFirstPass: failed to find attribute Uuid in "<<node.name()<<"\n";
       throw std::runtime_error(ss.str());
     }
   }
   else if (strcmp(node.name(), "Panel") == 0)                // Create Panel
   {
    if(pugi::xml_attribute attr = getAttribute(node, "Uuid"))
     {
       int uuid = std::stoi(attr.value());
       if(pugi::xml_attribute attr = getAttribute(node, "Type"))
       {
         int type = std::stoi(attr.value());
         if(pugi::xml_attribute attr = getAttribute(node, "ContainerData"))
           createPanel(uuid, type, attr.value());
         else
          throw std::runtime_error("failed to find ContainerData in Panel\n");
       }
       else
         throw std::runtime_error("failed to find Type in Panel\n");
     }
     else
       throw std::runtime_error("failed to find UUID in Screen \n");
   }
   else if (strcmp(node.name(), "ColumnsPanelData") == 0)      // Create Columns Panel 
   {
     //TODO
   }
   else                                            // Base case   
   {
     return; // ignore nodes carrying attribute values
   } 
  
   // create all child node 
   for(pugi::xml_node child_node = node.first_child(); child_node; child_node = child_node.next_sibling())
     createTreeFirstPass(child_node);
 }
 
 pugi::xml_node XMLTree::findNode(pugi::xml_node node, int uuid)
 {
   
  if (pugi::xml_attribute attr=getAttribute(node, "Uuid"))
  {
    if(std::stoi(attr.value()) == uuid)
      return node;
    else
    {
      for(pugi::xml_node child=node.first_child(); child; child=child.next_sibling())
      {
        if (pugi::xml_node return_node = findNode(child, uuid))
          return return_node;
        else
         continue;
      }
    }
  }
  return pugi::xml_node();  
 }

 pugi::xml_node XMLTree::findNode(pugi::xml_node node, std::string name)
 {
  if (strcmp(name.c_str(), node.name()) == 0)
   return node;
  else
  { 
   for(pugi::xml_node child=node.first_child(); child; child=child.next_sibling())
   { 
    if (pugi::xml_node return_node = findNode(child, name))
     return return_node;
    else
     continue;
   }
  }
  return pugi::xml_node(); 
 }

 void XMLTree::updateObjFromAttrib(int uuid, std::string attr_name, View*& currView)
 {
   pugi::xml_node node = findNode(doc.child("Screen"), uuid);  // find node corresponding to the uuid
   if (!node)                                                // if node not found, throw exception with message
   {
    std::stringstream ss;
    ss<<"updateObjFromAttrib: node corresponding to uuid "<<uuid<<" not found \n";
    throw std::runtime_error(ss.str());
   }                                                         
   if(pugi::xml_attribute attr=getAttribute(node, attr_name)) // find atttribute in the node and set the parameter after appropriate conversion
     currView = funcPDict.getView(std::stoi(attr.value()));
     //currView = view_dict[std::stoi(attr.value())];
   else
   {
     std::stringstream ss;
     ss<<"failed to find "<<attr_name<<" in object "<<uuid<<"\n";
     throw std::runtime_error(ss.str()); 
   }
 }

 void XMLTree::updateObjFromAttrib(int uuid, std::string attr_name, SimplePanelData*& currPanel)
 {
   pugi::xml_node node = findNode(doc.child("Screen"), uuid);  // find node corresponding to the uuid
   if (!node)                                                // if node not found, throw exception with message
   {
    std::stringstream ss;
    ss<<"updateObjFromAttrib: node corresponding to uuid "<<uuid<<" not found \n";
    throw std::runtime_error(ss.str());
   }                                                         
   if(pugi::xml_attribute attr=getAttribute(node, attr_name)) // find atttribute in the node and set the parameter after appropriate conversion
     //currPanel = panel_dict[std::stoi(attr.value())];
     currPanel = funcPDict.getPanel(std::stoi(attr.value()));
   else
   {
     std::stringstream ss;
     ss<<"failed to find "<<attr_name<<" in object "<<uuid<<"\n";
     throw std::runtime_error(ss.str()); 
   }
 }

 void XMLTree::updateObjFromAttrib(int uuid, std::string attr1, int& attr1_val, std::string attr2, int& attr2_val)
 {
   pugi::xml_node node = findNode(doc.child("Screen"), uuid);  // find node corresponding to the uuid
   if (!node)                                                // if node not found, throw exception with message
   {
    std::stringstream ss;
    ss<<"updateObjFromAttrib: node corresponding to uuid "<<uuid<<" not found \n";
    throw std::runtime_error(ss.str());
   }                                                         
   
   if(pugi::xml_attribute attr=getAttribute(node, attr1)) // find atttribute in the node and set the parameter after appropriate conversion
     attr1_val = std::stoi(attr.value());
   else
   {
     std::stringstream ss;
     ss<<"failed to find "<<attr1<<" in object "<<uuid<<"\n";
     throw std::runtime_error(ss.str()); 
   }
   
   if(pugi::xml_attribute attr=getAttribute(node, attr2)) // find atttribute in the node and set the parameter after appropriate conversion
     attr2_val = std::stoi(attr.value());
   else
   {
     std::stringstream ss;
     ss<<"failed to find "<<attr2<<" in object "<<uuid<<"\n";
     throw std::runtime_error(ss.str()); 
   }

 } 
 
 void XMLTree::updateObjFromNodeVal(int uuid, std::string child_name, std::vector<InfoProc<std::string>*>& object_list)
 {
   pugi::xml_node root  = findNode(doc.child("Screen"), uuid);
   pugi::xml_node child = findNode(root, child_name);
   pugi::xml_text txt_obj = child.text();
   if (txt_obj.empty())
   { 
     std::stringstream ss;
     ss<<"failure to read object list from panel "<<uuid<<", empty list\n";
     throw std::runtime_error(ss.str());
   }
   pugi::xml_attribute attr = getAttribute(child, "Separator");
   int capacity = std::stoi((getAttribute(child, "Capacity").value()));
   StringParser parser{toChar(attr.value())};
   parser<<txt_obj.get();
   object_list.reserve(capacity);
   for(auto word: parser.Words())
     object_list.push_back(new InfoProc<std::string>(word));
     
 }

 void XMLTree::updateObjFromNodeVal(int uuid, std::string child_name, std::vector<InfoProc<float>*>& object_list)
 {
 }

 void XMLTree::updateObjFromNodeVal(int uuid, std::string child_name, std::vector<InfoProc<int>*>& object_list)
 {
 }
 
 void XMLTree::updateObjFromNodeVal(int uuid, std::string child_name, std::vector<Info*>& object_list)
 {
  pugi::xml_node root  = findNode(doc.child("Screen"), uuid);
  pugi::xml_node child = findNode(root, child_name);
  pugi::xml_text txt_obj = child.text();
  if (txt_obj.empty())
  { 
    std::stringstream ss;
    ss<<"failure to read object list from panel "<<uuid<<", empty list\n";
    throw std::runtime_error(ss.str());
  }
  pugi::xml_attribute attr = getAttribute(child, "Separator");
  int capacity = std::stoi((getAttribute(child, "Capacity").value()));

  StringParser parser{toChar(attr.value())};
  parser<<txt_obj.get();
  std::vector<std::string> words = parser.Words();
  words.erase(words.begin());
  words.erase(words.end()-1); 
  parser.clear();
  parser.setSeparator(','); 
  object_list.reserve(capacity);
  for(auto word: words)
  {
     parser<<word;
     std::vector<std::string> info = parser.Words();
     object_list.push_back(new Info(info[0], info[1], info[2]));
     parser.clear();
  } 
 }

 void XMLTree::updateObjFromNodeVal(int uuid, std::string child_name, std::string child_attr, std::string child_attr_val, StateMachine& sm)
 {
  pugi::xml_node root  = findNode(doc.child("Screen"), uuid);
  pugi::xml_node_iterator it;
  for(it=root.begin(); it != root.end(); it++)
  {
    if (strcmp(it->name(), child_name.c_str()) == 0) // if child is a state machine node
    {
      pugi::xml_attribute attr = getAttribute(*it, child_attr);
      if (strcmp(attr.value(), child_attr_val.c_str())==0)
        break;
    }
  }
  if (it == root.end())
   return;
  // register State Machine with funcPDict
  pugi::xml_attribute attr = getAttribute(*it, "Uuid");
  int sm_uuid = std::stoi(attr.value());
  funcPDict.addSm(sm_uuid, &sm);         
  // populate state machine 
  attr = getAttribute(*it, "CurrState");
  sm.setCurrState(std::stoi(attr.value()));
  attr = getAttribute(*it, "Separator");
  StringParser parser{toChar(attr.value())};

  pugi::xml_node child = findNode(*it, "Transitions");
  pugi::xml_text txt_obj = child.text();
  if (txt_obj.empty())
  { 
    std::stringstream ss;
    ss<<"failure to read object list from panel "<<uuid<<", empty list\n";
    throw std::runtime_error(ss.str());
  }
    parser<<txt_obj.get();
  std::vector<std::string> words = parser.Words();
  words.erase(words.begin());
  words.erase(words.end()-1); 
  parser.clear();
  parser.setSeparator(','); 
  for(auto word: words)
  {
     parser<<word;
     std::vector<std::string> transn = parser.Words();
     sm.addTransition(std::stol(transn[1],NULL,0),std::stoi(transn[0],NULL,0),std::stoi(transn[2],NULL,0));	
     parser.clear();
  } 
 
 }
 
 void XMLTree::updateObjFromNodeVal(int uuid, std::string child_name, std::map<SimplePanelData*, std::pair<SimplePanelData*, SimplePanelData*>>& nbrs)
 {
   pugi::xml_node view_node = findNode(doc.child("Screen"), uuid);
   pugi::xml_node child_node;
   for(child_node = view_node.first_child(); child_node; child_node=child_node.next_sibling())
   {
     if(strcmp(child_node.name(), (child_name).c_str())==0)
      break;
   }
   if (!child_node)
    return;
   pugi::xml_text txt_obj = child_node.text();
   StringParser parser{'\n'};
   parser<<txt_obj.get();
   std::vector<std::string> nbr_lines = parser.Words();
   nbr_lines.erase(nbr_lines.begin());
   nbr_lines.erase(nbr_lines.end()-1);
   parser.setSeparator(',');
   parser.clear();
   //parse each line of neighbourhood 
   std::vector<std::string> panels_uuids;
   SimplePanelData* leftNbr, *rightNbr;
   for(auto word: nbr_lines)
   {
    parser<<word;
    panels_uuids = parser.Words();
    // if panel id for left/right nbr is 0, nbr will be set as NULL
    leftNbr = funcPDict.getPanel(std::stoi(panels_uuids[1]));
    rightNbr = funcPDict.getPanel(std::stoi(panels_uuids[2]));
    nbrs[funcPDict.getPanel(std::stoi(panels_uuids[0]))] = std::make_pair(leftNbr, rightNbr);
    panels_uuids.clear();
    parser.clear(); 
   } 
 }
 
 void XMLTree::updateViewPanelColors(int uuid, std::string child_name, View* view)
 {
   pugi::xml_node view_node = findNode(doc.child("Screen"), uuid);
   pugi::xml_node child_node;
   for(child_node = view_node.first_child(); child_node; child_node = child_node.next_sibling())
   {
     if (strcmp(child_node.name(), (child_name).c_str()) == 0)
      break;
   }
   if (!child_node)
    return;

   // now parse child node (PanelColors), for color pairs
   for(pugi::xml_node color_node = child_node.first_child(); color_node; color_node = color_node.next_sibling())
   {
     if (strcmp(color_node.name(), "Editing") == 0)
      view->editing_color_pair = std::stoi((color_node.text()).get());
     if (strcmp(color_node.name(), "Selection") == 0)
      view->selection_color_pair = std::stoi((color_node.text()).get());
     if (strcmp(color_node.name(), "Header") == 0)
      view->header_color_pair = std::stoi((color_node.text()).get());
   } 
 }


 void XMLTree::updateChildren(int uuid,std::string main_attrib, std::string sub_attrib1, std::string sub_attrib2, std::vector<std::pair<int, int>>& panels_yx)
 {
   pugi::xml_node view_node = findNode(doc.child("Screen"), uuid);
   pugi::xml_node positions_node = findNode(view_node, main_attrib);
   int n_children = 0; 
   for(pugi::xml_node pos_node=positions_node.first_child(); pos_node; pos_node=pos_node.next_sibling())
    n_children++;
   panels_yx.reserve(n_children); 
   for(pugi::xml_node pos_node=positions_node.first_child(); pos_node; pos_node=pos_node.next_sibling())
   {
     std::pair<int, int> temp;
     temp.first = std::stoi((getAttribute(pos_node,sub_attrib1)).value());
     temp.second = std::stoi((getAttribute(pos_node,sub_attrib2)).value());
     panels_yx.push_back(temp);
   }
 } 

 void XMLTree::updateChildren(int uuid, std::string child_name, std::vector<View*> & views)
 {
  pugi::xml_node root = findNode(doc.child("Screen"), uuid);  //find the root node for the sub-tree corresponding to uuid 
  int n_children = 0;
  for(pugi::xml_node child=root.first_child(); child; child = child.next_sibling()) // count the number of children
    n_children++;
  views.reserve(n_children);                               // reserve vector capacity to proprely store pointers
  
  View* pview; 
  for(pugi::xml_node child=root.first_child(); child; child = child.next_sibling())  //  
  {
    if (strcmp((char*)child.name(), "View") != 0)  // only if child node is view, continue, else remaining body of loop
      continue; 
    pugi::xml_attribute attr=getAttribute(child, "Uuid");

    int uuid = std::stoi(attr.value());    // view's uuid
    pview = funcPDict.getView(uuid);
    pview->win = newwin(40, 140, 0, 0);
    // populate view state machines
    updateObjFromNodeVal(uuid, "StateMachine", "Type", "Panel",pview->smPanels); 
    updateObjFromNodeVal(uuid, "StateMachine", "Type", "KeyDict",pview->smKeys);
    updateObjFromNodeVal(uuid, "Neighbourhood", pview->neighbourhood);
    // populate view's color pairs 
    updateViewPanelColors(uuid, "PanelColors", pview);
     
     
    updateObjFromAttrib(uuid, "currPanel", pview->curr_panel); 
    updateChildren(uuid, "PanelPositions", "YPos", "XPos", pview->panels_yx); 
    updateChildren(uuid, "Panel", pview->panels);
    // set panel colors
    for(auto panel: pview->panels)
    {
      panel->setBasicColor(pview->basic_color_pair);
      panel->setSelectionColor(pview->selection_color_pair);
      panel->setEditingColor(pview->editing_color_pair);
      panel->setHeaderColor(pview->header_color_pair);
    } 
    for(auto panel:pview->panels)
     panel->syncColors(); 
    for(int i=0; i<pview->panels_yx.size(); i++)
      pview->panels[i]->moveTo(pview->panels_yx[i].first, pview->panels_yx[i].second); // can be optimized
    views.push_back(pview);
  }
 }
 
 void XMLTree::updateChildren(int uuid, std::string child_name, std::vector<SimplePanelData*> & panels)
 {
  int root_uuid = uuid; 
  pugi::xml_node root = findNode(doc.child("Screen"), uuid);  //find the root node for the sub-tree corresponding to uuid 
  int n_children = 0;
  for(pugi::xml_node child=root.first_child(); child; child = child.next_sibling()) // count the number of children
    n_children++;
  panels.reserve(n_children);                               // reserve vector capacity to proprely store pointers
  int count = 0;
  SimplePanelData* ppanel;
  View* pview;
  for(pugi::xml_node child=root.first_child(); child; child = child.next_sibling())  //  
  {
    if (strcmp((char*)child.name(), "Panel") != 0)
      continue; 
    pugi::xml_attribute attr=getAttribute(child, "Uuid");
    int uuid = std::stoi(attr.value());
    pugi::xml_attribute container=getAttribute(child, "ContainerData");
    ppanel = funcPDict.getPanel(uuid);
  
    // base class initialization
    pview = funcPDict.getView(root_uuid); 
    updateObjFromAttrib(uuid, "Width", ppanel->width, "Height",ppanel->height);
    // set header
    ppanel->setHeader((getAttribute(child, "Header")).value());
 
    BOOST_LOG_SEV(lg, info)<<"Panel: "<<uuid<<" Ht: "<<ppanel->height<<" Wd: "<<ppanel->width;
    ppanel->win = derwin(pview->win, ppanel->height, ppanel->width, pview->panels_yx[count].first, pview->panels_yx[count].second);
    ppanel->panel = new_panel(ppanel->win);
    updateObjFromNodeVal(uuid, "StateMachine", "Type", "KeyDict",ppanel->smKeys);
 
    if (container.value() == "info_int")
      updatePolymorphicPanel(dynamic_cast<BrowsePanelData<InfoProc<int>>*>(ppanel)); 
    else if (container.value() == "info_float")
      updatePolymorphicPanel(dynamic_cast<BrowsePanelData<InfoProc<float>>*>(ppanel)); 
    else if (container.value() == "info_string") 
      updatePolymorphicPanel(dynamic_cast<BrowsePanelData<InfoProc<std::string>>*>(ppanel));
    else if (strcmp(container.value(),"panels")==0)
      updatePolymorphicPanel(dynamic_cast<ProcViewPanel*>(ppanel)); 
    else
      updatePolymorphicPanel(dynamic_cast<BrowsePanelData<Info>*>(ppanel));
    panels.push_back(ppanel);
    count++; 
  } 
 } 
 
void XMLTree::updateObjFromNodeVal(int uuid, std::string child_name, std::map<std::string, int>& dict, SimplePanelData* main_panel)
{
 pugi::xml_node child_node = findNode(doc.child("Screen"), uuid);
 pugi::xml_node prop_node = findNode(child_node, "Columns");
 StringParser parser{'\n'};
 pugi::xml_text txt_obj = prop_node.text();
 parser<<txt_obj.get();
 std::vector<std::string> lines = parser.Words();
 lines.erase(lines.begin());
 lines.erase(lines.end()-1);
 parser.clear();
 parser.setSeparator(',');
 int panel_xpos = 0;
 for(auto line: lines)
 {
  parser<<line;
  std::string clm_prop_name = (parser.Words())[0];
  int clm_width = std::stoi((parser.Words())[1]);
  dict[clm_prop_name] = clm_width; 
  parser.clear();
 }
}  

void XMLTree::updatePolymorphicPanel(ProcViewPanel* pPanel)
{ 
  updateObjFromNodeVal(pPanel->uuid, "Columns", pPanel->prop_wid_dict, pPanel);
  pugi::xml_node proc_node = findNode(doc.child("Screen"), pPanel->uuid);
  for(pugi::xml_node child_node = proc_node.first_child(); child_node; child_node = child_node.next_sibling())
  {
   if(strcmp(child_node.name(), "ActiveProc") == 0)
   {
     pugi::xml_text txt_obj = child_node.text();
     pPanel->active_proc_panel = funcPDict.getPanel(std::stoi(txt_obj.get()));

   }
   if(strcmp(child_node.name(), "ProcDb") == 0)
   {
     pugi::xml_text txt_obj = child_node.text();
     pPanel->proc_database = funcPDict.getDatabase(std::stoi(txt_obj.get()));
   }
   if(strcmp(child_node.name(), "SortKey") == 0)
   {
     pugi::xml_text txt_obj = child_node.text();
     pPanel->sort_key= txt_obj.get();
   }
  }
}

 void XMLTree::updatePolymorphicPanel(BrowsePanelData<InfoProc<int>>* pPanel)
 {
      
   if (dynamic_cast<BrowsePanelData<InfoProc<int>>*>(pPanel) != NULL)
    updateObjFromNodeVal(pPanel->uuid, "ObjectList", pPanel->object_list);
   
   if (dynamic_cast<EditPanelData<InfoProc<int>>*>(pPanel))
    (dynamic_cast<EditPanelData<InfoProc<int>>*>(pPanel))->editing = false;
 }

 void XMLTree::updatePolymorphicPanel(BrowsePanelData<InfoProc<float>>* pPanel)
 {
   
   if (dynamic_cast<BrowsePanelData<InfoProc<float>>*>(pPanel) != NULL)
    updateObjFromNodeVal(pPanel->uuid, "ObjectList", pPanel->object_list);
  
   if (dynamic_cast<EditPanelData<InfoProc<float>>*>(pPanel))
    (dynamic_cast<EditPanelData<InfoProc<float>>*>(pPanel))->editing = false;
 } 
  
 void XMLTree::updatePolymorphicPanel(BrowsePanelData<InfoProc<std::string>>* pPanel)
 {
   
   // If edit panel can be cast as  
   if (dynamic_cast<BrowsePanelData<InfoProc<std::string>>*>(pPanel) != NULL)
   {
    BOOST_LOG_SEV(lg, debug)<<"--> XMLTree::updatePoly-Browse-InfoPorc-string";
    updateObjFromNodeVal(pPanel->uuid, "ObjectList", pPanel->object_list);
    pPanel->selected = pPanel->object_list.size(); 
    pPanel->refreshItemList();
   }
  
   if (dynamic_cast<EditPanelData<InfoProc<std::string>>*>(pPanel))
    (dynamic_cast<EditPanelData<InfoProc<std::string>>*>(pPanel))->editing = false;
 
 } 

 void XMLTree::updatePolymorphicPanel(BrowsePanelData<Info>* pPanel)
 {
   if (dynamic_cast<BrowsePanelData<Info>*>(pPanel))
   {
    updateObjFromNodeVal(pPanel->uuid, "ObjectList", pPanel->object_list);
    pPanel->selected = pPanel->object_list.size(); 
    pPanel->refreshItemList();
   } 
   if (dynamic_cast<EditPanelData<Info>*>(pPanel))
    (dynamic_cast<EditPanelData<Info>*>(pPanel))->editing = false;
 } 

 
} // namespace rtop

#endif // _XML_H_
