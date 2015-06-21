Graphical User Interface Productivity (GUIPro)
Portable Application Launcher (PortAL)
-----------------------------------------------

Graphical User Interface Productivity tools is a set of tools to improve usability, productivity and accessibility of the standard win32 interface while keeping the tools as light as possible.
PortAL is a part of GUIPro, it is a portable and light application launcher which use hotkeys and popup menus.

The PortAL configuration is a XML file which contains the structure of menus and application shortcuts.

The root node ( <portal> ) could have several attributes.
 * defaultshell="true" - start all applications using the shell (ShellExecute)
 * showshortcut="true" - display by default shortcuts in the menu

Children of the root node will be displayed in the System Tray.
So if you add an application as a root child, a link for this application will be displayed in the systray.
There is two main type of nodes : group, app.
All yes/no attributes have as default value "no". You may configure it with keywords "yes/true".

The group node ( <group ...> ) could be configure with theses attributes:
 * name - 
 * key - 
 * mod - 
 * ico - 
 * autoopen - [yes/no]

The application node ( <app .../> ) could be configure with theses attributes:
 * name - 
 * key - 
 * mod - 
 * exe - 
 * ico - 
 * path - 
 * param - 

 * shell - [yes/no] - Launch the application using shell execution (can launch non executable files)
 * browse - [yes/no] - Display the content of the directory
 * showsc - [yes/no] - Display the shortcut information in the menu. You can use "no/false" if portal:showshortcut is activated.
 * default - [yes/no] - Display the elemnet as default
 * autorun - [yes/no] - Start the application at portal startup
 * break - [yes/no] - Display the element in a new column

There is another types of nodes which could be added in a group node.
 * sep - Create a separator.
 * quit - Create an menu entry which allow you to quit PortAL. This item is automatically added in systray menus.
 * about - Create an menu entry which allow you to show the about balloon. This item is automatically added in systray menus.
 * reload (deprecated)

Theses nodes could work without any attribute but you can still override somes.
 * name - Override the name. Add one name if it is a separator.
 * ico - Override the icon (Do not work on separators).
 * key/mod - Configure and attached an hotkey to this item (Do not work on separators).

------------------------------------------------
                CHANGELOG
------------------------------------------------

------------------------
Release: 1.5.0
Date   : 2015-06-22
------------------------

* Addition of a skin system.
* Addition of the event attribute.
* Addition of the variables section to define internal variables.
* Addition of the clipboard variables.
* Addition of the %config% variable.
* Improvement of the file notification system.
* Fix memory leak when reloading the configuration.
* Avoid error when a percent character is not forming a variable.
* Avoir error when loading a config file with an empty group.

------------------------
Release: 1.4.7
Date   : 2015-02-8
------------------------

* Stop quitting when there is an hotkey conflict.
* Removing some alert boxes to use tray balloons instead.
* Do not open the website with generic balloons, only with the about one.
* Updating website url
* Using the latest pugixml library

------------------------
Release: 1.4.6
Date   : 2014-03-15
------------------------
 * Transforming path with relative elements into real path (bug #22)

------------------------
Release: 1.4.5
Date   : 2013-05-10
------------------------
 * New '-config" parameter (Feature Request #18)
 * Fix with variable parsing (bug #21)

------------------------
Release: 1.4.4
Date   : 2013-01-22
------------------------
 * Adding support for Windows 8
 * Can override system hotkeys

------------------------
Release: 1.4.3
Date   : 2012-09-02
------------------------
 * XML Library modification : using pugixml instead of TinyXML.
 * Merging xmlconfig with PortAL
 * new "break" option

------------------------
Release: 1.4.2
Date   : 2011-12-14
------------------------
 * New menu design
 * Shortcut display in menus (showsc)
 * Dynamic loading of sub menus
 * Icon manager
 * New "browse" option
 * Add a link in the balloon notification

------------------------
Release: 1.4.1
Date   : 2011-08-26
------------------------
 * Set multiple hotkeys for groups and applications
 * Minors fixes

------------------------
Release: 1.4.0
Date   : 2010-12-01
------------------------
 * Improving Windows Vista/Seven compatibility
 * Support of keyboard naviguation in menus
 * 64bits version