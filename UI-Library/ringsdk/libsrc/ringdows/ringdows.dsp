# Microsoft Developer Studio Project File - Name="ringdows" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ringdows - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ringdows.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ringdows.mak" CFG="ringdows - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ringdows - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ringdows - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ringdows - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /EHa /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ringdows - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\lib"
# PROP Intermediate_Dir "debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\ringdowsd.lib"

!ENDIF 

# Begin Target

# Name "ringdows - Win32 Release"
# Name "ringdows - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\controls.cpp
# End Source File
# Begin Source File

SOURCE=.\ringbasewnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ringbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\ringchecklist.cpp
# End Source File
# Begin Source File

SOURCE=.\ringcombo.cpp
# End Source File
# Begin Source File

SOURCE=.\ringcoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ringdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ringdockbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ringdocksite.cpp
# End Source File
# Begin Source File

SOURCE=.\ringdragger.cpp
# End Source File
# Begin Source File

SOURCE=.\ringedit.cpp
# End Source File
# Begin Source File

SOURCE=.\ringheader.cpp
# End Source File
# Begin Source File

SOURCE=.\ringimagelist.cpp
# End Source File
# Begin Source File

SOURCE=.\ringipaddr.cpp
# End Source File
# Begin Source File

SOURCE=.\ringlist.cpp
# End Source File
# Begin Source File

SOURCE=.\ringlistview.cpp
# End Source File
# Begin Source File

SOURCE=.\ringmdichild.cpp
# End Source File
# Begin Source File

SOURCE=.\ringmdiframewnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ringmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ringmenubar.cpp
# End Source File
# Begin Source File

SOURCE=.\ringoptionview.cpp
# End Source File
# Begin Source File

SOURCE=.\ringoutlookbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ringpropsheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ringscrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ringsizebar.cpp
# End Source File
# Begin Source File

SOURCE=.\ringstatic.cpp
# End Source File
# Begin Source File

SOURCE=.\ringstatus.cpp
# End Source File
# Begin Source File

SOURCE=.\ringtab.cpp
# End Source File
# Begin Source File

SOURCE=.\ringtoolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\ringtooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\ringtree.cpp
# End Source File
# Begin Source File

SOURCE=.\ringtreedir.cpp
# End Source File
# Begin Source File

SOURCE=.\ringwnd.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\ringbutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringchecklist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringcombo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringcontrols.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringcoolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringdataspool.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringdockbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringdowbase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringdows.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringedit.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringheader.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringimagelist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringipaddr.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringlist.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringlistview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringmenu.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringmenubar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringoptionview.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringoutlook.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringprogress.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringPropSheet.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringscrollbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringspinbutton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringstatic.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringstatus.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringtab.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringtoolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringtooltip.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringtrackbar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ringtree.h
# End Source File
# End Group
# End Target
# End Project
