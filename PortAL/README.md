# Portable Application Launcher (PortAL)

**Graphical User Interface Productivity** is a set of tools to improve usability, productivity and accessibility of the standard win32 interface while keeping the tools as light as possible.

**PortAL** is a part of *GUIPro*, it is a portable and light application launcher which use hotkeys and popup menus.

> An open source application launcher.
> Design for portable mode (usb key for example). 
> Low cost memory, hotkey access, application icons in menu.

The PortAL configuration is a XML file which contains the structure of menus and application shortcuts.

## Installation

Download binaries from: http://glatigny.github.io/guipro/ (or in the [releases section](https://github.com/glatigny/guipro/releases))

## Configuration

Copy the `portal.xml` file in the `%appdata%\Guipro\` folder.
The application will first look in your application data, then in the binary folder.

```XML
<?xml version="1.0" encoding="UTF-8"?>
<portal skin="default">
  <!--
  This group will be put in the systray
   Win+Z will open the menu under your mouse
  -->
  <group key="Z">
    <!-- A Separator can also be a title -->
    <sep name="My menu"/>
    <!-- The first Application, will launch the command line in the C:\ folder -->
    <app name="command line" exe="%system%\cmd.exe" path="C:\"/>
    <!-- Second application entry, for Notepad. Can also by launch thanks to Win+N hotkey -->
    <app name="notepad" exe="%win%\notepad.exe" key="N"/>
    <!-- A sub-group will create a submenu -->
    <group name="A sub menu">
      <sep name="Sub menu title"/>
      <!-- mod is the modifier : C for Ctrl. A for Alt. W for Win. S for Shift -->
      <quit name="A &quot;quit&quot; item" mod="CA" key="Q"/>
    </group>
    <!-- That submenu have an accelerator. Press "x" to open directly that entry -->
    <group name="E&amp;xplorers">
      <app name="explorer" exe="%win%\explorer.exe" />
      <!-- Override the icon of an item via the "ico" parameter -->
      <app name="explorer with another icon" exe="%win%\explorer.exe" ico="%win%\explorer.exe,2"/>
    </group>
    <!-- Open any kind of file thanks to the "shell execute" -->
    <app name="Portal Documentation" exe="%portal%\doc\index.html" shell="true"/>
    <!-- Open a file directly with notepad -->
    <app name="Edit this config" exe="%win%\notepad.exe" param="%portal%\portal.xml" ico="%portal%\portal.xml" />
    <sep/>
    <quit/>
  </group>
</portal>
```

For more details regarding the configuration, see `configuration.md`

## Compilation

PortAL is Windows only.
You can use the Visual Studio Project.
```
msbuild /p:Configuration=Release;Platform=x64
```
A makefile is also available but in work in progress.

### Experimentations

The `common.h` file contains some defines that you can uncomment to enable *work in progress* features.
```c
/* -- Enabled settings -- */
/* Icon manager system */
#define ICON_MANAGER

/* -- Disabled settings -- */
/* Old rules for the loading of the configuration. */
#define PORTAL_REFUSE_HOTKEY_ERROR
/* Use GDI+ to handle and display the icons (experimental) */
#define USE_GDI_MENU
/* Add an extra layer to draw the menu and avoid the white borders (experimental) */
#define DEV_MENU_DESIGN_SUBCLASSING_FOR_BORDER
/* Path Storage feature to reduce memory footprint (experimental) */
#define USE_PATH_STORAGE
```

## License

* Copyright (C) 2007 - 2024 Glatigny Jerome. All rights reserved.
* Distributed under the GNU General Public License version 2 or later