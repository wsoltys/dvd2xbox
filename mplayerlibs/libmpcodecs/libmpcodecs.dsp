# Microsoft Developer Studio Project File - Name="libmpcodecs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=libmpcodecs - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libmpcodecs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libmpcodecs.mak" CFG="libmpcodecs - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libmpcodecs - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "libmpcodecs - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "libmpcodecs - Xbox Release"

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

!ELSEIF  "$(CFG)" == "libmpcodecs - Xbox Debug"

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
# ADD LIB32 /nologo /out:"Debug\libmpcodecsd.lib"

!ENDIF 

# Begin Target

# Name "libmpcodecs - Xbox Release"
# Name "libmpcodecs - Xbox Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ad.c
# End Source File
# Begin Source File

SOURCE=.\ad_alaw.c
# End Source File
# Begin Source File

SOURCE=.\ad_dk3adpcm.c
# End Source File
# Begin Source File

SOURCE=.\ad_dmo.c
# End Source File
# Begin Source File

SOURCE=.\ad_dshow.c
# End Source File
# Begin Source File

SOURCE=.\ad_dvdpcm.c
# End Source File
# Begin Source File

SOURCE=.\ad_faad.c
# End Source File
# Begin Source File

SOURCE=.\ad_ffmpeg.c
# End Source File
# Begin Source File

SOURCE=.\ad_hwac3.c
# End Source File
# Begin Source File

SOURCE=.\ad_imaadpcm.c
# End Source File
# Begin Source File

SOURCE=.\ad_liba52.c
# End Source File
# Begin Source File

SOURCE=.\ad_libdv.c
# End Source File
# Begin Source File

SOURCE=.\ad_libmad.c
# End Source File
# Begin Source File

SOURCE=.\ad_libvorbis.c
# End Source File
# Begin Source File

SOURCE=.\ad_mp3lib.c
# End Source File
# Begin Source File

SOURCE=.\ad_msadpcm.c
# End Source File
# Begin Source File

SOURCE=.\ad_msgsm.c
# End Source File
# Begin Source File

SOURCE=.\ad_pcm.c
# End Source File
# Begin Source File

SOURCE=.\ad_qtaudio.c
# End Source File
# Begin Source File

SOURCE=.\ad_realaud.c
# End Source File
# Begin Source File

SOURCE=.\ad_roqaudio.c
# End Source File
# Begin Source File

SOURCE=.\native\cinepak.c
# End Source File
# Begin Source File

SOURCE="..\codec-cfg.c"
# End Source File
# Begin Source File

SOURCE=.\native\cyuv.c
# End Source File
# Begin Source File

SOURCE=.\dec_audio.c
# End Source File
# Begin Source File

SOURCE=.\dec_video.c
# End Source File
# Begin Source File

SOURCE=.\native\fli.c
# End Source File
# Begin Source File

SOURCE=.\img_format.c
# End Source File
# Begin Source File

SOURCE=.\native\minilzo.c
# End Source File
# Begin Source File

SOURCE=.\native\msvidc.c
# End Source File
# Begin Source File

SOURCE=.\native\nuppelvideo.c
# End Source File
# Begin Source File

SOURCE=.\native\qtrle.c
# End Source File
# Begin Source File

SOURCE=.\native\qtrpza.c
# End Source File
# Begin Source File

SOURCE=.\native\qtsmc.c
# End Source File
# Begin Source File

SOURCE=.\native\roqav.c
# End Source File
# Begin Source File

SOURCE=.\native\svq1.c
# End Source File
# Begin Source File

SOURCE=.\vd.c
# End Source File
# Begin Source File

SOURCE=.\vd_cinepak.c
# End Source File
# Begin Source File

SOURCE=.\vd_cyuv.c
# End Source File
# Begin Source File

SOURCE=.\vd_divx4.c
# End Source File
# Begin Source File

SOURCE=.\vd_dmo.c
# End Source File
# Begin Source File

SOURCE=.\vd_dshow.c
# End Source File
# Begin Source File

SOURCE=.\vd_ffmpeg.c
# End Source File
# Begin Source File

SOURCE=.\vd_fli.c
# End Source File
# Begin Source File

SOURCE=.\vd_huffyuv.c
# End Source File
# Begin Source File

