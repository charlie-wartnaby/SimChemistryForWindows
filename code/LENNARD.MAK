# Microsoft Visual C++ generated build script - Do not modify

PROJ = LENNARD
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\CHARLIE\LENNARD\CODE\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = RAN1.C      
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /FPi87 /W3 /Zi /ALw /Gt128 /Gx- /YX /Od /OV9 /D "_DEBUG" /GA /Fd"LENNARD.PDB"       /Fp"LENNARD.PCH"
CFLAGS_R_WEXE = /nologo /f- /Gs /G3 /FPi87 /W3 /ALw /Gt128 /Gx- /YX /O2 /Ob2 /OV8 /D "NDEBUG" /Fc /Fr /GA /Fp"LENNARD.PCH"
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /SEG:256 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /STACK:10240 /SEG:256 /ALIGN:16 /ONERROR:NOEXE /CO /MAP /LINE  
LIBS_D_WEXE = lafxcwd oldnames libw llibcew commdlg.lib shell.lib 
LIBS_R_WEXE = lafxcw oldnames libw llibcew commdlg.lib shell.lib 
RCFLAGS = /nologo /z 
RESFLAGS = /nologo /t /k
RUNFLAGS = 
DEFFILE = LENNARD.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = STDAFX.SBR \
		LENNARD.SBR \
		MAINFRM.SBR \
		LENDOC.SBR \
		CONSVW.SBR \
		CONSFRM.SBR \
		CMDVW.SBR \
		SIMOBJS.SBR \
		SIMVW.SBR \
		DRWMOLDG.SBR \
		RAN1.SBR \
		SIMFRM.SBR \
		MOLPRPDG.SBR \
		WALPRPDG.SBR \
		EDINTSDG.SBR \
		EDITLJDG.SBR \
		PISPRPDG.SBR \
		NDWALLDG.SBR \
		PSTRSTDG.SBR \
		BASEINT.SBR \
		MMHRDINT.SBR \
		MMLJINT.SBR \
		MOL.SBR \
		MPHRDINT.SBR \
		NULLINT.SBR \
		PISTON.SBR \
		SIM.SBR \
		WALL.SBR \
		WMDEFINT.SBR \
		DELTYPDG.SBR \
		TEXTBOX.SBR \
		TXTPRPDG.SBR \
		EDTXTDLG.SBR \
		SELNUMDG.SBR \
		MONITOR.SBR \
		MONPRPDG.SBR \
		MNCSETDG.SBR \
		MNMLDINT.SBR \
		SYSPRPDG.SBR \
		SCPDLYDG.SBR \
		FIRSTDLG.SBR \
		DALOFTDG.SBR \
		GRAPH.SBR \
		GRPRPSDG.SBR \
		DRWGPHDG.SBR \
		SMOLPRDG.SBR \
		TRACKER.SBR \
		GRIDDLG.SBR \
		SWALLPDG.SBR \
		SPISPRDG.SBR \
		SLIDER.SBR \
		SSLPRPDG.SBR \
		SLTYPRDG.SBR \
		NEWSVDG.SBR \
		REGDLG.SBR \
		BIGMATH.SBR \
		BIGNUM.SBR \
		COREMULT.SBR \
		FASTMULT.SBR \
		LONGMULT.SBR \
		MD2.SBR \
		MD4.SBR \
		MD5.SBR \
		MDIGEST.SBR \
		MEMALLOC.SBR \
		MSBTABLE.SBR \
		SHS.SBR \
		PUBKEY.SBR \
		BUFNET.SBR \
		BUFASN1.SBR \
		DSA.SBR \
		BIGPOW.SBR \
		EUCLID.SBR \
		QUANTIZE.SBR \
		GETRAND.SBR \
		DESMODES.SBR \
		DES.SBR \
		DESRAND.SBR


LENNARD_RCDEP = c:\charlie\lennard\code\res\lennard.ico \
	c:\charlie\lennard\code\res\lendoc.ico \
	c:\charlie\lennard\code\res\clr_grap.bmp \
	c:\charlie\lennard\code\res\file_sav.bmp \
	c:\charlie\lennard\code\res\hand.cur \
	c:\charlie\lennard\code\res\slide_lr.cur \
	c:\charlie\lennard\code\res\lennard.rc2


