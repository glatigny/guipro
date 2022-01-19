## The PortAL configuration file

The configuration file is a XML file named **portal.xml**.
Portal automatically reload the configuration when the XML file is modified.
By default PortAL will load the file in the same folder than the binary ; but you can create your personal configuration file
```
%localappdata%\GUIPro\portal.xml
```

On recent Windows Operating Systems, you need administrator rights to edit a file in the `Program Files` folder. Using a configuration in your local application data folder will be easier for you to edit and if you have administrator rights, you can customize the default configuration file for every user in the computer.

### Portal main element

The configuration is a simple XML file but it has to follow some rules.
```XML
<?xml version="1.0" encoding="UTF-8"?>
<portal>
  <!-- Your configuration here -->
</portal>
```
The file must contain a `portal` root element (and only one).

This root element have two possible parameters.
```XML
<portal defaultshell="false" showshortcut="true" skin="default">
```
* **defaultshell** (_false_/_true_) indicates if you want to launch by default the applications using the windows shell or if you let PortAL execute the application.
  The windows shell has the possibility to open documents. If the "application" is a HTML file, it will open your default browser to open the page.
* **showshortcut** (_false_/_true_) indicates if you want to display (by default) the application shortcut in the PortAL menu.
* **skin** (_name_) indicate the preset skin that you want to use ; you can otherwise create your own skin (describe below in the documentation).
  There is for the moment three skins: **default**, **flat** and **dark**.
  If you do not specify a value, it will use the _fourth_ skin based on your operation system configuration.

The elements you will place directly in the `portal` root will be displayed in the system tray (systray).
* If you place a `group`, PortAL will display a menu when you will click on the systray icon.
* If you place an `app`, PortAL will launch directly the application when you will click on the systray icon.

### Generic node

Every node (`group`, `app` or the special nodes) have the basic same parameters.
They can also have some specific parameters, specially for the `app` node.
```XML
<app name="Name" key="" mod="" over="false" ico="" default="false" showsc="false" break="false"/>
```

