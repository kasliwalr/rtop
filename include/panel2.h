#include <errno.h>
#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>

#ifndef _PANEL_H_
#define _PANEL_H_
#include "rtop_logger.h"
#include "rtop_utils.h"
#include "info.h"
#include "key_dict.h"
#include "sm.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
 extern MapOfKeyDicts mapKeyDict;
 extern MemFuncPDict funcPDict;

 //! Base class in heirarchy of UI objects responsible to storing the content associated with a panel and providing an API to manipulate it

 //! 
 //! encapsulates the idea of a panel which is a square region on the screen.
 //! Its API allows moving this region, setting its neighbours and specifying its size and changing its visibility
 //!     
 class SimplePanelData
 {
  protected:
    int uuid;
    //! width and height in panel window in ncurses units.  
    int width, height;
    //! panel to which the data structure is associated
    PANEL* panel;
    //! window attached to the panel
    WINDOW* win;
    //! pointer to panel data structures correponding to left/right neighbour of this panel
    SimplePanelData* r_next, *l_next;
    //! Keys statemachine
    StateMachine smKeys;
  public:
    //! SimplePanelData constructor
    SimplePanelData(int);
    
    // implemented in SimplePanelData
    //! returns unique identifier of this object
    int uUid() const {return uuid;}
    //! sets reference to ncurses WINDOW associated with panel
    void setWin(WINDOW* win_in){win = win_in;}
    //! sets reference to ncurses PANEL associated with this panel data structure
    void setPanel(PANEL* panel_in){panel = panel_in;}
    //! set internal width, height variables corresponding to panel window dimensions
    void setDimensions(int h, int w);
    //! set pointer to left neighbour panel
    void setLeftNbr(SimplePanelData*);
    //! set pointer to right neighbour panel
    void setRightNbr(SimplePanelData*);
    //! returns pointer to left neighbour panel
    SimplePanelData* leftNbr();
    //! returns pointer to right neighbour panel
    SimplePanelData* rightNbr();
    //! move panel window to desired x,y pos on stdscr. \todo: may cause bugs. investigate
    void moveTo(int, int);
    //! refreshes contents of associated BrowsePanel data structure and makes panel visible  
    virtual void show();
    //! makes panel visible
    virtual void show2();
    //! hides panel
    virtual void hide();
    //! returns pointer to ncurses WINDOW associated with panel
    WINDOW* window() const{return win;} 
 
    // mock implementations, to enable polymorphic access to Browse/Edit PanelData through SimplePanelData pointer
    
    //! mock 
    virtual void selectionOn(){}
    //! mock
    virtual void selectionOff(){}
    //! mock
    virtual void toggleEditMode(){} 
    //! mock
    virtual void editModeOn(){}
    //! mock
    virtual void editModeOff(){} 
    //! mock
    virtual void setEditingColor(int){}
    //! mock
    virtual void setSelectionColor(int){}
    //! mock
    virtual void setBasicColor(int){}
    //! mock 
    virtual void setHeaderColor(int){}
    //! mock 
    virtual void setHeader(std::string){}
    //! mock
    virtual void Down(){}
    //! mock
    virtual void Up(){}
    //! mock 
    virtual void Top(){}
    //! mock 
    virtual void refreshItemList(){}
    //! mock 
    virtual void refreshItemList2(std::vector<InfoProc<std::string>>&, std::string, const std::vector<int>&, int&){}
    //! mock 
    virtual void resolveKey(int){};
    //! mock 
    virtual void moveIntoLeftNbr(){}
    //! mock 
    virtual void moveIntoRightNbr(){}
    //! mock 
    virtual void insertIntoLeftNbr(){}
    //! mock 
    virtual void insertIntoRightNbr(){}
    //! mock 
    virtual void addIntoLeftNbr(){}
    //! mock 
    virtual void addIntoRightNbr(){}
    //! mock 
    virtual void remove(){}
    
    // ProcViewPanel's methods
    
    //! mock 
    virtual std::vector<std::string> displayedKeys() const{}
    //! mock
    virtual void deleteColumn(){}
    //! mock
    virtual void insertColumn(){}
    //! mock
    virtual void refresh(){}
    //! mock
    virtual void moveLeft(){}
    //! mock
    virtual void moveRight(){}  
    //! mock
    virtual void syncColors(){}    
    friend class XMLTree;
 };

 SimplePanelData::SimplePanelData(int id)
 {
  uuid = id;
 }
 
 void SimplePanelData::setDimensions(int h, int w)
 {
  height = h;
  width = w;
 }

 void SimplePanelData::setLeftNbr(SimplePanelData* panel)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_setLeftNbr::";
  l_next = panel;
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_setLeftNbr::";
  log_spacer.delSpace();
 }
 
 void SimplePanelData:: setRightNbr(SimplePanelData* panel)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_setRightNbr::";
  r_next = panel;
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_setRightNbr::";
  log_spacer.delSpace();
 }
 
 SimplePanelData* SimplePanelData::leftNbr()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_leftNbr::";
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_leftNbr::";
   log_spacer.delSpace();
   return l_next;
 }
 
 SimplePanelData* SimplePanelData::rightNbr()
 {
   log_spacer.addSpace(); 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_rightNbr::";
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_rightNbr::";
   log_spacer.delSpace();
   return r_next;
 }
 
 void SimplePanelData::moveTo(int y, int x)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_moveTo::x="<<x<<" y="<<y;
  move_panel(panel, y, x);
  doupdate();
  update_panels();
  BOOST_LOG_SEV(lg,debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_moveTo::";
  log_spacer.delSpace();
 }
 
 void SimplePanelData::show()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_show::";
   refreshItemList();
   //! \bug: when new prop is added to active_proc_panel and deletion is attemped, seg fault occurs at show_panel. 
   show_panel(panel);
   update_panels();
   doupdate();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_show::";
   log_spacer.delSpace();
 }
 
 void SimplePanelData::show2()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_show2::";
  show_panel(panel);
  update_panels();
  doupdate();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_show2::";
  log_spacer.delSpace();
 }
 
 void SimplePanelData::hide()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> SPData-"<<uuid<<"_hide::";
   hide_panel(panel);
   update_panels();
   doupdate();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- SPData-"<<uuid<<"_hide::";
   log_spacer.delSpace();
 }
 
 template<class T>
 class EditPanelData;


 //! derived from SimplePanelData. capable of storing list of item in a ncurses menu object. 

 //! 
 //! enables browsing capability of menu items in up/down direction. 
 //! also enables object highlighting with different color when the panel is selected
 //! also provides API to resolve keys associated with the panels
 //!     
 template<class T>
 class BrowsePanelData: public SimplePanelData
 {
  protected:
   //! vector of pointers to generic type containing value that goes into menu item. \todo: inefficient method of accessing data. replace either with linked list or some other mechanism
   std::vector<T*> object_list;
   //! array of pointers to ITEM (array of ITEM*) for menu initialization
   ITEM** item_list;
   //! size of object_list 
   int size_item_list;   
   //! capacity of object_list. \todo: no explanation for using this. rationalize (because we are maintaining a vector of pointers?) 
   int capacity;
   //! pointer to MENU
   MENU* menu;
   //! item selected in menu, 0 being header item 
   int selected;         
   //! color_pair id for selected item in menu 
   int selection_color;
   //! color_pair id for unselected item in menu
   int basic_color;
   //! current color_pair (either selection_color or basic_color)
   int curr_color;
   //! color_paid id for header color
   int header_color;
   //! panel header. \todo: header disappers when browsing a menu longer than screen height. add separate panel to hold header
   std::string header;
  public:
   //! BrowsePanelData<T> constructor
   BrowsePanelData(int);
   //! returns contents of panel's menu as vector of strings
   std::vector<std::string> contents(); 
   //! returns currently selected item on object_list  
   int currIndex() const{return selected-1;}
   //! sets panel header. \todo: will become redundant once header is implemented in separate panel. remove
   void setHeader(std::string head_str) override {header = head_str;}
   //! intended to be used with browse panels. tears down menu and re-initializes with items in object_list
   void refreshItemList() override;
   //! called from ProcViewPanel::refresh. tears down menu and reinitializes with data (reference to vector valued ProcDb dict entry) values 
   void refreshItemList2(std::vector<InfoProc<std::string>>&, std::string, const std::vector<int>&, int&) override;
   //! sets color_pair for selected menu item
   void setSelectionColor(int color_pair) override {selection_color = color_pair;}
   //! sets color_pair for unselected menu item
   void setBasicColor(int color_pair) override {basic_color = color_pair;}
   //! sets menu's first item as selected item
   void Top() override;
   //! returns copy of currently selected item in object_list 
   virtual T* currItem() const;
   //! sets currently selected menu item's color to selection color_pair and update curr_color to selection color_pair
   virtual void selectionOn() override;
   //! sets currently selected menu item's color to basic color_pair and update curr_color to basic color_pair
   virtual void selectionOff() override;
   //! moves menu item selection down by 1
   virtual void Down() override;
   //! moves menu item selection up by 1
   virtual void Up() override;
   //! resolves the key input at panel level
   virtual void resolveKey(int) override;
   //! sets color of menu's header item to be header color_pair 
   virtual void setHeaderColor(int color_pair) override; 

   //! inserts value of currently selected menu item into menu of left neighbour 
   virtual void insertIntoLeftNbr() override;
   //! inserts value of currently selected menu item into menu of right  neighbour 
   virtual void insertIntoRightNbr() override;
   virtual void addIntoLeftNbr() override;
   virtual void addIntoRightNbr() override;
   virtual void moveIntoLeftNbr() override;
   virtual void moveIntoRightNbr() override;
             
   friend class XMLTree;
 };


 template<class T>
 void BrowsePanelData<T>::setHeaderColor(int color_pair)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_setHeaderColor::"<<header; 
   header_color = color_pair;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_setHeaderColor::"<<header;
   log_spacer.delSpace();
 }
 
 template<class T>
 BrowsePanelData<T>::BrowsePanelData(int id):SimplePanelData(id)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<this->uuid<<"_constructor::";
   menu = NULL; 
   curr_color = 1;
   selected = -1;
   size_item_list = 0;
   header = "";
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<this->uuid<<"_constructor::";
   log_spacer.delSpace();
 }
 
 template<class T>
 std::vector<std::string> BrowsePanelData<T>::contents()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<this->uuid<<"_contents::";
  std::vector<std::string> out_vec;
  for(auto elem: object_list)
   out_vec.push_back(elem->shortDesc());
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<this->uuid<<"_contents::";
  log_spacer.delSpace();
  return out_vec;  
 }
 
 template<class T>
 void BrowsePanelData<T>::insertIntoLeftNbr()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<this->uuid<<"_insertIntoLeftNbr::"<<this->header;
  dynamic_cast<EditPanelData<T>*>(this->leftNbr())->insert(this->currItem());
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<this->uuid<<"_insertIntoLeftNbr::"<<this->header;
  log_spacer.delSpace();
 }
 
 template<class T>
 void BrowsePanelData<T>::insertIntoRightNbr()
 {
 }
 
 template<class T>
 void BrowsePanelData<T>::addIntoLeftNbr()
 {
 }
 
 
 template<class T>
 void BrowsePanelData<T>::addIntoRightNbr()
 {
 }

 template<class T>
 void BrowsePanelData<T>::moveIntoLeftNbr()
 {
 }
 
 
 template<class T>
 void BrowsePanelData<T>::moveIntoRightNbr()
 {
 }


 template<class T>
 T* BrowsePanelData<T>::currItem() const
 {
  log_spacer.addSpace(); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_currItem::";
  if (selected >= 1)
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_currItem::";
   log_spacer.delSpace();
   return object_list[selected-1];
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_currItem::";
  log_spacer.delSpace(); 
  return NULL;
 }
 
 template<class T>
 void BrowsePanelData<T>::refreshItemList()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_refreshItemList:: sel:"<<selected;
  
  // tear down menu
  if (menu != NULL)
  { 
   unpost_menu(menu);                         // erases menu from associated window
   free_menu(menu);                           // disconnects menu from its item array and frees storage allocated for menu
   for(int i=0; i<=(size_item_list+1);i++)    // clean up (header + data + NULL entry)
    free_item(item_list[i]);
   free(item_list);                           // free memory allocated for item_list
   size_item_list = 0;
  }    
  
  // reinitialize menu 
  item_list = (ITEM**) calloc(object_list.size()+2, sizeof(ITEM*));      // reserve space for header + data + NULL entry
    
  size_item_list = (int)object_list.size();                              // update size_item_list
  item_list[0] = (ITEM*) new_item((const char*)(header.c_str()), NULL);  // set menu's 1st entry to be header. \todo: will become irrelevant once header in sep panel. remove 
  item_opts_off(item_list[0], O_SELECTABLE);                             // \todo: will become redundant once header in sep panel. remove 
  
  for(int i=1; i<=object_list.size(); i++)                               // populate menu with entries in object list
    item_list[i] = (ITEM*) new_item((object_list[i-1]->shortDesc()), (object_list[i-1]->longDesc()));
  item_list[object_list.size()+1] = (ITEM *)NULL;                        // REQUIRED  by ncurses, set last entry to NULL
  // create new menu with new item_list
  menu = new_menu((ITEM**) item_list);
  set_menu_grey(menu, COLOR_PAIR(header_color));                         // sets color_pair for non-selectable item
  // format the menu 
  set_menu_win(menu, win);                                               // associate menu with panel's window                                              
  set_menu_mark(menu, "");                                               // suppress the mark that comes alongside the selected item 
  post_menu(menu);
  menu_driver(menu, REQ_FIRST_ITEM);
  set_menu_fore(menu, COLOR_PAIR(curr_color));                           // sets color_pair for selected menu item
  for(int i=0; i<selected;i++)                                           // restore position of menu cursor to what it was before tear down
    menu_driver(menu, REQ_DOWN_ITEM);
  wrefresh(win);
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_refreshItemList:: sel:"<<selected;
  log_spacer.delSpace(); 
 }


 template<class T>
 void BrowsePanelData<T>::refreshItemList2(std::vector<InfoProc<std::string>>& data, std::string prop_name, const std::vector<int>& sorted_indices, int& browse_pos)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_refreshItemList2::sel:"<<selected<<" browse:"<<browse_pos<<" data: "<<data.size()<<" prop: "<<prop_name<<"menu: "<<menu<<" win:"<<win<<"header: "<<header;
  
  // tear down menu
  if (menu != NULL)
  { 
   unpost_menu(menu);                                                    // erases menu from associated window
   free_menu(menu);                                                      // disconnects menu from its item array and frees storage allocated for menu
   for(int i=0; i<=(size_item_list+1);i++)                               // clean up header + data + NULL entry
     free_item(item_list[i]);
   free(item_list);                                                      // free memory allocated for item_list
   item_list = (ITEM**) NULL;
   size_item_list = 0;
  }    
  
  // reinitialize menu 
  errno = 0; 
  item_list = (ITEM**) calloc(data.size()+2, sizeof(ITEM*));   
  if (errno != 0) 
   BOOST_LOG_SEV(lg, error)<<log_spacer<<strerror(errno);

  size_item_list = (int)data.size();                                     // update size_item_list 
  item_list[0] = (ITEM*) new_item((const char*)header.c_str(), NULL);    // set 1st menu entry to header
  item_opts_off(item_list[0], O_SELECTABLE);                             // set header entry to be non-selectable,\todo: will become redundant once header is implemented in sep panel. remove 
   
  for(int i=1; i<=data.size(); i++)                                      // populate menu with data's items in order specified in sorted_indices
    item_list[i] = (ITEM*) new_item(data[sorted_indices[i-1]].shortDesc(), NULL);
  
  item_list[data.size()+1] = (ITEM *)NULL;                               // required by ncurses, last entry set to NULL
  // create new menu with new item_list
  menu = new_menu((ITEM**) item_list);                                   // attach item_list to menu
  if (menu == NULL)
   BOOST_LOG_SEV(lg, error)<<log_spacer<<"ProcViewPanel-"<<uuid<<"_refreshItemList2::menu allocation failed";
  // format the menu
  set_menu_grey(menu, COLOR_PAIR(header_color));                         //set non-selectable item's color to header_pair_color, \todo: will become redundant once header is implemented in sep panel. remove 
  set_menu_win(menu, win);                                               // associate menu with panel window 
  set_menu_mark(menu, "");                                               // suppress the mark that comes alongside the selected item
  if (data.size() > 0)                                                   // if there was data
  {
    if (browse_pos == -1 || browse_pos == 0)                             //Case: menu was empty before teardown, then set column browser to 1st item
      browse_pos = 1;
    else                                                                 //Case: fewer item now than before tear down, set browser position to last entry
    {
     if (browse_pos >= data.size())
      browse_pos = data.size();
    }
  }
  else                                                                   // no data
    browse_pos = 0;
  
  selected = browse_pos;                                                 // sync selected index in menu, and browser_pos in columns
  set_menu_fore(menu, COLOR_PAIR(curr_color));                           // set color of selected item to curr_color
  set_menu_format(menu, this->height, 1);                                // set menu's height 
  post_menu(menu);                                                       
  menu_driver(menu, REQ_FIRST_ITEM);                                     // restore position of menu cursor to what it was before tear down
  for(int i=0; i<selected; i++)
   menu_driver(menu, REQ_DOWN_ITEM);
  post_menu(menu);
  update_panels();
  doupdate();
  wrefresh(win); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_refreshItemList2:: sel:"<<selected<<" browse:"<<browse_pos<<" data: "<<data.size()<<" prop:"<<prop_name<<" menu: "<<menu<<" win:"<<win;
   log_spacer.delSpace();
 }

 template<class T>
 void BrowsePanelData<T>::selectionOn()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_selectionOn::";
  set_menu_fore(menu, COLOR_PAIR(selection_color));
  curr_color = selection_color;
  update_panels();
  doupdate();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_selectionOn::";
  log_spacer.delSpace();
 }

 template<class T>
 void BrowsePanelData<T>::selectionOff()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_selectionOff::";
  set_menu_fore(menu, COLOR_PAIR(1));
  curr_color = 1;
  update_panels();
  doupdate();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_selectionOff::";
  log_spacer.delSpace();
 }
  
 template<class T>
 void BrowsePanelData<T>::Down()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_Down:: sel:"<<selected<<" datasize: "<<size_item_list;
   if (selected == -1 || selected == size_item_list)
   {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_Down:: sel:"<<selected;
    log_spacer.delSpace();
    return;
   }
   selected++;
   menu_driver(menu, REQ_DOWN_ITEM);
   post_menu(menu);
   wrefresh(win);
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_Down:: sel:"<<selected<<" datasize: "<<size_item_list;
   log_spacer.delSpace();
 }
 
 template<class T>
 void BrowsePanelData<T>::Up()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_Up:: sel: "<<selected<<" datasize: "<<size_item_list;
   if (selected <= 1)
   {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_Up:: sel:"<<selected;
    log_spacer.delSpace();
    return;
   }
   selected--;
   menu_driver(menu, REQ_UP_ITEM);
   post_menu(menu);
   wrefresh(win);
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_Up:: sel:"<<selected<<" datasize: "<<size_item_list;
   log_spacer.delSpace();
 }

 template<class T>
 void BrowsePanelData<T>::Top()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_Top:: sel:"<<selected;
  if (selected == -1)
  {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_Top:: sel: "<<selected;
    log_spacer.delSpace();
    return;
  }
  selected = 1;
  menu_driver(menu, REQ_FIRST_ITEM);
  menu_driver(menu, REQ_DOWN_ITEM);
  wrefresh(win);
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_Top:: sel: "<<selected;
  log_spacer.delSpace();
 }
 
 template<class T>
 void BrowsePanelData<T>::resolveKey(int ch)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> BrowsePanelData-"<<uuid<<"_resolveKey::";
   (mapKeyDict.getKeyDict(this->smKeys.currState())).resolve(ch, this);                // resolve passed key with current KeyDict               
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- BrowsePanelData-"<<uuid<<"_resolveKey::";
   log_spacer.delSpace();
 }

 //! derived from BrowsePanelData. capable of editing items in ncurses menu list. 

 //! 
 //! enables editing capability of menu items by deleting, adding and rearraging items. 
 //! also enables object highlighting with different color when the panel is selected and in edit mode
 //!     
 template<class T>
 class EditPanelData: public BrowsePanelData<T>
 {
  private:
   //! editing status of panel
   bool editing;
   //! index of color_pair for editing
   int editing_color;
  public:
   EditPanelData(int);

   // methods overriding ancestor class methods

   //! sets editing to true
   void editModeOn() override;
   //! sets editing to false
   void editModeOff() override;
   //! toggles editing mode
   void toggleEditMode() override;
   //! swap selected item with item below it (editing) or moves cursor down by one (not editing)
   void Down() override;
   //! swap selected item with item above it (editing) or moves cursor up by one (not editing)
   void Up() override;
   //! remove currently selected menu item
   void remove() override;
   //! sets editing color_pair 
   void setEditingColor(int color_pair) override {editing_color = color_pair;}
   //! resolves current key with KeyDict
   virtual void resolveKey(int) override;
   //! sets currently selected menu item's color to selection curr_color_pair (editiing_color or selection_color) and update curr_color to selection color_pair/editing_color
   void selectionOn() override;
   //! sets currently selected menu item's color to basic_color and update curr_color to basic_color 
   void selectionOff() override;
   //! sets header color_pair
   void setHeaderColor(int) override;
   //! inserts value of currently selected menu item into menu of left neighbour 
   void insertIntoLeftNbr() override;
   //! inserts value of currently selected menu item into menu of right neighbour
   void insertIntoRightNbr() override;
  
   // non-overriding methods

   //! inserts Info* object into into object_list above selected item 
   void insert(Info*);
   //! not implemented
   void insert(InfoProc<float>*){}
   //! not implemented
   void insert(InfoProc<int>*){}
   //! not implemented
   void insert(InfoProc<std::string>*){}
   //! return edit mode
   bool editMode() const {return editing;}
   //! releases UI resources associated with this panel
   void release();                          
   friend class XMLTree;   
 }; 
 
 template<class T>
 void EditPanelData<T>::release()
 {
  log_spacer.addSpace(); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_release::"<<"prop: "<<this->header<<"win: "<<this->win;
  // tear down menu and object_list
  if (this->menu != NULL)
  {
   unpost_menu(this->menu);
   free_menu(this->menu);
   for(int i=0; i<=(this->size_item_list+1);i++)            // clean up (header + data + NULL entry)
    free_item(this->item_list[i]);
   free(this->item_list);                                   // free memory allocated for item_list
   for(int i=0; i<this->object_list.size(); i++)            // delete memory pointed to by object_list entires. 
    delete(this->object_list[i]);
  }
  delwin(this->win);                                        // delete window associated with panel
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_release::"<<"prop: "<<this->header;
  log_spacer.delSpace();
 }
 
 template<class T>
 void EditPanelData<T>::setHeaderColor(int color)
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_setHeaderColor::"<<this->header; 
  this->header_color = color;             
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_setHeaderColor::"<<this->header;
  log_spacer.delSpace();
 }

 template<class T>
 void EditPanelData<T>::insertIntoLeftNbr()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_insertIntoLeftNbr::"<<this->header;
  dynamic_cast<EditPanelData<T>*>(this->leftNbr())->insert(this->currItem()); //cast leftNbr reference as EditPanelData before inserting into left neighbour
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<--EditPanelData-"<<this->uuid<<"_insertIntoLeftNbr::"<<this->header;
  log_spacer.delSpace();
 }

 template<class T>
 void EditPanelData<T>::insertIntoRightNbr()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_insertIntoRightNbr::"<<this->header;
  dynamic_cast<EditPanelData<T>*>(this->rightNbr())->insert(this->currItem()); //cast rightNbr reference as EditPanelData before inserting into right neighbour
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<--EditPanelData-"<<this->uuid<<"_insertIntoRightNbr::"<<this->header;
  log_spacer.delSpace();
 }
 
 template<class T>
 void EditPanelData<T>::toggleEditMode()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_toggleEditMode::";
  editing = !editing;
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_toggleEditMode::";
  log_spacer.delSpace();
 }
 
 template<class T>
 void EditPanelData<T>::selectionOn()
 {
   log_spacer.addSpace(); 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_selectionOn::";
   if (editing)
     this->curr_color = editing_color;
   else
     this->curr_color = this->selection_color;
   set_menu_fore(this->menu, COLOR_PAIR(this->curr_color));
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_selectionOn::";
   log_spacer.delSpace();
 }

 template<class T>
 void EditPanelData<T>::selectionOff()
 {
  log_spacer.addSpace(); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_selectionOff::";
  this->curr_color = 1;
  set_menu_fore(this->menu, COLOR_PAIR(this->curr_color));
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_selectionOff::";
  log_spacer.delSpace();
 }

 template<class T>
 void EditPanelData<T>::insert(Info* pitem)
 {
  log_spacer.addSpace(); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_insert:: cap:"<<this->object_list.capacity();
  Info* pinfo = new Info{*pitem};                                       // instantiate Info object on heap with content of *pitem
  std::vector<Info*> tmpvec;                              
  tmpvec.reserve(this->object_list.capacity());                         // prepare Info* vector to store object_list
  for(auto it=(this->object_list.begin()+this->selected-1); it != this->object_list.end(); it++)  // copy object_list from selected item until end
   tmpvec.push_back(*it); 
  this->object_list.insert(this->object_list.begin()+this->selected-1, pinfo);  // insert newly created object on heap at position 'selected' into object_list
  
  // restoring invalidated pointers. 
  for(int i=0; i<tmpvec.size(); i++)                                     // repopulate invalidated object_list entries (everything) after newly insert entry.  
   this->object_list[this->selected+i] = tmpvec[i];
  BrowsePanelData<T>::refreshItemList();                                 // invoke refreshItemList to update panel menu
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_insert::";
  log_spacer.delSpace(); 
 }
 
 //! EditPanelData<T> constructor 
 template<class T>
 EditPanelData<T>::EditPanelData(int id):BrowsePanelData<T>(id)
 {
 }
 
 template<class T>
 void EditPanelData<T>::editModeOn()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_editModeOn::";
  editing = true;
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_editModeOn::";
  log_spacer.delSpace();
 }  

 template<class T>
 void EditPanelData<T>::editModeOff()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_editModeOff::";
   editing = false;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_editModeOff::";
   log_spacer.delSpace();
 }  

 template<class T>
 void EditPanelData<T>::Down()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_Down:: sel:"<<this->selected<<" datasize:"<<this->size_item_list;
   if (!editing)                                                       // if not in editing mode, browse down and return
   {
    BrowsePanelData<T>::Down();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_Down:: sel: "<<this->selected;
    log_spacer.delSpace();
    return;
   }
   
   if (this->selected == -1 || this->selected == this->size_item_list)
   {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_Down:: sel:"<<this->selected;
    log_spacer.delSpace();
    return;
   }
   // if in editing mode
   swap(this->object_list, this->selected-1, this->selected);          // swap with item below in object_list
   this->selected++; 
   BrowsePanelData<T>::refreshItemList();                              // refresh panel's menu 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_Down:: sel:"<<this->selected<<" datasize:"<<this->size_item_list;
   log_spacer.delSpace();
 } 
 
 template<class T>
 void EditPanelData<T>::Up()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_Up:: sel:"<<this->selected<<" datasize:"<<this->size_item_list;

   if (!editing)                                                        // if not in editing mode, browse up and return
   {
    BrowsePanelData<T>::Up();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_Up:: sel:"<<this->selected;
    log_spacer.delSpace();
    return; 
   }
   if (this->selected <= 1)
   {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_Up:: sel:"<<this->selected;
    log_spacer.delSpace();
    return;
   }
   //! if in editing mode
   swap(this->object_list, this->selected-1, this->selected-2);         // swap with item above in object_list
   this->selected--;
   BrowsePanelData<T>::refreshItemList();                               // refresh panel's menu 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_Up:: sel:"<<this->selected<<" datasize:"<<this->size_item_list;
   log_spacer.delSpace();
 } 

 template<class T>
 void EditPanelData<T>::resolveKey(int ch)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_resolveKey::panel: "<<this->uuid;
   (mapKeyDict.getKeyDict(this->smKeys.currState())).resolve(ch, this); // resolve key with panel's key dictionary
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_resolveKey::panel: "<<this->uuid;
   log_spacer.delSpace();
 } 
 
 template<class T>
 void EditPanelData<T>::remove()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> EditPanelData-"<<this->uuid<<"_remove::, sel: :"<<this->selected;

   if (this->selected <= 0)
   {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_remove::panel: "<<this->uuid;
   log_spacer.delSpace();
    return;
   }
   delete(this->object_list[this->selected-1]);                              // delete heap memory associated with current item 
   std::vector<T*> tmpvec;
   tmpvec.reserve(this->object_list.capacity());
   for(auto it=(this->object_list.begin()+this->selected); it != this->object_list.end(); it++)  // copy object_list items from item past current item until the end
    tmpvec.push_back(*it); 
   this->object_list.erase(this->object_list.begin()+this->selected-1);      // erase item from object list 
   if (this->selected == (this->object_list.size()+1))
     this->selected--;
   
   // restore invalidated pointers
   for(int i=0; i<tmpvec.size(); i++)                                        // repopulate object_list items from selected item until end from tmpvec
     this->object_list[i+this->selected-1] = tmpvec[i];
   BrowsePanelData<T>::refreshItemList();                                    // update panel's menu
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- EditPanelData-"<<this->uuid<<"_remove::sel: "<<this->selected;
   log_spacer.delSpace();
 } 

 //! panel data structure responsible for displaying of property values for all live processes.

 //!
 //! responsible for deleting/adding/moving edit panels (referred internally as columns) corresponding to each property. <br/> 
 //! Also holds information for width, height and arrangement of property columns. 
 //! 
 class ProcViewPanel: public SimplePanelData
 {
  private:
   std::mutex clms_mutex;
   //! browser position in property column. 1 corresponds to 1st item
   int browse_pos;
   //! vector of EditPanel data structures, each containing list of specific propertie's value for all processes
   std::vector<EditPanelData<InfoProc<std::string>>> columns;
   //! pointer reference to ProcDb object 
   ProcDb* proc_database;
   //! sorting key - some process's properties name
   std::string sort_key;
   //! dictionary of column width assigned to each process property
   std::map<std::string, int> prop_wid_dict;
   //! vector of property names currently being displayed by the panel 
   std::vector<std::string> clm_names;
   //! vector of column widths corresponding to properties being displayed 
   std::vector<int> clm_widths;
   //! reference to panel data structure containing property names to be displayed. clm_names, clm_widths, columns remains in sync with item list in this panel
   SimplePanelData* active_proc_panel;
   //! \todo: not being used. make use of 
   int y_start;
   //! color_pair corresponding to column headers
   int header_color;
   
   // methods

   //! responsible for creating a new edit panel containing menu to hold property values
   void createColumn(EditPanelData<InfoProc<std::string>>&, int, int, std::string);
   //! runs only initially to create columns to load all properties to be displayed
   void loadColumnsFromActiveProc();
  public:
   //! ProcViewPanel constructor
   ProcViewPanel(int id): browse_pos{-1},SimplePanelData(id){}
     
   // methods overriding ancestor class methods

   //! resolve current key with panel's key dictionary 
   void resolveKey(int) override;
   //! makes all columns associated with this panel visible 
   void show() override;
   //! hides all columns associated with this panel
   void hide() override;
   //! deletes Column based on item selected in active_proc_panel
   void deleteColumn() override;
   //! inserts Column based on item selected in active_proc_panel
   void insertColumn() override;
   //! swaps column (based on selection in active_proc_panel) with left neighbour 
   void moveLeft() override;
   //! swaps column (based on selection in active_proc_panel) with left neighbour 
   void moveRight() override;
   //! move cursor one position down
   void Down() override;
   //! move cursor one position up
   void Up() override;
   //! manages column initialization and column updates 
   void refresh() override;
   //! set column header color_pair
   void setHeaderColor(int hcolor) override;
   //! \todo: some bug is assigning header color to procviewpanel's columns. this function not doing its job. investigate 
   void syncColors() override;

   // non-overriding methods

   //! called by Column::read. return vector of property names currently being displayed
   std::vector<std::string> displayedKeys() const;
   //! called by Column::read. returns property name to be used as sort key 
   std::string sortKey() const{return sort_key;}

   friend class XMLTree;
 };

 void ProcViewPanel::setHeaderColor(int hcolor) 
 {
    log_spacer.addSpace();
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_setHeaderColor::";
    header_color=hcolor;
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_setHeaderColor::";
    log_spacer.delSpace();
 }

 std::vector<std::string> ProcViewPanel::displayedKeys() const
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_displayedKeys::";
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_displayedKeys::";
   log_spacer.delSpace();
   return clm_names;
 }

 void ProcViewPanel::resolveKey(int ch)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_resolveKey::";
   (mapKeyDict.getKeyDict(smKeys.currState())).resolve(ch, this);
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_resolveKey::";
   log_spacer.delSpace();
 }
 
 void ProcViewPanel::syncColors()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_syncColors::";
   for(auto panelobj:columns)
    panelobj.setHeaderColor(header_color);
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_syncColors::";
   log_spacer.delSpace();
 }
 
 void ProcViewPanel::createColumn(EditPanelData<InfoProc<std::string>>& panel_data, int width, int xpos, std::string header)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_createColumn::"<<" mainwin:"<<win;
   WINDOW* panel_win = derwin(win, this->height-2, width, 0, xpos);  // create derived window from procviewpanel's main window
   PANEL* temp_panel = new_panel(panel_win);                         // associate panel with derived window, data structure associated with panel is on heap 
   panel_data.setWin(panel_win);                                     // set editpanel's window to be the derived window
   panel_data.setPanel(temp_panel);                                  // set editpanel's panel to be created panel
   panel_data.setHeader(header);                 
   panel_data.setHeaderColor(4);                                     //! \todo: header color being set using hardcoded number. set properly. 
   panel_data.setDimensions(this->height-2, width);                  // update column's dimensions
   mvderwin(panel_win, 0, xpos);                                     // move derived window to (xpos, 0) within procviewpanel's window
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_createColumn::"<<" win:"<<panel_win<<" width: "<<width;
   log_spacer.delSpace();
 }
 
 void ProcViewPanel::loadColumnsFromActiveProc()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_loadColumnsFromActiveProc::";
  std::vector<std::string> active_props = dynamic_cast<BrowsePanelData<Info>*>(active_proc_panel)->contents();  // obtain list of properties to be displayed. The order determines the layout of the columns
  // create one column for each property to be displayed
  int x_pos = 0;
  int width = 0;
  for(auto prop: active_props)
  {
   EditPanelData<InfoProc<std::string>> panel_data(-1);   // create an empty edit panel
   width = prop_wid_dict[prop];                           // read edit panel width from procviewpanel's prop_wid_dict
   createColumn(panel_data, width , x_pos, prop);         // create a column from the edit panel
   x_pos += width;                                        // update xpos for next incoming panel
   columns.push_back(panel_data);                          
   clm_widths.push_back(width);
   clm_names.push_back(prop); 
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_loadColumnsFromActiveProc::";
  log_spacer.delSpace();
 }
  
 void ProcViewPanel::show()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<this->uuid<<"_show::";
  for(auto panelobj: columns)
    panelobj.show2();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<this->uuid<<"_show::";
  log_spacer.delSpace();
 }

 void ProcViewPanel::hide()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<this->uuid<<"_hide::";
  for(auto panel: columns)
   panel.hide();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<this->uuid<<"_hide::";
  log_spacer.delSpace();
 }

 void ProcViewPanel::deleteColumn() 
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_deleteColumn::";
  int curr_index = 0;
  curr_index = dynamic_cast<BrowsePanelData<Info>*>(active_proc_panel)->currIndex();   // obtain index of column to delete
  clm_names.erase(clm_names.begin()+curr_index);                                       // update clm_names by deleting prop name
  
  clm_widths.erase(clm_widths.begin()+curr_index);                                     // update clm_widths by deleting width of clm corresponding to obtained index
  
  columns[curr_index].release();                                                       // make editpanel associated with column to release UI resources
  columns.erase(columns.begin()+curr_index);                                           // remove column from columns

  // update position of remaining columns
  int x_pos = 0;                                                                       
  if (curr_index >= clm_widths.size())  // if deleted element was the last element, reduce curr_index by 1
   curr_index--;
  for(int i=0; i<curr_index; i++)       
   x_pos += clm_widths[i];
  for(int i=curr_index; i<clm_widths.size(); i++)
  {
   mvderwin(columns[i].window(), 0, x_pos);
   x_pos += clm_widths[i];
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_deleteColumn::"<<curr_index;
  log_spacer.delSpace(); 
 }

 void ProcViewPanel::insertColumn()
 {
  log_spacer.addSpace(); 
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_insertColumn::";
  std::string prop_name = (dynamic_cast<BrowsePanelData<Info>*>(active_proc_panel)->currItem())->shortDesc();  // obtain prop name to be inserted 
  int curr_index = 0;
  curr_index = dynamic_cast<BrowsePanelData<Info>*>(active_proc_panel)->currIndex();                           // obtain location at which new prop column to be inserted
  clm_names.insert(clm_names.begin()+curr_index, prop_name);                                                   // update columns with new prop name
  clm_widths.insert(clm_widths.begin()+curr_index, prop_wid_dict[prop_name]);                                  // update clm_widths
  int x_pos=0;
  for(int i=0; i<curr_index; i++)
   x_pos += clm_widths[i];
   
  EditPanelData<InfoProc<std::string>> panel_data(-1);                                                         // create new edit panel 
  createColumn(panel_data, clm_widths[curr_index], x_pos, prop_name);                                          // update edit panel with column parameters
  columns.insert(columns.begin()+curr_index, panel_data);                                                      // insert edit panel into columns
  x_pos += clm_widths[curr_index];
  // update position of columns 
  for(int i=curr_index+1; i<columns.size(); i++)
  {
    mvderwin(columns[i].window(), 0, x_pos);
    x_pos += clm_widths[i];
  }
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_insertColumn::"<<"win: "<<panel_data.window();
  log_spacer.delSpace();
 }
 
  void ProcViewPanel::moveLeft()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_moveLeft::";
  bool edit_mode = dynamic_cast<EditPanelData<Info>*>(active_proc_panel)->editMode();
  if(!edit_mode)                               // if not in edit mode, return without doing anything
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_moveLeft::"<<"mode:"<<edit_mode;
   log_spacer.delSpace();
   return;
  } 
  int curr_index = 0;
  curr_index = dynamic_cast<BrowsePanelData<Info>*>(active_proc_panel)->currIndex();  // obtain index of column to be moved left
  if (curr_index <= 0)                                                                // if leftmost element, return withouth doing anything
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_moveLeft::"<<"mode:"<<edit_mode;
   log_spacer.delSpace();
   return;
  }
  swap(clm_widths[curr_index-1],clm_widths[curr_index]);                              // update clm_widths by swapping element pos with one to left
  swap(clm_names[curr_index-1], clm_names[curr_index]);                               // update clm_names by  swapping element pos with one to left

  // swap columns
  EditPanelData<InfoProc<std::string>> tmpPanel = columns[curr_index-1];              // update columns by swapping element pos with one to left 
  columns[curr_index-1] = columns[curr_index];
  columns[curr_index] = tmpPanel;
 
  // update column window positions 
  int x_pos = 0;                                        
  for(int i=0; i<(curr_index-1); i++)
   x_pos += clm_widths[i]; 
  mvderwin(columns[curr_index-1].window(), 0, x_pos);
  x_pos += clm_widths[curr_index-1];
  mvderwin(columns[curr_index].window(), 0, x_pos);
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_moveLeft::"<<"mode:"<<edit_mode;
  log_spacer.delSpace();
 }

 void ProcViewPanel::moveRight()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_moveRight::";
  bool edit_mode = dynamic_cast<EditPanelData<Info>*>(active_proc_panel)->editMode();
  if(!edit_mode)                                             // if not in edit mode, return without doing anything
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_moveRight::"<<"mode:"<<edit_mode;
   log_spacer.delSpace();
   return;
  } 

  int curr_index = 0;
  curr_index =  dynamic_cast<BrowsePanelData<Info>*>(active_proc_panel)->currIndex();
  if (curr_index >= (clm_names.size()-1))                                          // if leftmost element, return withouth doing anything
  {
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_moveRight::"<<"mode:"<<edit_mode;
    log_spacer.delSpace();
    return;
  }
  
  // swap columns
  swap(clm_widths[curr_index], clm_widths[curr_index+1]);              // update clm_widths by swapping element pos with one to left
  swap(clm_names[curr_index], clm_names[curr_index+1]);                // update clm_names by swapping element pos with one to left

  EditPanelData<InfoProc<std::string>> tmpPanel = columns[curr_index]; // update columns by swapping element pos with one to left 
  columns[curr_index] = columns[curr_index+1];
  columns[curr_index+1] = tmpPanel;
  
  // update column window positions 
  int x_pos = 0;
  for(int i=0; i<curr_index; i++)
   x_pos += clm_widths[i];
  mvderwin(columns[curr_index].window(), 0, x_pos);
  x_pos += clm_widths[curr_index];
  mvderwin(columns[curr_index+1].window(), 0, x_pos);
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_moveRight::"<<"mode:"<<edit_mode;
  log_spacer.delSpace();
 }

 void ProcViewPanel::Down()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_Down:: browse:"<<browse_pos;
  proc_database->access.lock();        // need to gain access to proc_database lock, as we use its size, which should not change while we browse down
  int sz = proc_database->dbsize;       
  if (browse_pos >= sz)                // if browse_pos > sz since last check, reset it to last element
  {
    browse_pos = sz;
    BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_Down:: browse:"<<browse_pos;
    log_spacer.delSpace();
    return;
  }  
  browse_pos++;
  for(auto panel: columns)             // move each column's menu element selection down by 1 position
   panel.Down();
  proc_database->access.unlock();      // release access to proc_database lock
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_Down:: browse:"<<browse_pos;
  log_spacer.delSpace();
 }

 void ProcViewPanel::Up()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_Up:: browse:"<<browse_pos;
  proc_database->access.lock();        // need to gain access to proc_database lock, as we use its size, which should not change while we browse down
  if (browse_pos <= 1)                 // if browse position is already at top, return without doing anything
  {
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_Up:: browse:"<<browse_pos;
   log_spacer.delSpace();
   return;
  }
  browse_pos--;
  for(auto panel: columns)              // move each column's menu element selection up by 1 position
   panel.Up();
  proc_database->access.unlock();       // release access to proc_database lock
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_Up:: browse:"<<browse_pos;
  log_spacer.delSpace();
 }
 
 void ProcViewPanel::refresh()
 {
  log_spacer.addSpace();
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> ProcViewPanel-"<<uuid<<"_refresh:: browse: "<<browse_pos;
  clms_mutex.lock();                     //! \todo: no rationale for using mutex here. rationalize 
  if(clm_names.empty())                  // initialization step, if clm_names has not been populated, do so now
   loadColumnsFromActiveProc();
  clms_mutex.unlock();
  // refresh the columns to be displayed with entries from process database
  proc_database->access.lock();          // acquire lock on proc_database
  //! \todo: no mutex to restrict access to clm_names. rationalize and implement 
  for(int i=0; i<clm_names.size(); i++)  // refresh each column to vector entry from proc_database data dictionary
    columns[i].refreshItemList2(proc_database->database[clm_names[i]], clm_names[i], proc_database->sorted_indices,browse_pos);
  proc_database->access.unlock();        // release lock on proc_database
  BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- ProcViewPanel-"<<uuid<<"_refresh:: browse:"<<browse_pos;
  log_spacer.delSpace();
 }

} // namespace rtop

#endif  //_PANEL_H_
    
