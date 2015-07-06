;Include Modern UI
!include "MUI2.nsh"
!include "nsDialogs.nsh"
!include "InstallOptions.nsh"
!include "WinMessages.nsh"
!include "x64.nsh"

;--------------------------------
;General
	;Name and file
	Name "Portable Application Launcher"
	OutFile "PortAL-1.5.0.exe"
	BrandingText "Graphical User Interface Productivity : PortAL"

	!define PRODUCT_NAME "PortAL"
	!define PRODUCT_VERSION "1.5.0"
	!define PRODUCT_PUBLISHER "Graphical User Interface Productivity"
	!define PRODUCT_WEB_SITE "http://obsidev.github.io/guipro/"
	!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
	!define PRODUCT_UNINST_ROOT_KEY "HKLM"

	!define WND_CLASS "_Portal90_PortAL_MainWnd_"
	!define WND_TITLE "PortAL"
	
	;Default installation folder
	InstallDir "$PROGRAMFILES64\GUIPro\PortAL"

	;Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\GUIPro\PortAL" ""

	Icon "usb.minimal.install.ico"
	!define MUI_ICON "usb.minimal.install.ico"
	UninstallIcon "usb.minimal.uninstall.ico"
	!define MUI_UNICON "usb.minimal.uninstall.ico"

	XPStyle on
	;Request application privileges for Windows Vista
	RequestExecutionLevel user

	SetCompressor /SOLID lzma
	
;--------------------------------
;Interface Configuration

	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP "header.bmp"
	!define MUI_WELCOMEFINISHPAGE_BITMAP "bigHeader.bmp"
	!define MUI_COMPONENTSPAGE_CHECKBITMAP "${NSISDIR}\Contrib\Graphics\Checks\simple-round2.bmp"
	!define MUI_ABORTWARNING

	!define MUI_FINISHPAGE_TITLE "PortAL Installed"
	!define MUI_FINISHPAGE_TEXT "PortAL was successfully installed"
	!define MUI_FINISHPAGE_RUN "$INSTDIR\PortAL.exe"
	!define MUI_FINISHPAGE_RUN_TEXT "Start PortAL"
	!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\doc\index.html"
	!define MUI_FINISHPAGE_SHOWREADME_TEXT "Read the documentation"
	!define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED

	!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of PortAL : Portable Application Launcher.$\r$\n$\r$\nIt is recommended that you close current $\"Portable Application Launcher$\" applications.$\r$\nThis setup doesn't need to reboot your computer.$\r$\n$\r$\nClick Next to continue."

;--------------------------------
;Pages

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "../License.txt"
	!insertmacro MUI_PAGE_COMPONENTS
	Page custom MUI_Version_PageFct
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_PAGE_FINISH

	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_COMPONENTS
	!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"
	
	LangString runningMsg ${LANG_ENGLISH} "${WND_TITLE} is running.$\nDo you want to close it?"
	LangString termMsg ${LANG_ENGLISH} "Installer cannot stop running ${WND_TITLE}.$\nDo you want to terminate process?"

;--------------------------------
;Custom page
Function MUI_Version_PageFct
	ReserveFile "selectVersion.ini"
	!insertmacro INSTALLOPTIONS_EXTRACT "selectVersion.ini"
	!insertmacro INSTALLOPTIONS_DISPLAY "selectVersion.ini"
FunctionEnd

;--------------------------------
;Close Application
!macro TerminateApp
	Push $0 ; window handle
	Push $1
	Push $2 ; process handle
	FindWindow $0 '${WND_CLASS}' ''
	IntCmp $0 0 TerminateAppDone
	MessageBox MB_YESNO|MB_ICONEXCLAMATION "$(runningMsg)" /SD IDYES IDYES TerminateAppClose IDNO TerminateAppDone
 TerminateAppClose:
	System::Call 'user32.dll::GetWindowThreadProcessId(i r0, *i .r1) i .r2'
	System::Call 'kernel32.dll::OpenProcess(i 0x00100001, i 0, i r1) i .r2'
	SendMessage $0 ${WM_CLOSE} 0 0 /TIMEOUT=2000
	System::Call 'kernel32.dll::WaitForSingleObject(i r2, i 2000) i .r1'
	IntCmp $1 0 TerminateAppEnd
	MessageBox MB_YESNO|MB_ICONEXCLAMATION "$(termMsg)" /SD IDYES IDYES TerminateAppTerminate IDNO TerminateAppEnd
	System::Call 'kernel32.dll::CloseHandle(i r2) i .r1'
	Quit
 TerminateAppTerminate:
	System::Call 'kernel32.dll::TerminateProcess(i r2, i 0) i .r1'
 TerminateAppEnd:
	System::Call 'kernel32.dll::CloseHandle(i r2) i .r1'
 TerminateAppDone:
	Pop $2
	Pop $1
	Pop $0
!macroend

;--------------------------------
;Installer Sections
InstType "Portable"
InstType "Install On Computer"  
InstType "Full"  

;--------------------------------
;Installer Sections

Section "!PortAL" SecInstall
	SectionIn RO
	
	SetOutPath "$INSTDIR"
	!insertmacro TerminateApp
	
	; VC2005 SP1 : http://www.microsoft.com/downloads/details.aspx?FamilyID=200b2fd9-ae1a-4a14-984d-389c36f85647
	; VC2008 SP1 : http://www.microsoft.com/downloads/details.aspx?familyid=A5C84275-3B97-4AB7-A40D-3802B2AF5FC2
	
