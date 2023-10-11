# ezΩManager v1.0.0  

Hotkey/Hotstring manager.  
It is "better version of [ezΩ.ahk](https://github.com/BartolHrg/easyOmega)".  
It can:  
* enable/diable hotkeys  
* make replacements  
* open other GUIs  
* run anything (any command: run AHK functions, DLLs, terminal commndas)  


Before running, run `init.cmd/.sh` to install [UIAutomation dependency](#disclaimer-for-uiautomation-dependency)  
Instructions for running AutoHotKey are in root [../README.md](../README.md).  
Run file `ezΩManager.ahk2`.  

At the moment, project is incomplete:  
v1.0.0 has only Greek letter support and sub/superscript.  
I will add more functionality in later versions.  

Of course, it is encouraged to modify the script's source code to adjust to personal preferences.  

To use it, press key `SC029` which is left of key `1` (below `Esc`),  
and type in a command, and press `Tab`.  
If you don't want to execute anything, press `Esc`.  
To reset everything, simply press `SC029` again.  

Example: type in the `greek` command.  
Now, when you type on your keyboard, greek letters show up.  
(Typing `abc` produces `αβς`)  

# Disclaimer for UIAutomation dependency  

I don't wanna bother with copyright and licenses  
So if you use ezΩManager, either  
* download [UIAutomation v2 github](https://github.com/Descolada/UIA-v2)  
  So that `UIA.ahk` is in location `ezΩManager/UIAutomation/Lib/UIA.ahk`  
* run `init.cmd/.sh`  

