# Microsoft Developer Studio Project File - Name="libmpdvdkit2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=libmpdvdkit2 - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmpdvdkit2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmpdvdkit2.mak" CFG="libmpdvdkit2 - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmpdvdkit2 - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "libmpdvdkit2 - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "libmpdvdkit2 - Xbox Release"

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
# ADD CPP /nologo /W3 /GX /O2 /Ob0 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmpdvdkit2 - Xbox Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\libmpdvdkit2d.lib"

!ENDIF 

# Begin Target

# Name "libmpdvdkit2 - Xbox Release"
# Name "libmpdvdkit2 - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\css.c
# End Source File
# Begin Source File

SOURCE=.\decoder.c
# End Source File
# Begin Source File

SOURCE=.\device.c
# End Source File
# Begin Source File

SOURCE=.\dvd_input.c
# End Source File
# Begin Source File

SOURCE=.\dvd_reader.c
# End Source File
# Begin Source File

SOURCE=.\dvd_udf.c
# End Source File
# Begin Source File

SOURCE=.\dvdnav.c
# End Source File
# Begin Source File

SOURCE=.\error.c
# End Source File
# Begin Source File

SOURCE=.\highlight.c
# End Source File
# Begin Source File

SOURCE=.\ifo_print.c
# End Source File
# Begin Source File

SOURCE=.\ifo_read.c
# End Source File
# Begin Source File

SOURCE=.\ioctl.c
# End Source File
# Begin Source File

SOURCE=.\libdvdcss.c
# End Source File
# Begin Source File

SOURCE=.\md5.c
# End Source File
# Begin Source File

SOURCE=.\nav_print.c
# End Source File
# Begin Source File

SOURCE=.\nav_read.c
# End Source File
# Begin Source File

SOURCE=.\navigation.c
# End Source File
# Begin Source File

SOURCE=.\pthread.c
# End Source File
# Begin Source File

SOURCE=.\read_cache.c
# End Source File
# Begin Source File

SOURCE=.\searching.c
# End Source File
# Begin Source File

SOURCE=.\settings.c
# End Source File
# Begin Source File

SOURCE=.\vm.c
# End Source File
# Begin Source File

SOURCE=.\vmcmd.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bsdi_dvd.h
# End Source File
# Begin Source File

SOURCE=.\bswap.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=".\config-dvdnav.h"
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\css.h
# End Source File
# Begin Source File

SOURCE=.\csstables.h
# End Source File
# Begin Source File

SOURCE=.\decoder.h
# End Source File
# Begin Source File

SOURCE=.\device.h
# End Source File
# Begin Source File

SOURCE=.\dvd_functions.h
# End Source File
# Begin Source File

SOURCE=.\dvd_input.h
# End Source File
# Begin Source File

SOURCE=.\dvd_reader.h
# End Source File
# Begin Source File

SOURCE=.\dvd_types.h
# End Source File
# Begin Source File

SOURCE=.\dvd_udf.h
# End Source File
# Begin Source File

SOURCE=.\dvdcss.h
# End Source File
# Begin Source File

SOURCE=.\dvdnav.h
# End Source File
# Begin Source File

SOURCE=.\dvdnav_events.h
# End Source File
# Begin Source File

SOURCE=.\dvdnav_internal.h
# End Source File
# Begin Source File

SOURCE=.\ifo_print.h
# End Source File
# Begin Source File

SOURCE=.\ifo_read.h
# End Source File
# Begin Source File

SOURCE=.\ifo_types.h
# End Source File
# Begin Source File

SOURCE=.\ioctl.h
# End Source File
# Begin Source File

SOURCE=.\libdvdcss.h
# End Source File
# Begin Source File

SOURCE=.\nav_print.h
# End Source File
# Begin Source File

SOURCE=.\nav_read.h
# End Source File
# Begin Source File

SOURCE=.\nav_types.h
# End Source File
# Begin Source File

SOURCE=.\pthread.h
# End Source File
# Begin Source File

SOURCE=.\read_cache.h
# End Source File
# Begin Source File

SOURCE=.\vm.h
# End Source File
# Begin Source File

SOURCE=.\vmcmd.h
# End Source File
# End Group
# End Target
# End Project
