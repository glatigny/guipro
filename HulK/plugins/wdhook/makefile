#
# GUIPro - HulK
#
PROGNAME = wdhook
BINARY = $(PROGNAME).dll
SRC = src

Configuration = Release
PlatformName = x64

# OBJ = Build\$(Configuration)\Objs_$(PlatformName)
# OUTBIN = Build\$(Configuration)\$(PlatformName)

OBJ = $(OBJ_FOLDER)\plugins\$(PROGNAME)
OUTBIN = $(OUTBIN_FOLDER)\plugins\$(PROGNAME)

# Some ENV variable are set-up by vs bat file...
#VSCMD_ARG_TGT_ARCH = x64
#VSCMD_ARG_HOST_ARCH = x86
#__VCVARSALL_TARGET_ARCH=X86

CC=cl.exe
RC=rc.exe
LINK=link.exe

.PHONY : all
all : app

APP_FILES = \
  $(OBJ)\common.obj \
  $(OBJ)\wdhook.obj \
  \
  $(OBJ)\wdhook.res

SYS_LIBS = \
  kernel32.lib \
  user32.lib \
  Msimg32.lib \
  gdi32.lib \
  shell32.lib \
  ole32.lib \
  oleaut32.lib
# Rest of defaults libs: uuid.lib advapi32.lib winspool.lib comdlg32.lib odbc32.lib odbccp32.lib

# Spliting - work in progress
CL_FLAGS_BASE = /nologo /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /FC /Gd /EHsc /WX- /Fo"$(OBJ)\\" /Fd"$(OBJ)\vc141.pdb" /errorReport:prompt /diagnostics:classic 
CL_FLAGS_RELEASE = /D "NDEBUG" /MDd 
CL_FLAGS_DEBUG = /D "_DEBUG" /MT /RTC1 
CL_FLAGS_X64 =
CL_FLAGS_X86 = 
#
LD_FLAGS_BASE = /ERRORREPORT:PROMPT /NOLOGO /NXCOMPAT /DYNAMICBASE /SUBSYSTEM:WINDOWS /TLBID:1 /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /PDB:"$(OUTBIN)\$(PROGNAME).pdb" /IMPLIB:"$(OUTBIN)\$(PROGNAME).lib"
LD_FLAGS_RELEASE = /OPT:REF /OPT:ICF /INCREMENTAL:NO /LTCG:incremental
LD_FLAGS_DEBUG = /DEBUG /INCREMENTAL
LD_FLAGS_X64 = /MACHINE:X64
LD_FLAGS_X86 = /MACHINE:X86 /SAFESEH
# + $(SYS_LIBS)

# Current flags
# CL_FLAGS = /c /Zi /nologo /W3 /WX- /diagnostics:classic /O1 /Ot /Oy /GL /D WIN32 /D NDEBUG /D _WINDOWS /D _UNICODE /D UNICODE /Gm- /EHsc /MT /GS /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fd"$(OBJ)\vc141.pdb" /Gd /TP /FC /errorReport:prompt
CL_FLAGS = /c /Zi /nologo /W4 /WX- /diagnostics:classic /O2 /GL /D WIN32 /D NDEBUG /D _WINDOWS /D _USRDLL /D XMLCONFIG_EXPORTS /D _WINDLL /D _UNICODE /D UNICODE /Gm- /EHsc /MT /GS /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fd"$(OBJ)\vc141.pdb" /Gd /TP /FC /errorReport:queue
# LD_FLAGS = /ERRORREPORT:PROMPT /INCREMENTAL:NO /NOLOGO $(SYS_LIBS) /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /PDB:"$(OUTBIN)\$(PROGNAME).pdb" /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /LTCG:incremental /TLBID:1 /DYNAMICBASE /NXCOMPAT /IMPLIB:"$(OUTBIN)\$(PROGNAME).lib" /MACHINE:X64 /DLL
LD_FLAGS = /ERRORREPORT:QUEUE /INCREMENTAL:NO /NOLOGO /LIBPATH:"$(LIBPATH)" $(SYS_LIBS) /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /DEBUG /PDB:"$(OUTBIN)\$(PROGNAME).pdb" /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /LTCG:incremental /TLBID:1 /DYNAMICBASE:NO /IMPLIB:"$(OUTBIN)\$(PROGNAME).lib" /MACHINE:X64 /DLL

# EXPORT_DEF = /DEF:"src\Source.def"

# Generatings objs from sources
{$(SRC)}.cpp{$(OBJ)}.obj ::
  @echo compiling: $<
  @$(CC) $(CL_FLAGS) /Fo"$(OBJ)\\" /I"$(INCLUDE_FOLDER)" $<

# Generating resources
{$(SRC)}.rc{$(OBJ)}.res ::
  @echo creating ressources: $<
  @$(RC) /D _UNICODE /D UNICODE /l"0x0409" /nologo /fo"$(OBJ)\wdhook.res" $<

# Creation of working folder
folders:
  @echo Folder: $(OBJ)
  @if not exist $(OBJ) @echo Creating Objs folder...
  @if not exist $(OBJ) mkdir $(OBJ)
  @if not exist $(OBJ)\pugixml mkdir $(OBJ)\pugixml
  @if not exist $(OUTBIN) @echo Creating Output folder...
  @if not exist $(OUTBIN) mkdir $(OUTBIN)

app: folders $(APP_FILES)
  @echo Generating $(BINARY)
  @$(LINK) /OUT:"$(OUTBIN)\$(BINARY)" $(APP_FILES) $(LD_FLAGS)