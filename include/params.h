#include <map>

#ifndef PARAMS_H_
#define PARAMS_H_

namespace rtop
{
 int POLL_INTVL = 1000;
 bool EXIT = false;
 int KEY_INPUT = 0;
 std::map<int, std::pair<int,int>> colors;  
} // rtop



#endif // PARAMS_H_
