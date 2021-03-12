# Microsoft Developer Studio Project File - Name="ogmSasD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ogmSasD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ogmSasD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ogmSasD.mak" CFG="ogmSasD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ogmSasD - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ogmSasD - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ogmSasD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OGMSASD_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OGMSASD_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "ogmSasD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\debug"
# PROP Intermediate_Dir "..\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OGMSASD_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /I "..\datagrid\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_WINDLL" /D "_AFXDLL" /D "_GXDLL" /YX /FD /c
# SUBTRACT CPP /O<none>
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /pdb:none /debug /machine:I386 /def:".\ogmsasd.DEF" /libpath:"..\debug"

!ENDIF 

# Begin Target

# Name "ogmSasD - Win32 Release"
# Name "ogmSasD - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gxabstr.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxapp.cpp

!IF  "$(CFG)" == "ogmSasD - Win32 Release"

!ELSEIF  "$(CFG)" == "ogmSasD - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gxautscr.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcalcsi.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcbmbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\gxccbbox.cpp
# End Source File
# Begin Source File

SOURCE=.\gxccheck.cpp
# End Source File
# Begin Source File

SOURCE=.\gxccombo.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcedhot.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcedit.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcedspn.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcharro.cpp
# End Source File
# Begin Source File

SOURCE=.\gxchbmbt.cpp
# End Source File
# Begin Source File

SOURCE=.\gxchbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\gxchcbbt.cpp
# End Source File
# Begin Source File

SOURCE=.\gxchead.cpp
# End Source File
# Begin Source File

SOURCE=.\gxchhsbt.cpp
# End Source File
# Begin Source File

SOURCE=.\gxchild.cpp
# End Source File
# Begin Source File

SOURCE=.\gxclear.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxcoll.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorabs.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorbuf.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorclp.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorcur.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcordim.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcordra.CPP
# End Source File
# Begin Source File

SOURCE=.\Gxcore.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcoret.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorfit.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorfr.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorfri.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcormsg.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorprn.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorscr.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorsel.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorsiz.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcorsty.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcpmov.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcpmovd.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcpush.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcradio.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcstat.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcvedit.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcvrcel.cpp
# End Source File
# Begin Source File

SOURCE=.\gxcvrdra.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxdata.cpp
# End Source File
# Begin Source File

SOURCE=.\gxdib.cpp
# End Source File
# Begin Source File

SOURCE=.\gxdll.cpp

!IF  "$(CFG)" == "ogmSasD - Win32 Release"

!ELSEIF  "$(CFG)" == "ogmSasD - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Gxdrawx.cpp
# End Source File
# Begin Source File

SOURCE=.\gxdrgwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\gxdxtext.cpp
# End Source File
# Begin Source File

SOURCE=.\gxfactry.cpp
# End Source File
# Begin Source File

SOURCE=.\gxfreeze.cpp
# End Source File
# Begin Source File

SOURCE=.\gxgmouse.cpp
# End Source File
# Begin Source File

SOURCE=.\gxgmousw.cpp
# End Source File
# Begin Source File

SOURCE=.\gxgscrlt.cpp
# End Source File
# Begin Source File

SOURCE=.\gxhide.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxhint.cpp
# End Source File
# Begin Source File

SOURCE=.\gxhinti.cpp
# End Source File
# Begin Source File

SOURCE=.\gximouse.cpp
# End Source File
# Begin Source File

SOURCE=.\gximousw.cpp
# End Source File
# Begin Source File

SOURCE=.\gxinl.cpp
# End Source File
# Begin Source File

SOURCE=.\gxmarked.cpp
# End Source File
# Begin Source File

SOURCE=.\gxmoverc.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxparam.cpp
# End Source File
# Begin Source File

SOURCE=.\gxplugin.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxprndev.cpp
# End Source File
# Begin Source File

SOURCE=.\gxprof.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxprp.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxprpdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxrange.cpp
# End Source File
# Begin Source File

SOURCE=.\gxscrltp.cpp
# End Source File
# Begin Source File

SOURCE=.\gxseldrg.cpp
# End Source File
# Begin Source File

SOURCE=.\gxselgrx.cpp
# End Source File
# Begin Source File

SOURCE=.\gxselrgu.cpp
# End Source File
# Begin Source File

SOURCE=.\gxsizeu.cpp
# End Source File
# Begin Source File

SOURCE=.\GXSplit.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxstydlg.cpp
# End Source File
# Begin Source File

SOURCE=.\gxstygd2.cpp
# End Source File
# Begin Source File

SOURCE=.\gxstygd3.cpp
# End Source File
# Begin Source File

SOURCE=.\gxstygdi.cpp
# End Source File
# Begin Source File

SOURCE=.\gxstyle2.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxstyles.cpp
# End Source File
# Begin Source File

SOURCE=.\gxstymap.cpp
# End Source File
# Begin Source File

SOURCE=.\gxstyua.cpp
# End Source File
# Begin Source File

SOURCE=.\gxstyuai.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxtbeam.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxtime.cpp
# End Source File
# Begin Source File

SOURCE=.\gxttn.cpp
# End Source File
# Begin Source File

SOURCE=.\gxtwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\gxupdopt.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxview.cpp
# End Source File
# Begin Source File

SOURCE=.\gxviewt.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxvw.cpp
# End Source File
# Begin Source File

SOURCE=.\Gxwait.cpp
# End Source File
# Begin Source File

SOURCE=.\gxwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ogmsasd.DEF

!IF  "$(CFG)" == "ogmSasD - Win32 Release"

!ELSEIF  "$(CFG)" == "ogmSasD - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gxresflt.h
# End Source File
# Begin Source File

SOURCE=.\gxresrc.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\RES\GX3D95.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GX3DCTRL.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXCHK31.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXCHK95.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXCHKNT.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXCOLUMN.CUR
# End Source File
# Begin Source File

SOURCE=.\GxDll.rc
# End Source File
# Begin Source File

SOURCE=.\RES\GXHEIGHT.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXIMA.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXIMALL.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXIMD.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXIMHORZ.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXIML.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXIMR.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXIMU.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXIMVERT.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXROW.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXSELDRG.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXSPLITH.CUR
# End Source File
# Begin Source File

SOURCE=.\RES\GXTTFONT.BMP
# End Source File
# Begin Source File

SOURCE=.\RES\GXWIDTH.CUR
# End Source File
# End Group
# Begin Group "Packages"

# PROP Default_Filter "vcx;vcp;vcw"
# Begin Source File

SOURCE=.\Uml\ogmSasD.vcp
# End Source File
# Begin Source File

SOURCE=..\ogmSasD.vcx
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