STDAFX_DEP = c:\charlie\lennard\code\stdafx.h


LENNARD_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\mainfrm.h \
	c:\charlie\lennard\code\consfrm.h \
	c:\charlie\lennard\code\simfrm.h \
	c:\charlie\lennard\code\consvw.h \
	c:\charlie\lennard\code\simvw.h \
	c:\charlie\lennard\code\firstdlg.h \
	c:\charlie\lennard\code\regdlg.h \
	c:\charlie\lennard\cryptlib\src\libcrypt.h


MAINFRM_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\mainfrm.h


LENDOC_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\piston.h \
	c:\charlie\lennard\code\textbox.h \
	c:\charlie\lennard\code\monitor.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\molprpdg.h \
	c:\charlie\lennard\code\walprpdg.h \
	c:\charlie\lennard\code\edintsdg.h \
	c:\charlie\lennard\code\pisprpdg.h \
	c:\charlie\lennard\code\ndwalldg.h \
	c:\charlie\lennard\code\txtprpdg.h \
	c:\charlie\lennard\code\deltypdg.h \
	c:\charlie\lennard\code\monprpdg.h \
	c:\charlie\lennard\code\sysprpdg.h \
	c:\charlie\lennard\code\scpdlydg.h \
	c:\charlie\lennard\code\daloftdg.h \
	c:\charlie\lennard\code\grprpsdg.h \
	c:\charlie\lennard\code\sltyprdg.h


CONSVW_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\consvw.h


CONSFRM_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\consfrm.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\consvw.h \
	c:\charlie\lennard\code\cmdvw.h


CMDVW_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\cmdvw.h


SIMOBJS_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\simvw.h \
	c:\charlie\lennard\code\ran1.h


SIMVW_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\simvw.h


DRWMOLDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\drwmoldg.h


RAN1_DEP = c:\charlie\lennard\code\ran1.h


SIMFRM_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simfrm.h


MOLPRPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\molprpdg.h


WALPRPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\walprpdg.h


EDINTSDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\edintsdg.h


EDITLJDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\editljdg.h \
	c:\charlie\lennard\code\simobjs.h


PISPRPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\piston.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\pisprpdg.h


NDWALLDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\ndwalldg.h


PSTRSTDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\pstrstdg.h


BASEINT_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\ran1.h


MMHRDINT_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\mmhrdint.h \
	c:\charlie\lennard\code\lennard.h


MMLJINT_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\mmljint.h \
	c:\charlie\lennard\code\editljdg.h


MOL_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\drwmoldg.h \
	c:\charlie\lennard\code\molprpdg.h \
	c:\charlie\lennard\code\smolprdg.h


MPHRDINT_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\piston.h \
	c:\charlie\lennard\code\mphrdint.h


NULLINT_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\nullint.h


PISTON_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\simvw.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\piston.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\pstrstdg.h \
	c:\charlie\lennard\code\pisprpdg.h \
	c:\charlie\lennard\code\spisprdg.h


SIM_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\piston.h \
	c:\charlie\lennard\code\textbox.h \
	c:\charlie\lennard\code\monitor.h \
	c:\charlie\lennard\code\graphx.h \
	c:\charlie\lennard\code\slider.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\nullint.h \
	c:\charlie\lennard\code\mmhrdint.h \
	c:\charlie\lennard\code\mmljint.h \
	c:\charlie\lennard\code\wmdefint.h \
	c:\charlie\lennard\code\mphrdint.h \
	c:\charlie\lennard\code\mnmldint.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\griddlg.h \
	c:\charlie\lennard\code\ran1.h


WALL_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\piston.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\walprpdg.h \
	c:\charlie\lennard\code\swallpdg.h


WMDEFINT_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\wmdefint.h


DELTYPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\deltypdg.h


TEXTBOX_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\textbox.h \
	c:\charlie\lennard\code\edtxtdlg.h \
	c:\charlie\lennard\code\txtprpdg.h


TXTPRPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\txtprpdg.h


EDTXTDLG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\textbox.h \
	c:\charlie\lennard\code\edtxtdlg.h \
	c:\charlie\lennard\code\selnumdg.h


SELNUMDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\selnumdg.h


MONITOR_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\mnmldint.h \
	c:\charlie\lennard\code\monitor.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\mncsetdg.h \
	c:\charlie\lennard\code\monprpdg.h \
	c:\charlie\lennard\code\ran1.h


MONPRPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\monprpdg.h


MNCSETDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\monitor.h \
	c:\charlie\lennard\code\mncsetdg.h


MNMLDINT_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\baseint.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\monitor.h \
	c:\charlie\lennard\code\mnmldint.h


SYSPRPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sysprpdg.h


SCPDLYDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\scpdlydg.h


FIRSTDLG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\firstdlg.h


DALOFTDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\daloftdg.h


GRAPH_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\graphx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\lendoc.h \
	c:\charlie\lennard\code\drwgphdg.h


GRPRPSDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\grprpsdg.h


DRWGPHDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\drwgphdg.h \
	c:\charlie\lennard\code\selnumdg.h


SMOLPRDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\mol.h \
	c:\charlie\lennard\code\smolprdg.h


TRACKER_DEP = c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\simvw.h


GRIDDLG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\griddlg.h


SWALLPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\swallpdg.h


SPISPRDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\wall.h \
	c:\charlie\lennard\code\piston.h \
	c:\charlie\lennard\code\spisprdg.h


SLIDER_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\tracker.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\slider.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\sslprpdg.h \
	c:\charlie\lennard\code\newsvdg.h \
	c:\charlie\lennard\code\sltyprdg.h


SSLPRPDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\simobjs.h \
	c:\charlie\lennard\code\sim.h \
	c:\charlie\lennard\code\sslprpdg.h \
	c:\charlie\lennard\code\selnumdg.h


SLTYPRDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\sltyprdg.h


NEWSVDG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\newsvdg.h


REGDLG_DEP = c:\charlie\lennard\code\stdafx.h \
	c:\charlie\lennard\code\lennard.h \
	c:\charlie\lennard\code\regdlg.h


BIGMATH_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


BIGNUM_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


COREMULT_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


FASTMULT_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


LONGMULT_DEP = c:\charlie\lennard\cryptlib\src\longlong.h


MD2_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


MD4_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


MD5_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


MDIGEST_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


MEMALLOC_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


MSBTABLE_DEP = 

SHS_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


PUBKEY_DEP = 

BUFNET_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


BUFASN1_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


DSA_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


BIGPOW_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


EUCLID_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


QUANTIZE_DEP = 

GETRAND_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


DESMODES_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


DES_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


DESRAND_DEP = c:\charlie\lennard\cryptlib\src\libcrypt.h


all:	$(PROJ).EXE $(PROJ).BSC

