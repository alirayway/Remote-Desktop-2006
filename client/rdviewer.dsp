# Microsoft Developer Studio Project File - Name="rdviewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=rdviewer - Win32 Profile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "rdviewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "rdviewer.mak" CFG="rdviewer - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "rdviewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "rdviewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "rdviewer - Win32 Profile" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/rdviewer", KYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rdviewer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob0 /I "omnithread" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "__NT__" /D "_WINSTATIC" /D "__WIN32__" /D "XMD_H" /D "NEED_FAR_POINTERS" /FR /YX"stdhdrs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib omnithread.lib zlib.lib libjpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib /nologo /subsystem:windows /machine:I386 /libpath:"omnithread/Release" /libpath:"zlib/Release" /libpath:"libjpeg/Release"

!ELSEIF  "$(CFG)" == "rdviewer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "omnithread" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__NT__" /D "_WINSTATIC" /D "__WIN32__" /D "XMD_H" /FR /YX"stdhdrs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ole32.lib oleaut32.lib winmm.lib omnithread.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept /libpath:"./omnithread/Debug"

!ELSEIF  "$(CFG)" == "rdviewer - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "rdviewe"
# PROP BASE Intermediate_Dir "rdviewe"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Profile"
# PROP Intermediate_Dir "Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "omnithread" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__NT__" /D "_WINSTATIC" /FR /YX"stdhdrs.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "omnithread" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "__NT__" /D "_WINSTATIC" /D "__WIN32__" /FR /YX"stdhdrs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ole32.lib oleaut32.lib winmm.lib omnithread.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"./omnithread/Debug"
# ADD LINK32 ole32.lib oleaut32.lib winmm.lib omnithread.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib wsock32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /libpath:"./omnithread/Debug"

!ENDIF 

# Begin Target

# Name "rdviewer - Win32 Release"
# Name "rdviewer - Win32 Debug"
# Name "rdviewer - Win32 Profile"
# Begin Group "Resources"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\idr_tray.ico
# End Source File
# Begin Source File

SOURCE=.\res\nocursor.cur
# End Source File
# Begin Source File

SOURCE=.\res\rd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rd32.BMP
# End Source File
# Begin Source File

SOURCE=.\res\rd48.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rdviewer.ico
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AboutBox.cpp
# End Source File
# Begin Source File

SOURCE=.\AboutBox.h
# End Source File
# Begin Source File

SOURCE=.\AuthDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AuthDialog.h
# End Source File
# Begin Source File

SOURCE=.\ClientConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnection.h

!IF  "$(CFG)" == "rdviewer - Win32 Release"

!ELSEIF  "$(CFG)" == "rdviewer - Win32 Debug"

!ELSEIF  "$(CFG)" == "rdviewer - Win32 Profile"

# PROP Intermediate_Dir "Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ClientConnectionClipboard.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionCopyRect.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionCoRRE.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionFullScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionHextile.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionRaw.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionRRE.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionTight.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionZlib.cpp
# End Source File
# Begin Source File

SOURCE=.\ClientConnectionZlibHex.cpp
# End Source File
# Begin Source File

SOURCE=.\d3des.c
# End Source File
# Begin Source File

SOURCE=.\d3des.h
# End Source File
# Begin Source File

SOURCE=.\Daemon.cpp
# End Source File
# Begin Source File

SOURCE=.\Daemon.h
# End Source File
# Begin Source File

SOURCE=.\Exception.cpp
# End Source File
# Begin Source File

SOURCE=.\Exception.h
# End Source File
# Begin Source File

SOURCE=.\Flasher.cpp
# End Source File
# Begin Source File

SOURCE=.\Flasher.h
# End Source File
# Begin Source File

SOURCE=.\KeyMap.cpp
# End Source File
# Begin Source File

SOURCE=.\KeyMap.h
# End Source File
# Begin Source File

SOURCE=.\keysymdef.h
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\MRU.cpp
# End Source File
# Begin Source File

SOURCE=.\MRU.h
# End Source File
# Begin Source File

SOURCE=.\rdauth.c
# End Source File
# Begin Source File

SOURCE=.\rdauth.h
# End Source File
# Begin Source File

SOURCE=.\rdOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\rdOptions.h
# End Source File
# Begin Source File

SOURCE=.\rdviewer.cpp
# End Source File
# Begin Source File

SOURCE=.\rdviewer.h
# End Source File
# Begin Source File

SOURCE=.\res\rdviewer.rc
# End Source File
# Begin Source File

SOURCE=.\rdviewerApp.cpp
# End Source File
# Begin Source File

SOURCE=.\rdviewerApp.h
# End Source File
# Begin Source File

SOURCE=.\rdviewerApp32.cpp
# End Source File
# Begin Source File

SOURCE=.\rdviewerApp32.h
# End Source File
# Begin Source File

SOURCE=.\res\resource.h
# End Source File
# Begin Source File

SOURCE=.\rfb.h
# End Source File
# Begin Source File

SOURCE=.\rfbproto.h
# End Source File
# Begin Source File

SOURCE=.\SessionDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SessionDialog.h
# End Source File
# Begin Source File

SOURCE=.\stdhdrs.cpp

!IF  "$(CFG)" == "rdviewer - Win32 Release"

# ADD CPP /YX"stdhdrs.h"

!ELSEIF  "$(CFG)" == "rdviewer - Win32 Debug"

# ADD CPP /Yc"stdhdrs.h"

!ELSEIF  "$(CFG)" == "rdviewer - Win32 Profile"

# ADD BASE CPP /Yc"stdhdrs.h"
# ADD CPP /Yc"stdhdrs.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stdhdrs.h
# End Source File
# End Target
# End Project