* **name** (_text_) the display name of the node. If you place an `&` (`&amp;` to respect the XML convention) the next letter will become the accelerator key while browsing the menu.
* **key**/**mod** the hotkey (combination of the key and his modifier).
  Please check the HotKey part of his documentation for more details.
* **over** (_true_/_false_) is the option to activate the hotkey override.
* **ico** (_file path_) specify an icon for the group, which will be display in the systray or in the menu depending the context.
* **default** (_true_/_false_) the node will be display as the default item in the menu (in bold)
* **showsc** (_true_/_false_) override the default configuration for "show shortcut".
* **break** (_true_/_false_) display a menu break after the item (it will create a new column in the menu).

Note that you can assign several hotkeys for a node (`group`, `app`, etc).

### Group

A `group` contain other `group`/`app`/_special_ nodes, PortAL display the group as a menu or a sub-menu.
Here the minimal requirement for a group node:
```XML
<group name="Group name">
  <!-- group content -->
</group>
```

If the group is directly under the `portal` root, it will be display in the systray.
If the group is in another group, it will be display as a sub-menu of his parent group.

By activating the hotkey (or several), PortAL will open the group and display it as a popup menu directly where your mouse is.
```XML
<group name="Group name" key="" mod="" over="" ico="">
  <!-- group content -->
</group>
```

The `group` node have a special parameter which allow PortAL to automatically open a systray menu when the mouse is moving over the systray icon.
```XML
<group name="Group name" autoopen="true">
```
_This option is not recommended but it has been requested !_

### Application

The application is one of the most important node in the PortAL configuration. It allow you to launch an application, to open a file or even more !

Here a generic `app` node:
```XML
<app name="Name" exe="filepath" param="" path="" />
```
* **exe** (_text_) is the application you want to execute.
* **param** (_text_ / _optional_) are the parameter that you will pass to the application.
* **path** (_text_ / _optional_) is the execution path that you can override if you want to launch the application to start in a specific path (like a shell or a window explorer).

By default, PortAL will load the icon of the **exe** to display it in the menu or the systray (depending the context).
But you can override it ; like some other elements.
```XML
<app name="Name" exe="filepath" ico="filepath" shell="true" elevate="false" events="start,exit" />
```
* **shell** (_true_/_false_) override the default configuration for "shell execute".
* **autorun** _deprecated_ (_true_/_false_) will indicate that the application will be automatically launch when PortAL is executed (_like autoopen, this feature was a special request_). Please use _events start_ instead.
* **elevate** (_true_/_false_) if you are not using the "shell" option, PortAL can launch the application with administrator elevation (depending your OS configuration, you might have a elevation approval alert box).
* **events** (_text_) specify the different events when the application will be executed. You can use several events separated by a comma.
 * start - launch when portal start.
 * exit - launch when just portal quit.
 * lock - _not yet available_
 * unlock - _not yet available_

More than launch an application, the `app` node can also browse a folder and list his content.
```XML
<app name="My documents" exe="%mydocs%" browse="true" param="*.*" path="true" />
```
* **browse** (_true_/_false_) activate the special browse feature.
* **exe (browse)** (_folder path_) indicates the folder that will be browse.
* **param (browse)** (_file extension filter_) filters the files which will be display in the menus.
* **path (browse)** (_true_/_empty value_) display or not the sub directories.

### Special nodes

#### Separator

```XML
<sep name=""/>
```

#### About
```XML
<about name=""/>
```

#### Reload (deprecated)
```XML
<reload name=""/>
```

#### Quit
```XML
<quit name=""/>
```

### Variables
The `variables` node is a special node under the `portal` root node.
It allows you to create new variables which could be use afterwards in your configuration file.

Here an example:
```XML
<portal>
  <variables>
    <var name="test" value="my test value"/>
    <var name="docs" value="%mydocs%/documents/"/>
  </variables>
  <group>
     <!-- your config where you can use %test% or %docs% -->
  </group>
</portal>
```

### Skin
There is two way to set a skin in Portal ; with the ```skin``` attribute for the ```portal``` node (see above in the documentation) or using the ```skin``` node. That node allow you to create your own skin.

The ```skin``` node have three possible attributes:
* **base** (_color pair_) defines the text color and the background color for all items.
* **selected** (_color pair_) define the text color and the background color for an item when selected.
* **edge** (_true_ / _sel_ / _unsel_ / empty value) define if there is an edge display between the icon and the text.

A **color pair** is formed by two colors separated by a comma or a space.
A color should be describe in hexadecimal format, you can add a ```#``` before in order to read it easily.

Please note that PortAL can understand color with 3 or 6 characters ; otherwise the black color will be returned.

The ```skin``` node can have three child node ; which are all optional.
* **border** (_border node_) composed of a border ```size```, border ```round``` and border ```color```. A non-referenced element will have a 0 value.
* **background** (_gradiant node_) composed of a ```color``` pair and a ```direction``` (_h_/_v_ ; optional).
* **icon** (_gradiant node_) composed of a ```color``` pair and a ```direction``` (_h_/_v_ ; optional).

Example (an ugly one):
```XML
<portal>
  <skin base="" selected="#000:#FCF4ED" edge="true">
    <border size="1" round="5" color="#ffffff"/>
    <background color="#000000:#ffffff" direction="v"/>
    <icon color="#ffffff:#000000"/>
  </skin>
</portal>
```

## Hotkeys

### Keys
There is three differents way to configure a key in PortAL.

* A letter or a number like `A` or `1`
* A special name like `F1`, `NUM_0` or `SPACE`.
* The keycode number like `222` (which is the key just below "escape")

[[View the special key binding|PortAL-KeyBinding]]

### Modifiers
By default, if you do not specialy a modifier, PortAL will use the `Win` key. So you have to use the `None` modifier if you do not want a modifier.
The modifier value ask for letters in order to determine which modifier will be used for your hotkey.
* C - Ctrl
* A - Alt
* S - Shift
* W - Win
* Other letter - None
Please note that PortAL need to uppercase to work properly.

`mod="C"` or `mod="Ctrl"` set the `Ctrl` modifier. You should press the Ctrl key with the another key configure with key value.

`mod="AS"` or `mod="Alt Shift"` set the `Alt and Shift` modifier. You should press the Alt key and Shift key with the another key configure with key value.

`mod="N"` is the standard to not use a modifier with the key.

### Override
The override option allow to bypass the system and create an hotkey on a system hotkey.
For example, `Win + E` is a generic system hotkey which start a file explorer.

Using `over="true"`, you would override the system an catch the original hotkey with PortAL. 

### Several hotkeys per node

PortAL handle up to 10 hotkeys per node. You can use the combo `key`/`mod`/`over` but you can also use `key1`/`mod1`/`over1`, then `key2`/`mod2`/`over2`, then `key3`/`mod3`/`over3`, etc.
Please note that if you do not configure `key2`, you won't be able to use `key3`

## Variables replacement

* `%win%` - The windows directory (generally C:\windows\ or C:\WINNT\ for Windows 2000 systems).
* `%programfiles%` - The Program Files directories (generally C:\program files\ and c:\program files x86\). Look into both directories on x64 platforms.
* `%programfiles86%` - The x86 program file folder while running under x64 platform.
* `%system%` - The System directory (generally C:\windows\system\).
* `%system86%` - The x64 System directory while running under x64 platform.
* `%mydocs%` - My documents directory.
* `%mymusic%` - My Music directory.
* `%myvideo%` - My Videos directory.
* `%desktop%` - The Desktop directory.
* `%appdata%` - The Application Data directory.
* `%portal%` - The PortAL directory.
* `%config%` - The current open configuration file.
* `%clipboard%` - The current clipboard content.
  It supports text and file. When there are several files in the clipboard, it only paste the first one.
* `%clipboards%` - The current clipboard content.
  It supports text and files. When there are several files in the clipboard, it paste them all with a space as separator.

PortAL also handle OS common variables and will perform some dynamic replacements.

## Icons

When you specify an icon for a node, you can select a specific icon in the file.
```XML
<app exe="%win%\explorer.exe" ico="%win%\explorer.exe"/>
```
Will use the default explorer icon ; at this moment the *ico* is optional because PortAL will by default take the icon from the *exe*.

```XML
<app exe="%win%\explorer.exe" ico="%win%\explorer.exe,6"/>
```
will use the 6° icon in the explorer binary file (in this case it is the full recycle bin icon).