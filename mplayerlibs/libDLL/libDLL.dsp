# Microsoft Developer Studio Project File - Name="libDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=libDLL - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libDLL.mak" CFG="libDLL - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libDLL - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "libDLL - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "libDLL - Xbox Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /GR /GX /O2 /Ob0 /D "WIN32" /D "_XBOX" /D "NDEBUG" /FD /G6 /Ztmp /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libDLL - Xbox Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /FD /G6 /Ztmp /c
# SUBTRACT CPP /YX
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\libDLLd.lib"

!ENDIF 

# Begin Target

# Name "libDLL - Xbox Release"
# Name "libDLL - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dshow\allocator.cpp
# End Source File
# Begin Source File

SOURCE=.\dmo\buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\cmediasample.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\com.cpp
# End Source File
# Begin Source File

SOURCE=.\dmo\dmo.cpp
# End Source File
# Begin Source File

SOURCE=.\dmo\DMO_AudioDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\dmo\dmo_guids.cpp
# End Source File
# Begin Source File

SOURCE=.\dmo\DMO_VideoDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\DS_AudioDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\DS_Filter.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\DS_VideoDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\guids.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\inputpin.cpp
# End Source File
# Begin Source File

SOURCE=.\loadlib.cpp
# End Source File
# Begin Source File

SOURCE=.\module.cpp
# End Source File
# Begin Source File

SOURCE=.\dshow\outputpin.cpp
# End Source File
# Begin Source File

SOURCE=.\registry.c
# End Source File
# Begin Source File

SOURCE=.\resource.cpp
# End Source File
# Begin Source File

SOURCE=.\wrapper.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
