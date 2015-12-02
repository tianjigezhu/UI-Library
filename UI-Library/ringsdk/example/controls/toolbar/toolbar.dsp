# Microsoft Developer Studio Project File - Name="toolbar" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=toolbar - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "toolbar.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "toolbar.mak" CFG="toolbar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "toolbar - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "toolbar - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "toolbar - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "toolbar - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "toolbar - Win32 Release"
# Name "toolbar - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\child.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\001sys.ico
# End Source File
# Begin Source File

SOURCE=.\res\_actvex.ico
# End Source File
# Begin Source File

SOURCE=.\res\_app.ico
# End Source File
# Begin Source File

SOURCE=.\res\_DLL.ico
# End Source File
# Begin Source File

SOURCE=.\res\_server.ico
# End Source File
# Begin Source File

SOURCE=.\res\a.ico
# End Source File
# Begin Source File

SOURCE=.\res\BACK100.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Backup Archive.ico"
# End Source File
# Begin Source File

SOURCE=.\res\bkg.bmp
# End Source File
# Begin Source File

SOURCE=.\bkg1.BMP
# End Source File
# Begin Source File

SOURCE=.\res\buildbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\capture.ico
# End Source File
# Begin Source File

SOURCE=.\res\character1.ico
# End Source File
# Begin Source File

SOURCE=.\res\copy.ico
# End Source File
# Begin Source File

SOURCE=.\copyright.bmp
# End Source File
# Begin Source File

SOURCE=.\res\explorer4.ICO
# End Source File
# Begin Source File

SOURCE=.\res\find.ico
# End Source File
# Begin Source File

SOURCE=.\res\fullscre.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00005.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00006.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico4103.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 02.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 03.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 16.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 17.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 18.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 20.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 37.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 46.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 47.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 65.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 73.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 74.ico"
# End Source File
# Begin Source File

SOURCE=".\res\LaST (Cobalt) V1 Icon 80.ico"
# End Source File
# Begin Source File

SOURCE=.\res\open.ico
# End Source File
# Begin Source File

SOURCE=.\res\paste.ico
# End Source File
# Begin Source File

SOURCE=.\res\redo.ico
# End Source File
# Begin Source File

SOURCE=.\res\shhothi.bmp
# End Source File
# Begin Source File

SOURCE=.\shhothi.bmp
# End Source File
# Begin Source File

SOURCE=.\res\shhothi_gray.bmp
# End Source File
# Begin Source File

SOURCE=.\shhothi_gray.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tileh.ico
# End Source File
# Begin Source File

SOURCE=.\res\tilev.ico
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBAR.BMP
# End Source File
# Begin Source File

SOURCE=.\TOOLBAR.BMP
# End Source File
# Begin Source File

SOURCE=.\toolbar.rc
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\undo.ico
# End Source File
# Begin Source File

SOURCE=.\res\User.ico
# End Source File
# End Group
# End Target
# End Project
