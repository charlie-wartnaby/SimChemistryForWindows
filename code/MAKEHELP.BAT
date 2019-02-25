@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by lennard.HPJ. >"hlp\lennard.hm"
echo. >>"hlp\lennard.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\lennard.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\lennard.hm"
echo. >>"hlp\lennard.hm"
echo // Prompts (IDP_*) >>"hlp\lennard.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\lennard.hm"
echo. >>"hlp\lennard.hm"
echo // Resources (IDR_*) >>"hlp\lennard.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\lennard.hm"
echo. >>"hlp\lennard.hm"
echo // Dialogs (IDD_*) >>"hlp\lennard.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\lennard.hm"
echo. >>"hlp\lennard.hm"
echo // Frame Controls (IDW_*) >>"hlp\lennard.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\lennard.hm"
REM -- Make help for Project lennard


echo Building Win32 Help files
rem start /wait hcw /C /E /M "hlp\lennard.hpj"  ; Charlie: failed in WinXPPro, output not written before next step so changed to:
hcw /C /E /M "hlp\lennard.hpj"

if errorlevel 1 goto :Error1
if not exist "hlp\lennard.hlp" goto :Error2
if not exist "hlp\lennard.cnt" goto :Error3
echo.
if exist Debug\nul copy "hlp\lennard.hlp" Debug
if exist Debug\nul copy "hlp\lennard.cnt" Debug
if exist Release\nul copy "hlp\lennard.hlp" Release
if exist Release\nul copy "hlp\lennard.cnt" Release
echo.
goto :done

:Error1
echo hlp\lennard.hpj(1) : error: Problem encountered creating help file (error 1)
goto :done

:Error2
echo hlp\lennard.hpj(1) : error: Problem encountered creating help file (no lennard.hlp)
goto :done

:Error3
echo hlp\lennard.hpj(1) : error: Problem encountered creating help file (no lennard.cnt)
goto :done

:done
echo.
