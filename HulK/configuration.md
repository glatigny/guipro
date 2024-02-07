## The HulK configuration file

The configuration file is a XML file named **hulk.xml**.
By default HulK will load the file in the same folder than the binary ; but you can create your personal configuration file
```
%localappdata%\GUIPro\hulk.xml
```

On recent Windows Operating Systems, you need administrator rights to edit a file in the `Program Files` folder. Using a configuration in your local application data folder will be easier for you to edit and if you have administrator rights, you can customize the default configuration file for every user in the computer.

### HulK main element

The configuration is a simple XML file but it has to follow some rules.
```XML
<?xml version="1.0" encoding="UTF-8"?>
<hulk>
	<plugins>
		<!-- List of plugins to load and their settings -->
	</plugins>
	<mouse>
		<!-- Mouse actions -->
	</mouse>
	<hotkeys>
		<!-- Keyboard hotkeys actions -->
	</hotkeys>
</hulk>
```

### Plugin nodes

By default, all HulK plugins are included in the configuration but not all of them are set-up to be loaded at startup.
Each plugin can be configured with:
```XML
<plugin name="pluginName" load="true"/>
```
Where current `pluginName` are : `wdhook`, `systemmenu` and `desktops`.

The plugin can have further options which are configured with:
```XML
<plugin name="systemmenu" load="true">
	<option name="active" value="true"/>
</plugin>
```
The `active` setting indicates if the plugin will be loaded as active, since it can be activate (and deactivate) via a keyboard hotkey.

### Mouse nodes

The nodes under the `<mouse>` section are actions that you want to handle.
These are: `drag`, `resize` and `switch`.
For each action node, you can define the mouse boutton (`btn`) and the keyboard modifier (`mod`) to activate the action.

```XML
<drag btn="left" mod="alt"/>
<resize btn="right" mod="alt"/>
```

#### Drag

Allows you to drag the pointed window using your mouse.

#### Resize

Allows you to resize the pointed window using your mouse.
The closest corner of the window will be catch to your mouse movements.

#### Switch (still experimental)

Allow you to change the window with the focus using your mouse wheel.

### Hotkey nodes

Each hotkey node will define a keyboard hotkey (a key with a modifier) and an action to perform.
That action can be an internal HulK action or a plugin action.
Depending the action, you can indicate if you want to target the window with the focus or the window pointed by your mouse.

```XML
<hotkey key="222" mod="win" action="minimize" target="pointed"/>
```

The internal actions are:
 * `minimize` to reduce the (target) window
 * `close` to close the (target) window
 * `maximize` to maximize the (target) window
 * `iconize` to send the (target) window to the systray
 * `traynize` to send the (target) window to the HulK systray menu
 * `uniconize` to restore the latest iconize/traynize window
 * `alwtop` to set or remove the *always on top* flag on the (target) window
 * `borderless` to set or remove the *borderless* flag on the (target) window

Plugins have the action `activate` which allows to activate or deactiate manually the plugin while HulK is running.

The `desktops` actions are:
 * `desktopleft` to change the current desktop (if possible)
 * `desktopright` to change the current desktop (if possible)
 * `moveleft` to move the window to another desktop (if possible)
 * `moveright` to move the window to another desktop (if possible)
 * `switchleft` to move the window to another desktop and follow it (if possible)
 * `switchright` to move the window to another desktop and follow it (if possible)

`wdhook` and `systemmenu` plugins don't have specific hotkey actions.

## Hotkeys

### Keys
There is three differents way to configure a key in PortAL.

* A letter or a number like `A` or `1`
* A special name like `F1`, `NUM_0` or `SPACE`.
* The keycode number like `222` (which is the key just below "escape")

[[View the special key binding|PortAL-KeyBinding]]

### Buttons
Only four values are available for buttons: `left`, `right`, `middle` and `wheel`.

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