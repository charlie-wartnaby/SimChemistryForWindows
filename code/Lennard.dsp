# Microsoft Developer Studio Project File - Name="Lennard" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Lennard - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lennard.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lennard.mak" CFG="Lennard - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lennard - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Lennard - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lennard - Win32 Release"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G3 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fr /YX /FD /c
# ADD CPP /nologo /G3 /MT /W3 /GX /O2 /Ob2 /I "..\cryptlib\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fr /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /map /machine:IX86
# ADD LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /profile /map /machine:IX86

!ELSEIF  "$(CFG)" == "Lennard - Win32 Debug"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /Zi /Od /I "..\cryptlib\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /debug /machine:IX86 /pdbtype:sept
# ADD LINK32 oldnames.lib /nologo /stack:0x2800 /subsystem:windows /profile /debug /machine:IX86

!ENDIF 

# Begin Target

# Name "Lennard - Win32 Release"
# Name "Lennard - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat"
# Begin Source File

SOURCE=.\BASEINT.CPP
# End Source File
# Begin Source File

SOURCE=.\CMDVW.CPP
# End Source File
# Begin Source File

SOURCE=.\CONSFRM.CPP
# End Source File
# Begin Source File

SOURCE=.\CONSVW.CPP
# End Source File
# Begin Source File

SOURCE=.\DALOFTDG.CPP
# End Source File
# Begin Source File

SOURCE=.\DELTYPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\DRWGPHDG.CPP
# End Source File
# Begin Source File

SOURCE=.\DRWMOLDG.CPP
# End Source File
# Begin Source File

SOURCE=.\EDINTSDG.CPP
# End Source File
# Begin Source File

SOURCE=.\EDITLJDG.CPP
# End Source File
# Begin Source File

SOURCE=.\EDTXTDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\FIRSTDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\GRAPH.CPP
# End Source File
# Begin Source File

SOURCE=.\GRIDDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\GRPRPSDG.CPP
# End Source File
# Begin Source File

SOURCE=.\LENDOC.CPP
# End Source File
# Begin Source File

SOURCE=.\LENNARD.CPP
# End Source File
# Begin Source File

SOURCE=.\LENNARD.DEF
# End Source File
# Begin Source File

SOURCE=.\HLP\lennard.hpj

!IF  "$(CFG)" == "Lennard - Win32 Release"

# Begin Custom Build - Performing Custom Build Step on $(InputPath): help file creation
OutDir=.\Release
ProjDir=.
TargetName=Lennard
InputPath=.\HLP\lennard.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\makehelp.bat

# End Custom Build

!ELSEIF  "$(CFG)" == "Lennard - Win32 Debug"

# Begin Custom Build - Performing Custom Build Step on $(InputPath): help file creation
OutDir=.\Debug
ProjDir=.
TargetName=Lennard
InputPath=.\HLP\lennard.hpj

"$(OutDir)\$(TargetName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(ProjDir)\makehelp.bat

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LENNARD.RC
# End Source File
# Begin Source File

SOURCE=.\MAINFRM.CPP
# End Source File
# Begin Source File

SOURCE=.\MMHRDINT.CPP
# End Source File
# Begin Source File

SOURCE=.\Mmljint.cpp
# End Source File
# Begin Source File

SOURCE=.\MNCSETDG.CPP
# End Source File
# Begin Source File

SOURCE=.\MNMLDINT.CPP
# End Source File
# Begin Source File

SOURCE=.\MOL.CPP
# End Source File
# Begin Source File

SOURCE=.\MolMolReactiveInt.cpp
# End Source File
# Begin Source File

SOURCE=.\MOLPRPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\MONITOR.CPP
# End Source File
# Begin Source File

SOURCE=.\MONPRPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\MPHRDINT.CPP
# End Source File
# Begin Source File

SOURCE=.\NDWALLDG.CPP
# End Source File
# Begin Source File

SOURCE=.\NEWSVDG.CPP
# End Source File
# Begin Source File

SOURCE=.\NULLINT.CPP
# End Source File
# Begin Source File

