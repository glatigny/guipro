# Hotkey Unlimited (HulK)

**Graphical User Interface Productivity** is a set of tools to improve usability, productivity and accessibility of the standard win32 interface while keeping the tools as light as possible.

**HulK** is a part of *GUIPro*

## Installation

Download binaries from: http://glatigny.github.io/guipro/ (or in the [releases section](https://github.com/glatigny/guipro/releases))

## Configuration

Copy the `hulk.xml` file in the `%appdata%\Guipro\` folder.
The application will first look in your application data, then in the binary folder.
```xml
<hulk>
	<plugins>
		<plugin name="wdhook" load="false"/>
		<plugin name="systemmenu" load="true">
			<option name="active" value="true"/>
		</plugin>
		<plugin name="desktops" load="true"/>
	</plugins>
	<mouse>
		<drag btn="left" mod="alt"/>
		<resize btn="right" mod="alt"/>
		<!-- <switch btn="wheel" mod="alt"/> -->
	</mouse>
	<hotkeys>
		<hotkey key="222" mod="win" action="minimize" target="pointed"/>
		<hotkey key="escape" mod="win" action="close" target="pointed"/>
		<hotkey key="left" mod="win alt" action="moveleft" plugin="desktops"/>
		<hotkey key="right" mod="win alt" action="moveright" plugin="desktops"/>
	</hotkeys>
</hulk>
```
For more details regarding the configuration, see `configuration.md`

## Compilation

PortAL is Windows only.
You can use the Visual Studio Project.
```
msbuild /p:Configuration=Release;Platform=x64
```
A makefile is also available but in work in progress.

## License

* Copyright (C) 2007 - 2024 Glatigny Jerome. All rights reserved.
* Distributed under the GNU General Public License version 2 or later