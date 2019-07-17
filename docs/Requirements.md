# Requirements

## Introduction

This document contains informal requirements for the text based user interface application [rtop](../README.md). **rtop** is a user friendly system monitor which allows a typical linux user (who is more comfortable with keyboard than mouse) to access information regarding processes running on the system. A user may want to see resource hogging processes, search for a specific process and then send signals to them such as killing them altogether. The information typically includes process's consumption of system resources such as cpu time and physical memory (RAM) and other relevant information. The process monitor typically also provides facilities to send signals to the processes. One may ask, why not a GUI? Sure that could work, but GUI applications are typically resource hoggers, a text user interface is more lightweight, closer to the system, and thus more easily portable to linux running on less powerful processors with less memory.  


The sections below detail the requirements for the process monitor. Since this is an evolving product, this set of requirements is one in a series, and corresponds to some version of rtop. Also, note that we distinguish between must-have and nice-to-have requirements. Must-have requirements are indicated in regular font and nice-to-have requiremnets are in _italics_. The nice-to-haves may or may not be met. Look for the implementation details in the [design document](Design.md)

## Requirements

We want to make a text based user interface. This means it is entirely controlled by the keyboard, although it can process simple mouse instructions as well.   

### Target System
1. The application must run on all 64-bit desktop systems supported by Ubuntu 16.04 LTS 
2. *The application should be portable to major Linux distributions such as Fedora, Ubuntu, Debian etc. targeted towards desktop/embedded platforms.* 

### User interface

The interface should consists of the following views at the minimum

1. Terminal
   - the interface should fit on common terminal emulators available on Ubuntu 16.04 LTS. At the minimum it should work flawlessly on gnome-terminal. 
   - user should be able to move the terminal window, without any effect on the display 
   - *user should be able to resize the terminal window, and the application should reorganize information suitably for viewing*
      
2. Process View
   - Layout
     - should contain process view in the upper part of window, and key dictionary in the lower part
     - process view should consist of columns where each column corresponds to a specific process property. Each column should have a header. 
     - each row in the process list should correspond to a single process.
     - information that does not fit on the screen, shall not be visible to the user. It will be made visible, when user scrolls downwards. The topmost rows then become hidden. The headers should continue to remain visible.
     - key dictionary should consist of 
       - Up/Down arrow key: slow browsing
       - PgUp/PgDown key: fast browsing
       - ENTER: select a process
       - F10: exit application
       - F2: navigate to process configration view
     - each key in key dictionary should be listed by name, and the action it performs  
      
   - Visual Style
     - kernel process should be in a different colored foreground
     - property column header should be in different colored background
     - process under scroll cursor should be in a different colored background 
     - selected process should be in a different colored background 
   - Updates
     - process information should be updated regularly but not faster than once every 1 sec
   - Actions
     - user should be able to scroll vertically up/down, in both fast and slow modes   
     - a process should be selectable, so that action can be taken on it (post selection)
     - user should be able to choose a particular property as sorting criteria for listing the process list in a sorted order 
     - *user should be able to browse left-right*,  in which case the columns may be partially hidden
     - *user should be able to search for a particular process by name*, in which case the cursor should move to that process
     - *user should be able to filter processes by name*, in which case, only the filtered processes are visible. If no process matches, filtered view should be empty
   - Scroll Behaviour
     - when process dies - the next available entry in list is selected. 
     - scroll position does not track process across refreshes, it only maintains its position from the start. 
     - if number of process is fewer than last scroll position, then scroll position sticks to last item in process list.   
 
3. Usability information
   - for any view, all the relevant control keys, along with their effects should be indicated. 
   - some obvious key such as used for scrolling may be omitted. We will include a list of keys that can be optionally displayed. 
   - optionally displayed keys:
     - Scrolling the process list: Up/Down arrow, PgUp/PgDown
     - Left/Right navigation between active prop and all prop columns: Left/Right arrow
     - Scrolling the active properties or all properties column: Up/Down arrow
      
 