;/*	# Official part, for multiple VCRedist versions
	# remove the ";" of this section and add a ";" before the "/*" of the next "section"

	!insertmacro INSTALLOPTIONS_READ $R0 "selectVersion.ini" "Field 2" "State"
	IntCmp $R0 1 installAuto
	!insertmacro INSTALLOPTIONS_READ $R0 "selectVersion.ini" "Field 3" "State"
	IntCmp $R0 1 installx86
	!insertmacro INSTALLOPTIONS_READ $R0 "selectVersion.ini" "Field 4" "State"
	IntCmp $R0 1 installx64
	
installAuto:
	${If} ${RunningX64}
		File /oname=PortAL.exe x64\PortAL.exe
	${Else}
		File /oname=PortAL.exe x86\PortAL.exe
	${EndIf}
	Goto PortalExeOk
installx86:
	File /oname=PortAL.exe x86\PortAL.exe
	Goto PortalExeOk

installx64:
	File /oname=PortAL.exe x64\PortAL.exe
	Goto PortalExeOk
;*/

/*	# Local part, for single VCRedist versions
	# just comment the first line of this comment section and remove the ";" of the previous "section"
	
	File ..\PortAL.exe
;*/

PortalExeOk:
	File ..\portal.txt
	File ..\license.txt
	
	SetOverwrite off
	File ..\portal.xml
	SetOverwrite on
	
	SetOutPath "$INSTDIR\doc"
	File /r /x .svn ..\doc\*.*

SectionEnd

SectionGroup /e "Install On Computer"
	RequestExecutionLevel admin
	
	Section /o "Uninstaller" secUninstall
		SectionIn 2 3
		;Create uninstaller
		WriteUninstaller "$INSTDIR\Uninstall.exe"

		;Store installation folder
		WriteRegStr HKCU "Software\GUIPro\PortAL" "" $INSTDIR
		
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" 		"Portable Application Launcher"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" 	"$\"$INSTDIR\Uninstall.exe$\""
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon"		"$\"$INSTDIR\PortAL.exe$\""
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" 	"${PRODUCT_VERSION}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" 		"${PRODUCT_WEB_SITE}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" 			"${PRODUCT_PUBLISHER}"
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoModify" 			1
		WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "NoRepair"			1
	;	WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
	SectionEnd

	Section /o "Autorun" secAutorun
		SectionIn 2 3
		WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Run" "PortAL" "$INSTDIR\PortAL.exe"
	SectionEnd

	Section /o "Start menu" setStartMenu
		SectionIn 3
		CreateDirectory "$SMPROGRAMS\GUIPro"
		CreateDirectory "$SMPROGRAMS\GUIPro\PortAL"
		CreateShortCut "$SMPROGRAMS\GUIPro\PortAL\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
		CreateShortCut "$SMPROGRAMS\GUIPro\PortAL\PortAL.lnk" "$INSTDIR\PortAL.exe" "" "$INSTDIR\PortAL.exe" 0
		CreateShortCut "$SMPROGRAMS\GUIPro\PortAL\documentation.lnk" "$INSTDIR\doc\index.html" "" "$INSTDIR\doc\index.html" 0
		CreateShortCut "$SMPROGRAMS\GUIPro\PortAL\edit config.lnk" "$WINDIR\notepad.exe" "$INSTDIR\portal.xml" "" 0
	SectionEnd

SectionGroupEnd

Function .onInit
	SetRegView 64
	${EnableX64FSRedirection}
/*	
	SectionSetText ${SecInstall} ""
	SectionSetSize ${SecInstall} 100
	SectionSetText ${secUninstall} ""
	SectionSetSize ${secUninstall} 100
	SectionSetText ${secAutorun} ""
	SectionSetSize ${secAutorun} 100
	SectionSetText ${setStartMenu} ""
	SectionSetSize ${setStartMenu} 100
*/
FunctionEnd

;--------------------------------
;Descriptions

	;Language strings
	LangString DESC_InstallSec 		${LANG_ENGLISH} "The main application. (Required)"
	LangString DESC_UninstallSec 	${LANG_ENGLISH} "Create an uninstall program."
	LangString DESC_AutorunSec 		${LANG_ENGLISH} "Launch PortAL when your session starts."
	LangString DESC_StartMenuSec 	${LANG_ENGLISH} "Add PortAL shortcuts in your start menu."

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecInstall} 	$(DESC_InstallSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${secUninstall} 	$(DESC_UninstallSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${secAutorun} 	$(DESC_AutorunSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${setStartMenu} 	$(DESC_StartMenuSec)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "!un.Uninstall Portal" secUnRemove
	SectionIn RO
	Delete "$INSTDIR\PortAL.exe"
	Delete "$INSTDIR\portal.txt"
	Delete "$INSTDIR\license.txt"
	Delete "$INSTDIR\Uninstall.exe"

	RMDir /r "$SMPROGRAMS\GUIPro\PortAL"
	RMDir /r "$INSTDIR\doc"
	RMDir "$SMPROGRAMS\GUIPro"
	RMDir "$INSTDIR"

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\PortAL"
	DeleteRegKey /ifempty HKCU "Software\GUIPro\PortAL"

SectionEnd

Section /o "un.Remove Configuration" secUnConfig
	Delete "$INSTDIR\PortAL.xml"
	RMDir "$INSTDIR"
SectionEnd

;--------------------------------
; Uninstall Descriptions

	;Language strings
	LangString DESC_Un_RemoveSec	${LANG_ENGLISH} "Remove PortAL"
	LangString DESC_Un_ConfigSec	${LANG_ENGLISH} "Delete your config file (portal.xml)"

	;Assign language strings to sections
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${secUnRemove} 	$(DESC_Un_RemoveSec)
	!insertmacro MUI_DESCRIPTION_TEXT ${secUnConfig} 	$(DESC_Un_ConfigSec)
	!insertmacro MUI_UNFUNCTION_DESCRIPTION_END