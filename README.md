# AstroShooter

An old-fashioned astroid shooter game made in C++ using sfml library.

```
Currently the build system available are:
  MS Build (Visual Studio)
  GNU Make
```
If you are a **beginer**, try to 
- Contribute and fix the issues that will be posted.
- Add new issues.
- Create a CMAKE file

## Requirements

#### Must Have (dependencies)
- SFML 2.4.1 or above - http://www.sfml-dev.org/
##### Linux
    
    ```
     sudo apt-get install libsfml-dev
    ```
    
##### Windows
- Vist www.sfml-dev.org and download the lastest version of the library.
- In project settings,
 1. Add the `</path/to/sfml/>/include` to your **c++ include path**.
 2. Then add `</path/to/sfml/>/lib` to the **linker extra library path**.
      
#### MS Build
 * Visual Studio 15 or above
 * VC++ 15 or above

#### Linux
 * g++ 4.8 or above
 * GNU Make
  
## Building from source

### First Method (prefered and used) MS Build

  * Download the source code.
  * Import into Visual Studio
  * Build according to your need (x64 or x86 or whatever)
   
    ```
    Don't forget to link the sfml libraries - graphics, window and system.
    put the following files in linker input:
     sfml-system-d.lib, sfml-window-d.lib, sfml-graphics-d.lib and GameMenu.lib
    ```
  * Copy the sfml dlls (window, graphics and system) along with the executable
  * Then Run and enjoy
  
### Second Method Makefile
(Build will work, but the application is untested)

 * Download source code
 * cd to directory
  eg
  
  ```
   $ cd sfml-snake
  ```
 * Run make
 
  ```
   $ make
  ```
 * the exectable `bin/game` should apear. The directory will be created if doesn't exist.
 * Run and enjoy