SOURCE=.\PISPRPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\PISTON.CPP
# End Source File
# Begin Source File

SOURCE=.\PSTRSTDG.CPP
# End Source File
# Begin Source File

SOURCE=.\ReactiveIntPropsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ResolveMultipleSelnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SCPDLYDG.CPP
# End Source File
# Begin Source File

SOURCE=.\SELNUMDG.CPP
# End Source File
# Begin Source File

SOURCE=.\SIM.CPP

!IF  "$(CFG)" == "Lennard - Win32 Release"

# ADD CPP /Ox /Ot /Ow /Og /Oi /Op- /Oy-
# SUBTRACT CPP /Oa /Os

!ELSEIF  "$(CFG)" == "Lennard - Win32 Debug"

# ADD CPP /Od

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SIMFRM.CPP
# End Source File
# Begin Source File

SOURCE=.\SIMOBJS.CPP
# End Source File
# Begin Source File

SOURCE=.\SimSizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SIMVW.CPP
# End Source File
# Begin Source File

SOURCE=.\SLIDER.CPP
# End Source File
# Begin Source File

SOURCE=.\SLTYPRDG.CPP
# End Source File
# Begin Source File

SOURCE=.\SMOLPRDG.CPP
# End Source File
# Begin Source File

SOURCE=.\SPISPRDG.CPP
# End Source File
# Begin Source File

SOURCE=.\SSLPRPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\STDAFX.CPP
# End Source File
# Begin Source File

SOURCE=.\SWALLPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\SYSPRPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\TEXTBOX.CPP
# End Source File
# Begin Source File

SOURCE=.\TRACKER.CPP
# End Source File
# Begin Source File

SOURCE=.\TXTPRPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\WALL.CPP
# End Source File
# Begin Source File

SOURCE=.\WALPRPDG.CPP
# End Source File
# Begin Source File

SOURCE=.\WMDEFINT.CPP
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\BASEINT.H
# End Source File
# Begin Source File

SOURCE=.\CMDVW.H
# End Source File
# Begin Source File

SOURCE=.\CONSFRM.H
# End Source File
# Begin Source File

SOURCE=.\CONSVW.H
# End Source File
# Begin Source File

SOURCE=.\DALOFTDG.H
# End Source File
# Begin Source File

SOURCE=.\DELTYPDG.H
# End Source File
# Begin Source File

SOURCE=.\DRWGPHDG.H
# End Source File
# Begin Source File

SOURCE=.\DRWMOLDG.H
# End Source File
# Begin Source File

SOURCE=.\EDINTSDG.H
# End Source File
# Begin Source File

SOURCE=.\EDITLJDG.H
# End Source File
# Begin Source File

SOURCE=.\EDTXTDLG.H
# End Source File
# Begin Source File

SOURCE=.\FIRSTDLG.H
# End Source File
# Begin Source File

SOURCE=.\GRAPHX.H
# End Source File
# Begin Source File

SOURCE=.\GRIDDLG.H
# End Source File
# Begin Source File

SOURCE=.\GRPRPSDG.H
# End Source File
# Begin Source File

SOURCE=.\LENDOC.H
# End Source File
# Begin Source File

SOURCE=.\LENNARD.H
# End Source File
# Begin Source File

SOURCE=.\MAINFRM.H
# End Source File
# Begin Source File

SOURCE=.\MMHRDINT.H
# End Source File
# Begin Source File

SOURCE=.\MMLJINT.H
# End Source File
# Begin Source File

SOURCE=.\MNCSETDG.H
# End Source File
# Begin Source File

SOURCE=.\MNMLDINT.H
# End Source File
# Begin Source File

SOURCE=.\MOL.H
# End Source File
# Begin Source File

SOURCE=.\MolMolReactiveInt.h
# End Source File
# Begin Source File

SOURCE=.\MOLPRPDG.H
# End Source File
# Begin Source File

SOURCE=.\MONITOR.H
# End Source File
# Begin Source File

SOURCE=.\MONPRPDG.H
# End Source File
# Begin Source File

SOURCE=.\MPHRDINT.H
# End Source File
# Begin Source File