SOURCE=.\vd_libdv.c
# End Source File
# Begin Source File

SOURCE=.\vd_libmpeg2.c
# End Source File
# Begin Source File

SOURCE=.\vd_lzo.c
# End Source File
# Begin Source File

SOURCE=.\vd_mpegpes.c
# End Source File
# Begin Source File

SOURCE=.\vd_mpng.c
# End Source File
# Begin Source File

SOURCE=.\vd_msrle.c
# End Source File
# Begin Source File

SOURCE=.\vd_msvidc.c
# End Source File
# Begin Source File

SOURCE=.\vd_mtga.c
# End Source File
# Begin Source File

SOURCE=.\vd_null.c
# End Source File
# Begin Source File

SOURCE=.\vd_nuv.c
# End Source File
# Begin Source File

SOURCE=.\vd_odivx.c
# End Source File
# Begin Source File

SOURCE=.\vd_qtrle.c
# End Source File
# Begin Source File

SOURCE=.\vd_qtrpza.c
# End Source File
# Begin Source File

SOURCE=.\vd_qtsmc.c
# End Source File
# Begin Source File

SOURCE=.\vd_qtvideo.c
# End Source File
# Begin Source File

SOURCE=.\vd_raw.c
# End Source File
# Begin Source File

SOURCE=.\vd_realvid.c
# End Source File
# Begin Source File

SOURCE=.\vd_roqvideo.c
# End Source File
# Begin Source File

SOURCE=.\vd_svq1.c
# End Source File
# Begin Source File

SOURCE=.\vd_theora.c
# End Source File
# Begin Source File

SOURCE=.\vd_xvid.c
# End Source File
# Begin Source File

SOURCE=.\vf.c
# End Source File
# Begin Source File

SOURCE=.\vf_1bpp.c
# End Source File
# Begin Source File

SOURCE=.\vf_2xsai.c
# End Source File
# Begin Source File

SOURCE=.\vf_bmovl.c
# End Source File
# Begin Source File

SOURCE=.\vf_boxblur.c
# End Source File
# Begin Source File

SOURCE=.\vf_crop.c
# End Source File
# Begin Source File

SOURCE=.\vf_cropdetect.c
# End Source File
# Begin Source File

SOURCE=.\vf_dint.c
# End Source File
# Begin Source File

SOURCE=.\vf_dvbscale.c
# End Source File
# Begin Source File

SOURCE=.\vf_expand.c
# End Source File
# Begin Source File

SOURCE=.\vf_fame.c
# End Source File
# Begin Source File

SOURCE=.\vf_flip.c
# End Source File
# Begin Source File

SOURCE=.\vf_format.c
# End Source File
# Begin Source File

SOURCE=.\vf_il.c
# End Source File
# Begin Source File

SOURCE=.\vf_lavc.c
# End Source File
# Begin Source File

SOURCE=.\vf_lavcdeint.c
# End Source File
# Begin Source File

SOURCE=.\vf_mirror.c
# End Source File
# Begin Source File

SOURCE=.\vf_palette.c
# End Source File
# Begin Source File

SOURCE=.\vf_perspective.c
# End Source File
# Begin Source File

SOURCE=.\vf_pp.c
# End Source File
# Begin Source File

SOURCE=.\vf_rectangle.c
# End Source File
# Begin Source File

SOURCE=.\vf_rgb2bgr.c
# End Source File
# Begin Source File

SOURCE=.\vf_rotate.c
# End Source File
# Begin Source File

SOURCE=.\vf_sab.c
# End Source File
# Begin Source File

SOURCE=.\vf_scale.c
# End Source File
# Begin Source File

SOURCE=.\vf_smartblur.c
# End Source File
# Begin Source File

SOURCE=.\vf_swapuv.c
# End Source File
# Begin Source File

SOURCE=.\vf_telecine.c
# End Source File
# Begin Source File

SOURCE=.\vf_test.c
# End Source File
# Begin Source File

SOURCE=.\vf_unsharp.c
# End Source File
# Begin Source File

SOURCE=.\vf_vo.c
# End Source File
# Begin Source File

SOURCE=.\vf_yuy2.c
# End Source File
# Begin Source File

SOURCE=.\vf_yvu9.c
# End Source File
# Begin Source File

SOURCE=.\native\xa_gsm.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