4. Keyboard control:
   - one should be able to control all functions of the TUI using keys available on the standard US keyboard. 

5. Visual Style
   - colors must be used to different b/w column header and column data
   - colors must be used to indicate position of scroll cursor
   - colors must be used to differentiate b/w a selection and scroll position

6. Process Config View
   - Layout
     - two columns, active properties and all properties in the top, and key dictionary in the bottom
     - active is to the left of all properties
     - Key Dictionary should have two configurations one for active properties and other for all properties
       - active properties
         - ENTER: turns selection on/off
         - Up arrow: move entry up
         - Down arrow: move entry down
         - Delete: delete entry
       - all properties
         - ENTER: insert selected entry into active properties column 
   - Action
     - active column should allow scrolling, deletion and rearranging of properties
     - all properties column should allow scrolling and insertion of properties to active column
   - List of properties to include
     - Command
     - PID
     - STATE (S): S/R/D/Z/T/W
     - PPID
     - PGRP
     - SESSION
     - TTY_NR (TTY)
     - TPGID
     - UTIME (UTIME+)
     - STIME (STIME+)
     - PRIORITY (PRI)
     - NICE (NI)
     - STARTTIME (START)
     - PROCESSOR (CPU)
     - M_SIZE (VIRT)
     - M_RESIDENT (RES)
     - M_SHARE (SHR)
     - M_TRS (CODE)
     - M_DRS (DATA)
     - ST_UID (UID)
     - PERCENT_CPU (CPU%)
     - PERCENT_MEM (MEM%)
     - USER
     - TIME (TIME+)
     - NLWP
     - IO_PRIORITY (IO)
     - PERCENT_CPU_DELAY (CPUD%)
     - PERCENT_IO_DELAY (IOD%)
     - PERCENT_SWAP_DELAY (SWAPD%)
     - *MINFLT*
     - *CMINFLT*
     - *MAJFLT*
     - *CMAJFLT*
     - *TGID*
     - *CTID* 
     - *VPID*
     - *VXID*
     - *RCHAR (RD_CHAR)*
     - *WCHAR (WR_CHAR)*
     - *SYSCR (RD_SYSC)*
     - *SYSCW (WR_SYSC)*
     - *RBYTES (IO_RBYTES)*
     - *WBYTES (IO_WBYTES)*
     - *CNCLWB (IO_CANCEL)*
     - *IO_READ_RATE (DISK READ)*
     - *IO_WRITE_RATE (DISK WRITE)*
     - *IO_RATE (DISK R/W)*
     - *CGROUP*
     - *OOM*
     - *CUTIME (CUTIME+)*
     - *CSTIME (CSTIME+)*
     - *M_LRS (LIB)*
     - *M_DT (DIRTY)*
 
### Configurability

   - the user should be able to configure the properties that are visible in the process view
   - the user should also be able to rearrange the order in which process properties are displayed
   - *the background/foreground color should be changeable*
   - *refresh interval should be configurable*

### Extensibility 
   - *developer should be able to add new properties for the process*
   - *developer should be able to change the key dictionary for views/columns/application*, this is to enable portability
   - *the developer should be able to change the look and feel of the view*, to what extent?? 
 
### Handling Bad Input

- ignore unrecognized key input
- continued key presses should either be ignored or rate limited. specific needed??
- when the error cannot be handled meaning the program is about to crash, log the erorr to a log file
- when sorting criteria does not make sense, ignore a sorting request

### Deployment
- the application source code should be available through an online git repo
- the repo should include detailed install instructions for Ubuntu 16.04 LTS Desktop to be performed by the user
- any 3rd party dependencies will not be distributed with source code and must be installed by the user before building application source code
- *automated script based on autotools or cmake to generate build script*, this is to simplify installation process across platforms 
 
## References

- page 120 in project notes
- page 37 in project notes