SOURCE=.\NDWALLDG.H
# End Source File
# Begin Source File

SOURCE=.\NEWSVDG.H
# End Source File
# Begin Source File

SOURCE=.\NULLINT.H
# End Source File
# Begin Source File

SOURCE=.\PISPRPDG.H
# End Source File
# Begin Source File

SOURCE=.\PISTON.H
# End Source File
# Begin Source File

SOURCE=.\PSTRSTDG.H
# End Source File
# Begin Source File

SOURCE=.\ReactiveIntPropsDlg.h
# End Source File
# Begin Source File

SOURCE=.\ResolveMultipleSelnDlg.h
# End Source File
# Begin Source File

SOURCE=.\RESOURCE.H
# End Source File
# Begin Source File

SOURCE=.\SCPDLYDG.H
# End Source File
# Begin Source File

SOURCE=.\SELNUMDG.H
# End Source File
# Begin Source File

SOURCE=.\SIM.H
# End Source File
# Begin Source File

SOURCE=.\SIMFRM.H
# End Source File
# Begin Source File

SOURCE=.\SIMOBJS.H
# End Source File
# Begin Source File

SOURCE=.\SimSizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\SIMVW.H
# End Source File
# Begin Source File

SOURCE=.\SLIDER.H
# End Source File
# Begin Source File

SOURCE=.\SLTYPRDG.H
# End Source File
# Begin Source File

SOURCE=.\SMOLPRDG.H
# End Source File
# Begin Source File

SOURCE=.\SPISPRDG.H
# End Source File
# Begin Source File

SOURCE=.\SSLPRPDG.H
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# Begin Source File

SOURCE=.\SWALLPDG.H
# End Source File
# Begin Source File

SOURCE=.\SYSPRPDG.H
# End Source File
# Begin Source File

SOURCE=.\TEXTBOX.H
# End Source File
# Begin Source File

SOURCE=.\TRACKER.H
# End Source File
# Begin Source File

SOURCE=.\TXTPRPDG.H
# End Source File
# Begin Source File

SOURCE=.\WALL.H
# End Source File
# Begin Source File

SOURCE=.\WALPRPDG.H
# End Source File
# Begin Source File

SOURCE=.\WMDEFINT.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\CLR_GRAP.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\FILE_SAV.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\HAND.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\LENDOC.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\LENNARD.ICO
# End Source File
# Begin Source File

SOURCE=.\res\lennard.rc2
# End Source File
# Begin Source File

SOURCE=.\RES\SLIDE_LR.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\TOOLBAR.BMP
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "*.rtf *.bmp *.hpj"
# Begin Source File

SOURCE=.\HLP\afxcore.rtf
# End Source File
# Begin Source File

SOURCE=.\HLP\AFXPRINT.RTF
# End Source File
# Begin Source File

SOURCE=.\HLP\APPEXIT.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\BULLET.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\CURARW2.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\CURARW4.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\CURHELP.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\EDITCOPY.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\EDITCUT.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\EDITPAST.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\EDITUNDO.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\FILENEW.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\FILEOPEN.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\FILEPRNT.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\FILESAVE.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\HLPABOUT.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\HLPCNTXT.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\HLPSBAR.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\HLPTBAR.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\keywords.txt
# End Source File
# Begin Source File

SOURCE=.\HLP\lennard.cnt
# End Source File
# Begin Source File

SOURCE=.\HLP\ljeqn.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\ljgraph.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\SCMAX.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\SCMENU.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\SCMIN.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\scpffwd.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\scppause.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\scpplay.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\scprecrd.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\scpstop.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\SIMRPLAY.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\SIMRUN.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\SIMSTOP.BMP
# End Source File
# Begin Source File

SOURCE=.\HLP\stopboth.bmp
# End Source File
# Begin Source File

SOURCE=.\HLP\strtboth.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Joblist.txt
# End Source File
# Begin Source File

SOURCE="c:\Program Files\Microsoft Visual Studio\Vc98\Lib\Winmm.lib"
# End Source File
# End Target
# End Project
