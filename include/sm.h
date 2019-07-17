#include <map>
#include <unordered_set>

#ifndef _SM_H_
#define _SM_H_
#include "rtop_logger.h"
#include "params.h"

extern src::severity_logger<severity_level> lg;
extern logSpacer log_spacer;

namespace rtop
{
 //! statemachine that uses integer corresponding to object uuids to represent states

 //!
 //! provides API to change state, based on key input <br/>
 //! and reading state information
 //! 
 class StateMachine
 {
   private:
    //! state transition table
    std::map<int, std::map<int, int>> state_transitions;
    //! state variable holding current state value 
    int curr_state;
    //! state variable holding previous state value
    int prev_state;
    //! indicates whether state has changed
    bool changed = false;
   public:
    //! StateMachine constructor. Instantiates state machine with provided state transition table
    StateMachine(std::map<int, std::map<int, int>>& trans):state_transitions{trans}{}
    //! StateMachine constructor.  
    StateMachine():curr_state{-1}, prev_state{-1}{}
    ~StateMachine(){}
    //! add a single transition i.e.  tuple (key_input, curr_state, next_state)
    void addTransition(int, int, int);
    //! updates current state with next state based on key input and encoded state transition table
    void next(int);
    //! same as StateMachine::next(int) except that reads key input from global 
    void next();
    //! resets state machine into base state as specified in transition table tuple entry (base_state, -1, base_state) 
    void reset();
    //! sets current state
    void setCurrState(int state){curr_state = state;}
    //! returns current state
    int currState() const {return curr_state;}
    //! returns previous state
    int prevState() const {return prev_state;}
    //! returns true if prev_state and curr_state are different
    bool isChanged() const {return changed;}
    //! sets changed to false
    void stale();
    friend class XMLTree;
 }; 

 
 void StateMachine::stale()
 {
   log_spacer.addSpace(); 
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> StateMachine-"<<curr_state<<"_stale::";   
   changed = false;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_stale::";  
   log_spacer.delSpace(); 
 }

 void StateMachine::reset()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> StateMachine-"<<curr_state<<"_reset::";   
   curr_state = state_transitions[-1][curr_state];      // corresponds to tuple (base_state, -1, base_state)
   changed = false;                                      
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_reset::";  
   log_spacer.delSpace(); 
 }
 
 void StateMachine::next(int ch)
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> StateMachine-"<<curr_state<<"_next-int::";  
    
   prev_state = curr_state;                                                       // save curr_state into prev_state
   if (state_transitions.find(ch) == state_transitions.end())                     // if key is not found in state transition table
   {
     changed = false;                                                             // no change occured, so changed set to false 
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next-int::";  
     log_spacer.delSpace(); 
     return;
   }
   if (state_transitions[ch].find(curr_state) == state_transitions[ch].end())     //! \todo: this is absurd as curr_state should always be one option in transition table. investigate, remove
   {
     changed = false;
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next-int::";  
     log_spacer.delSpace(); 
     return; 
   } 
   curr_state = state_transitions[ch][curr_state];                                // update current state
   if (curr_state == -1)                                                          // \todo: this is also absurd, why is state machine concerning itself with exit global. rationalize/investigate. remove 
   {
     EXIT = true;
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next-int::";  
     log_spacer.delSpace(); 
     return;
   } 
   if (prev_state == curr_state)                                                  // if current state is same as prev state
    changed = false;
   else 
    changed = true;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next-int::";  
   log_spacer.delSpace(); 
 }
 
 void StateMachine::next()
 {
   log_spacer.addSpace();
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"--> StateMachine-"<<curr_state<<"_next::";   

   prev_state = curr_state;                                                                  // save curr_state into prev_state
   if (state_transitions.find(KEY_INPUT) == state_transitions.end())                         // if key is not found in state transition table
   {
     changed = false;                                                                        // no change occured, so changed set to false 
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next::";   
     log_spacer.delSpace();
     return;
   }
   if (state_transitions[KEY_INPUT].find(curr_state) == state_transitions[KEY_INPUT].end())  // \todo: absurd case, not possible. investigate and remove
   {
     changed = false;
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next::";  
     log_spacer.delSpace(); 
     return; 
   } 
   curr_state = state_transitions[KEY_INPUT][curr_state];                                    // update current state
   if (curr_state == -1)                                                                     // \todo: absurd to handle EXIT global here. rationalize/investigate. remove
   {
     EXIT = true;
     BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next::";  
     log_spacer.delSpace(); 
     return;
   } 
   if (prev_state == curr_state)                                                             // if current state is same as prev state
    changed = false;
   else 
    changed = true;
   BOOST_LOG_SEV(lg, debug)<<log_spacer<<"<-- StateMachine-"<<curr_state<<"_next::";  
   log_spacer.delSpace(); 
 }

 void StateMachine::addTransition(int key, int curr_st, int next_st)
 {
   if(state_transitions.find(key) == state_transitions.end())                               // if key is not already present in transition table
   {
     std::map<int, int> newmap;
     state_transitions[key] = newmap;
   } 
   state_transitions[key][curr_st] = next_st;
 }

} // namespace rtop

#endif  // _SM_H_

