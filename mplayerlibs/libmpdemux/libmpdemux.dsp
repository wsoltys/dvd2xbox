# Microsoft Developer Studio Project File - Name="libmpdemux" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=libmpdemux - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmpdemux.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmpdemux.mak" CFG="libmpdemux - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmpdemux - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "libmpdemux - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "libmpdemux - Xbox Release"

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
# ADD CPP /nologo /W3 /GR /GX /O2 /Ob0 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libmpdemux - Xbox Debug"

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
# ADD CPP /nologo /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\libmpdemuxd.lib"

!ENDIF 

# Begin Target

# Name "libmpdemux - Xbox Release"
# Name "libmpdemux - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\asf_mmst_streaming.c
# End Source File
# Begin Source File

SOURCE=.\asf_streaming.c
# End Source File
# Begin Source File

SOURCE=.\asfheader.c
# End Source File
# Begin Source File

SOURCE=.\aviheader.c
# End Source File
# Begin Source File

SOURCE=.\aviprint.c
# End Source File
# Begin Source File

SOURCE=.\aviwrite.c
# End Source File
# Begin Source File

SOURCE=.\cdda.c
# End Source File
# Begin Source File

SOURCE=.\css.c
# End Source File
# Begin Source File

SOURCE=.\css_auth.c
# End Source File
# Begin Source File

SOURCE=.\css_descramble.c
# End Source File
# Begin Source File

SOURCE=.\demux_asf.c
# End Source File
# Begin Source File

SOURCE=.\demux_audio.c
# End Source File
# Begin Source File

SOURCE=.\demux_avi.c
# End Source File
# Begin Source File

SOURCE=.\demux_bmp.c
# End Source File
# Begin Source File

SOURCE=.\demux_demuxers.c
# End Source File
# Begin Source File

SOURCE=.\demux_film.c
# End Source File
# Begin Source File

SOURCE=.\demux_fli.c
# End Source File
# Begin Source File

SOURCE=.\demux_mf.c
# End Source File
# Begin Source File

SOURCE=.\demux_mkv.cpp
# End Source File
# Begin Source File

SOURCE=.\demux_mov.c
# End Source File
# Begin Source File

SOURCE=.\demux_mpg.c
# End Source File
# Begin Source File

SOURCE=.\demux_nuv.c
# End Source File
# Begin Source File

SOURCE=.\demux_ogg.c
# End Source File
# Begin Source File

SOURCE=.\demux_pva.c
# End Source File
# Begin Source File

SOURCE=.\demux_rawaudio.c
# End Source File
# Begin Source File

SOURCE=.\demux_rawdv.c
# End Source File
# Begin Source File

SOURCE=.\demux_rawvideo.c
# End Source File
# Begin Source File

SOURCE=.\demux_real.c
# End Source File
# Begin Source File

SOURCE=.\demux_realaud.c
# End Source File
# Begin Source File

SOURCE=.\demux_roq.c
# End Source File
# Begin Source File

SOURCE=.\demux_smjpeg.c
# End Source File
# Begin Source File

SOURCE=.\demux_ts.c
# End Source File
# Begin Source File

SOURCE=.\demux_viv.c
# End Source File
# Begin Source File

SOURCE=.\demux_xmms.c
# End Source File
# Begin Source File

SOURCE=.\demux_y4m.c
# End Source File
# Begin Source File

SOURCE=.\demuxer.c
# End Source File
# Begin Source File

SOURCE=.\dvd.c
# End Source File
# Begin Source File

SOURCE=.\dvd_util.c
# End Source File
# Begin Source File

SOURCE=.\dvdauth.c
# End Source File
# Begin Source File

SOURCE=.\dvdnav_stream.c
# End Source File
# Begin Source File

SOURCE=.\extension.c
# End Source File
# Begin Source File

SOURCE=.\frequencies.c
# End Source File
# Begin Source File

SOURCE=.\http.c
# End Source File
# Begin Source File

SOURCE=.\mf.c
# End Source File
# Begin Source File

SOURCE=.\mp3_hdr.c
# End Source File
# Begin Source File

SOURCE=.\mp_cmd.c
# End Source File
# Begin Source File

SOURCE=.\mpeg_hdr.c
# End Source File
# Begin Source File

SOURCE=.\network.c
# End Source File
# Begin Source File

SOURCE=.\open.c
# End Source File
# Begin Source File

SOURCE=.\parse_es.c
# End Source File
# Begin Source File

SOURCE=.\parse_mp4.c
# End Source File
# Begin Source File

SOURCE=.\stream.c
# End Source File
# Begin Source File

SOURCE=.\stream_file.c
# End Source File
# Begin Source File

SOURCE=..\sub_cc.c
# End Source File
# Begin Source File

SOURCE=..\subreader.c
# End Source File
# Begin Source File

SOURCE=.\url.c
# End Source File
# Begin Source File

SOURCE=.\video.c
# End Source File
# Begin Source File

SOURCE=.\yuv4mpeg.c
# End Source File
# Begin Source File

SOURCE=.\yuv4mpeg_ratio.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
