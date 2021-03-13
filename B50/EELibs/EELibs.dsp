# Microsoft Developer Studio Project File - Name="EELibs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=EELibs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EELibs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EELibs.mak" CFG="EELibs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EELibs - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "EELibs - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EELibs - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "EELibs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /GX /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\Debug\EELibs.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\EELibs.lib"

!ENDIF 

# Begin Target

# Name "EELibs - Win32 Release"
# Name "EELibs - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Packages"

# PROP Default_Filter "vcx;vcp;vcw"
# Begin Source File

SOURCE=.\Uml\EELibs.vcp
# End Source File
# Begin Source File

SOURCE=..\EELibs.vcx
# End Source File
# End Group
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\noirp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndoabap.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndcorexp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndcorep.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndolep.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndresp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndwebedp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nddbgwp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nddbp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nddbscpp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndresedp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndscpedp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndscrptp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndscvrbp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndtkitp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndvgmp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nddbedtp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndwebp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nxdbp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nxexep.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nxgfxp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nxgwp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nxoldp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nxoop.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nxrunp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndmicp.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ikernel.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\nddmep.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\dmep.lib
# End Source File
# Begin Source File

SOURCE=D:\Ee21\Cpp\Lib\ndgwp.lib
# End Source File
# End Target
# End Project