LENNARD.RES:	LENNARD.RC $(LENNARD_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r LENNARD.RC

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

LENNARD.OBJ:	LENNARD.CPP $(LENNARD_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c LENNARD.CPP

MAINFRM.OBJ:	MAINFRM.CPP $(MAINFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MAINFRM.CPP

LENDOC.OBJ:	LENDOC.CPP $(LENDOC_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c LENDOC.CPP

CONSVW.OBJ:	CONSVW.CPP $(CONSVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CONSVW.CPP

CONSFRM.OBJ:	CONSFRM.CPP $(CONSFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CONSFRM.CPP

CMDVW.OBJ:	CMDVW.CPP $(CMDVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CMDVW.CPP

SIMOBJS.OBJ:	SIMOBJS.CPP $(SIMOBJS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SIMOBJS.CPP

SIMVW.OBJ:	SIMVW.CPP $(SIMVW_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SIMVW.CPP

DRWMOLDG.OBJ:	DRWMOLDG.CPP $(DRWMOLDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DRWMOLDG.CPP

RAN1.OBJ:	RAN1.C $(RAN1_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c RAN1.C

SIMFRM.OBJ:	SIMFRM.CPP $(SIMFRM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SIMFRM.CPP

MOLPRPDG.OBJ:	MOLPRPDG.CPP $(MOLPRPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MOLPRPDG.CPP

WALPRPDG.OBJ:	WALPRPDG.CPP $(WALPRPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WALPRPDG.CPP

EDINTSDG.OBJ:	EDINTSDG.CPP $(EDINTSDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c EDINTSDG.CPP

EDITLJDG.OBJ:	EDITLJDG.CPP $(EDITLJDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c EDITLJDG.CPP

PISPRPDG.OBJ:	PISPRPDG.CPP $(PISPRPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PISPRPDG.CPP

NDWALLDG.OBJ:	NDWALLDG.CPP $(NDWALLDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c NDWALLDG.CPP

PSTRSTDG.OBJ:	PSTRSTDG.CPP $(PSTRSTDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PSTRSTDG.CPP

BASEINT.OBJ:	BASEINT.CPP $(BASEINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c BASEINT.CPP

MMHRDINT.OBJ:	MMHRDINT.CPP $(MMHRDINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MMHRDINT.CPP

MMLJINT.OBJ:	MMLJINT.CPP $(MMLJINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MMLJINT.CPP

MOL.OBJ:	MOL.CPP $(MOL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MOL.CPP

MPHRDINT.OBJ:	MPHRDINT.CPP $(MPHRDINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MPHRDINT.CPP

NULLINT.OBJ:	NULLINT.CPP $(NULLINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c NULLINT.CPP

PISTON.OBJ:	PISTON.CPP $(PISTON_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PISTON.CPP

SIM.OBJ:	SIM.CPP $(SIM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SIM.CPP

WALL.OBJ:	WALL.CPP $(WALL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WALL.CPP

WMDEFINT.OBJ:	WMDEFINT.CPP $(WMDEFINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WMDEFINT.CPP

DELTYPDG.OBJ:	DELTYPDG.CPP $(DELTYPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DELTYPDG.CPP

TEXTBOX.OBJ:	TEXTBOX.CPP $(TEXTBOX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TEXTBOX.CPP

TXTPRPDG.OBJ:	TXTPRPDG.CPP $(TXTPRPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TXTPRPDG.CPP

EDTXTDLG.OBJ:	EDTXTDLG.CPP $(EDTXTDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c EDTXTDLG.CPP

SELNUMDG.OBJ:	SELNUMDG.CPP $(SELNUMDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SELNUMDG.CPP

MONITOR.OBJ:	MONITOR.CPP $(MONITOR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MONITOR.CPP

MONPRPDG.OBJ:	MONPRPDG.CPP $(MONPRPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MONPRPDG.CPP

MNCSETDG.OBJ:	MNCSETDG.CPP $(MNCSETDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MNCSETDG.CPP

MNMLDINT.OBJ:	MNMLDINT.CPP $(MNMLDINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MNMLDINT.CPP

SYSPRPDG.OBJ:	SYSPRPDG.CPP $(SYSPRPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SYSPRPDG.CPP

SCPDLYDG.OBJ:	SCPDLYDG.CPP $(SCPDLYDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SCPDLYDG.CPP

FIRSTDLG.OBJ:	FIRSTDLG.CPP $(FIRSTDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c FIRSTDLG.CPP

DALOFTDG.OBJ:	DALOFTDG.CPP $(DALOFTDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DALOFTDG.CPP

GRAPH.OBJ:	GRAPH.CPP $(GRAPH_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c GRAPH.CPP

GRPRPSDG.OBJ:	GRPRPSDG.CPP $(GRPRPSDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c GRPRPSDG.CPP

DRWGPHDG.OBJ:	DRWGPHDG.CPP $(DRWGPHDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DRWGPHDG.CPP

SMOLPRDG.OBJ:	SMOLPRDG.CPP $(SMOLPRDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SMOLPRDG.CPP

TRACKER.OBJ:	TRACKER.CPP $(TRACKER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c TRACKER.CPP

GRIDDLG.OBJ:	GRIDDLG.CPP $(GRIDDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c GRIDDLG.CPP

SWALLPDG.OBJ:	SWALLPDG.CPP $(SWALLPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SWALLPDG.CPP

SPISPRDG.OBJ:	SPISPRDG.CPP $(SPISPRDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SPISPRDG.CPP

SLIDER.OBJ:	SLIDER.CPP $(SLIDER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SLIDER.CPP

SSLPRPDG.OBJ:	SSLPRPDG.CPP $(SSLPRPDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SSLPRPDG.CPP

SLTYPRDG.OBJ:	SLTYPRDG.CPP $(SLTYPRDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SLTYPRDG.CPP

NEWSVDG.OBJ:	NEWSVDG.CPP $(NEWSVDG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c NEWSVDG.CPP

REGDLG.OBJ:	REGDLG.CPP $(REGDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REGDLG.CPP

BIGMATH.OBJ:	..\CRYPTLIB\SRC\BIGMATH.C $(BIGMATH_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\BIGMATH.C

BIGNUM.OBJ:	..\CRYPTLIB\SRC\BIGNUM.C $(BIGNUM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\BIGNUM.C

COREMULT.OBJ:	..\CRYPTLIB\SRC\COREMULT.C $(COREMULT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\COREMULT.C

FASTMULT.OBJ:	..\CRYPTLIB\SRC\FASTMULT.C $(FASTMULT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\FASTMULT.C

LONGMULT.OBJ:	..\CRYPTLIB\SRC\LONGMULT.C $(LONGMULT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\LONGMULT.C

MD2.OBJ:	..\CRYPTLIB\SRC\MD2.C $(MD2_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\MD2.C

MD4.OBJ:	..\CRYPTLIB\SRC\MD4.C $(MD4_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\MD4.C

MD5.OBJ:	..\CRYPTLIB\SRC\MD5.C $(MD5_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\MD5.C

MDIGEST.OBJ:	..\CRYPTLIB\SRC\MDIGEST.C $(MDIGEST_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\MDIGEST.C

MEMALLOC.OBJ:	..\CRYPTLIB\SRC\MEMALLOC.C $(MEMALLOC_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\MEMALLOC.C

MSBTABLE.OBJ:	..\CRYPTLIB\SRC\MSBTABLE.C $(MSBTABLE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\MSBTABLE.C

SHS.OBJ:	..\CRYPTLIB\SRC\SHS.C $(SHS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\SHS.C

PUBKEY.OBJ:	..\CRYPTGEN\PUBKEY.C $(PUBKEY_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTGEN\PUBKEY.C

BUFNET.OBJ:	..\CRYPTLIB\SRC\BUFNET.C $(BUFNET_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\BUFNET.C

BUFASN1.OBJ:	..\CRYPTLIB\SRC\BUFASN1.C $(BUFASN1_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\BUFASN1.C

DSA.OBJ:	..\CRYPTLIB\SRC\DSA.C $(DSA_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\DSA.C

BIGPOW.OBJ:	..\CRYPTLIB\SRC\BIGPOW.C $(BIGPOW_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\BIGPOW.C

EUCLID.OBJ:	..\CRYPTLIB\SRC\EUCLID.C $(EUCLID_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\EUCLID.C

QUANTIZE.OBJ:	..\CRYPTLIB\SRC\QUANTIZE.C $(QUANTIZE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\QUANTIZE.C

GETRAND.OBJ:	..\CRYPTLIB\SRC\GETRAND.C $(GETRAND_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\GETRAND.C

DESMODES.OBJ:	..\CRYPTLIB\SRC\DESMODES.C $(DESMODES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\DESMODES.C

DES.OBJ:	..\CRYPTLIB\SRC\DES.C $(DES_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\DES.C

DESRAND.OBJ:	..\CRYPTLIB\SRC\DESRAND.C $(DESRAND_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\CRYPTLIB\SRC\DESRAND.C


$(PROJ).EXE::	LENNARD.RES

$(PROJ).EXE::	STDAFX.OBJ LENNARD.OBJ MAINFRM.OBJ LENDOC.OBJ CONSVW.OBJ CONSFRM.OBJ \
	CMDVW.OBJ SIMOBJS.OBJ SIMVW.OBJ DRWMOLDG.OBJ RAN1.OBJ SIMFRM.OBJ MOLPRPDG.OBJ WALPRPDG.OBJ \
	EDINTSDG.OBJ EDITLJDG.OBJ PISPRPDG.OBJ NDWALLDG.OBJ PSTRSTDG.OBJ BASEINT.OBJ MMHRDINT.OBJ \
	MMLJINT.OBJ MOL.OBJ MPHRDINT.OBJ NULLINT.OBJ PISTON.OBJ SIM.OBJ WALL.OBJ WMDEFINT.OBJ \
	DELTYPDG.OBJ TEXTBOX.OBJ TXTPRPDG.OBJ EDTXTDLG.OBJ SELNUMDG.OBJ MONITOR.OBJ MONPRPDG.OBJ \
	MNCSETDG.OBJ MNMLDINT.OBJ SYSPRPDG.OBJ SCPDLYDG.OBJ FIRSTDLG.OBJ DALOFTDG.OBJ GRAPH.OBJ \
	GRPRPSDG.OBJ DRWGPHDG.OBJ SMOLPRDG.OBJ TRACKER.OBJ GRIDDLG.OBJ SWALLPDG.OBJ SPISPRDG.OBJ \
	SLIDER.OBJ SSLPRPDG.OBJ SLTYPRDG.OBJ NEWSVDG.OBJ REGDLG.OBJ BIGMATH.OBJ BIGNUM.OBJ \
	COREMULT.OBJ FASTMULT.OBJ LONGMULT.OBJ MD2.OBJ MD4.OBJ MD5.OBJ MDIGEST.OBJ MEMALLOC.OBJ \
	MSBTABLE.OBJ SHS.OBJ PUBKEY.OBJ BUFNET.OBJ BUFASN1.OBJ DSA.OBJ BIGPOW.OBJ EUCLID.OBJ \
	QUANTIZE.OBJ GETRAND.OBJ DESMODES.OBJ DES.OBJ DESRAND.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
LENNARD.OBJ +
MAINFRM.OBJ +
LENDOC.OBJ +
CONSVW.OBJ +
CONSFRM.OBJ +
CMDVW.OBJ +
SIMOBJS.OBJ +
SIMVW.OBJ +
DRWMOLDG.OBJ +
RAN1.OBJ +
SIMFRM.OBJ +
MOLPRPDG.OBJ +
WALPRPDG.OBJ +
EDINTSDG.OBJ +
EDITLJDG.OBJ +
PISPRPDG.OBJ +
NDWALLDG.OBJ +
PSTRSTDG.OBJ +
BASEINT.OBJ +
MMHRDINT.OBJ +
MMLJINT.OBJ +
MOL.OBJ +
MPHRDINT.OBJ +
NULLINT.OBJ +
PISTON.OBJ +
SIM.OBJ +
WALL.OBJ +
WMDEFINT.OBJ +
DELTYPDG.OBJ +
TEXTBOX.OBJ +
TXTPRPDG.OBJ +
EDTXTDLG.OBJ +
SELNUMDG.OBJ +
MONITOR.OBJ +
MONPRPDG.OBJ +
MNCSETDG.OBJ +
MNMLDINT.OBJ +
SYSPRPDG.OBJ +
SCPDLYDG.OBJ +
FIRSTDLG.OBJ +
DALOFTDG.OBJ +
GRAPH.OBJ +
GRPRPSDG.OBJ +
DRWGPHDG.OBJ +
SMOLPRDG.OBJ +
TRACKER.OBJ +
GRIDDLG.OBJ +
SWALLPDG.OBJ +
SPISPRDG.OBJ +
SLIDER.OBJ +
SSLPRPDG.OBJ +
SLTYPRDG.OBJ +
NEWSVDG.OBJ +
REGDLG.OBJ +
BIGMATH.OBJ +
BIGNUM.OBJ +
COREMULT.OBJ +
FASTMULT.OBJ +
LONGMULT.OBJ +
MD2.OBJ +
MD4.OBJ +
MD5.OBJ +
MDIGEST.OBJ +
MEMALLOC.OBJ +
MSBTABLE.OBJ +
SHS.OBJ +
PUBKEY.OBJ +
BUFNET.OBJ +
BUFASN1.OBJ +
DSA.OBJ +
BIGPOW.OBJ +
EUCLID.OBJ +
QUANTIZE.OBJ +
GETRAND.OBJ +
DESMODES.OBJ +
DES.OBJ +
DESRAND.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) LENNARD.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	LENNARD.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) LENNARD.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
