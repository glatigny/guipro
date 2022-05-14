#
# GUIPro - HulK - Plugins
#
.PHONY : all
all : plugins

LOCAL_OUTBIN_FOLDER="..\$(OUTBIN_FOLDER)"
LOCAL_OBJ_FOLDER="..\$(OBJ_FOLDER)"
LOCAL_INCLUDE_FOLDER="..\$(INCLUDE_FOLDER)"

plg_desktops:
	@echo Generating plugin: Desktops
	@cd desktops && @$(MAKE) /NOLOGO $(MFLAGS) OUTBIN_FOLDER="$(LOCAL_OUTBIN_FOLDER)" OBJ_FOLDER="$(LOCAL_OBJ_FOLDER)" INCLUDE_FOLDER="$(LOCAL_INCLUDE_FOLDER)"

plg_systemmenu:
	@echo Generating plugin: systemmenu
	@cd systemmenu && @$(MAKE) /NOLOGO $(MFLAGS) OUTBIN_FOLDER="$(LOCAL_OUTBIN_FOLDER)" OBJ_FOLDER="$(LOCAL_OBJ_FOLDER)" INCLUDE_FOLDER="$(LOCAL_INCLUDE_FOLDER)"

plg_wdhook:
	@echo Generating plugin: wdhook
	@cd wdhook && @$(MAKE) /NOLOGO $(MFLAGS) OUTBIN_FOLDER="$(LOCAL_OUTBIN_FOLDER)" OBJ_FOLDER="$(LOCAL_OBJ_FOLDER)" INCLUDE_FOLDER="$(LOCAL_INCLUDE_FOLDER)"

plugins: plg_desktops plg_systemmenu plg_wdhook
