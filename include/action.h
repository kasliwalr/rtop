#include <string>

#ifndef ACTION_H_
#define ACTION_H_


namespace rtop
{

 //! Action object
 
 //!
 //! passed to MemFuncPDict::invoke(Action) 
 //! func_name refers to member function of objects. Currently, only void function() type are allowed
 //!
 class Action
 {
  public:
   int object_uuid;
   std::string func_name;
 };

} //namespace rtop
#endif 
