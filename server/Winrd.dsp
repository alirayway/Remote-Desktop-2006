# Microsoft Developer Studio Project File - Name="Winrd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101
# TARGTYPE "Win32 (ALPHA) Application" 0x0601

CFG=Winrd - Win32 Profile
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Winrd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Winrd.mak" CFG="Winrd - Win32 Profile"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Winrd - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Winrd - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Winrd - Win32 Purify" (based on "Win32 (x86) Application")
!MESSAGE "Winrd - Win32 No_CORBA" (based on "Win32 (x86) Application")
!MESSAGE "Winrd - Win32 Profile" (based on "Win32 (x86) Application")
!MESSAGE "Winrd - Win32 Alpha No_CORBA" (based on "Win32 (ALPHA) Application")
!MESSAGE "Winrd - Win32 Alpha Debug No_CORBA" (based on "Win32 (ALPHA) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Winrd", IABAAAAA"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "Winrd - Win32 Release"

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
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./omnithread" /I "\\shallot\omni\release\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "_CORBA" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib omniORB270_rt.lib /nologo /subsystem:windows /machine:I386 /libpath:"\\shallot\omni\release\lib\x86_nt_3.5" /libpath:"\\shallot\omni\release\lib\x86_nt_4.0"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

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
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./omnithread" /I "\\shallot\omni\release\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "NCORBA" /D "ZLIB_DLL" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /incremental:no /map /debug /machine:I386 /pdbtype:sept /libpath:"\\shallot\omni\release\lib\x86_nt_3.5"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Winrd__"
# PROP BASE Intermediate_Dir "Winrd__"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Purify"
# PROP Intermediate_Dir "Purify"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "O:\develop\include" /I "f:\Work\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./omnithread" /I "\\shallot\omni\release\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "_CORBA" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib omniORB2_rtd.lib omnithread_rtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"O:\develop\lib\x86_nt_3.5"
# ADD LINK32 omniORB260_rtd.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"\\shallot\omni\release\lib\x86_nt_3.5"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Winrd__"
# PROP BASE Intermediate_Dir "Winrd__"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "No_CORBA"
# PROP Intermediate_Dir "No_CORBA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "J:\develop\include" /I "f:\Work\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./omnithread" /I "./zlib" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "_WINSTATIC" /D "NCORBA" /D "ZLIB_DLL" /D "XMD_H" /Fr /YX /FD /c
# SUBTRACT CPP /X
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib omniORB2_rt.lib omnithread_rt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /libpath:"J:\develop\lib\x86_nt_3.5"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Winrd__"
# PROP BASE Intermediate_Dir "Winrd__"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Profile"
# PROP Intermediate_Dir "Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "O:\release\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "_CORBA" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./omnithread" /I "\\shallot\omni\release\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__x86__" /D "_CORBA" /YX /FD /c
MTL=midl.exe
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib omniORB2_rtd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"o:\release\lib\x86_nt_3.5"
# ADD LINK32 omniORB260_rtd.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /profile /debug /machine:I386 /libpath:"\\shallot\omni\release\lib\x86_nt_3.5"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Winrd__"
# PROP BASE Intermediate_Dir "Winrd__"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Alpha_No_CORBA"
# PROP Intermediate_Dir "Alpha_No_CORBA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /Gt0 /W3 /GX /O2 /I "./omnithread" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINSTATIC" /D "NCORBA" /YX /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /MD /Gt0 /W3 /GX /O2 /I "./omnithread" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__axp__" /D "_WINSTATIC" /D "NCORBA" /YX /FD /c
# SUBTRACT CPP /X
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:ALPHA
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:ALPHA
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Winrd__"
# PROP BASE Intermediate_Dir "Winrd__"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "AlphaDbg_No_CORBA"
# PROP Intermediate_Dir "AlphaDbg_No_CORBA"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
CPP=cl.exe
# ADD BASE CPP /nologo /MD /Gt0 /W3 /GX /O2 /I "./omnithread" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__axp__" /D "_WINSTATIC" /D "NCORBA" /YX /FD /c
# SUBTRACT BASE CPP /X
# ADD CPP /nologo /Gt0 /W3 /GX /Zi /Od /I "./omnithread" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "__WIN32__" /D "__NT__" /D "__axp__" /D "_WINSTATIC" /D "NCORBA" /YX /FD /MDd /c
# SUBTRACT CPP /X
MTL=midl.exe
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:ALPHA
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:ALPHA /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Winrd - Win32 Release"
# Name "Winrd - Win32 Debug"
# Name "Winrd - Win32 Purify"
# Name "Winrd - Win32 No_CORBA"
# Name "Winrd - Win32 Profile"
# Name "Winrd - Win32 Alpha No_CORBA"
# Name "Winrd - Win32 Alpha Debug No_CORBA"
# Begin Group "Source"

# PROP Default_Filter ".cpp, .c"
# Begin Source File

SOURCE=.\d3des.c

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Log.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MinMax.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdAbout.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdAcceptDialog.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdAdvancedProperties.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdauth.c

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdBuffer.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdClient.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdConnDialog.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdDesktop.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdDesktopDX.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\rdEncodeCoRRE.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdEncodeHexT.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdEncoder.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdEncodeRRE.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdEncodeTight.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdEncodeZlib.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdEncodeZlibHex.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdHTTPConnect.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdInstHandler.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdKeymap.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdMenu.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdProperties.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdRegion.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdServer.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdService.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdSockConnect.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdTimedMsgBox.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RectList.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\stdhdrs.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tableinitcmtemplate.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tableinittctemplate.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\tabletranstemplate.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\translate.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VSocket.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Winrd.cpp

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Winrd.rc
# End Source File
# End Group
# Begin Group "Headers"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\d3des.h
# End Source File
# Begin Source File

SOURCE=.\keysymdef.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=.\MinMax.h
# End Source File
# Begin Source File

SOURCE=.\rd.hh

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rd.idl
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\rdAbout.h
# End Source File
# Begin Source File

SOURCE=.\rdAcceptDialog.h
# End Source File
# Begin Source File

SOURCE=.\rdauth.h
# End Source File
# Begin Source File

SOURCE=.\rdBuffer.h
# End Source File
# Begin Source File

SOURCE=.\rdClient.h
# End Source File
# Begin Source File

SOURCE=.\rdConnDialog.h
# End Source File
# Begin Source File

SOURCE=.\rdCorbaConnect.h
# End Source File
# Begin Source File

SOURCE=.\rdDesktop.h
# End Source File
# Begin Source File

SOURCE=.\rdEncodeCoRRE.h

!IF  "$(CFG)" == "Winrd - Win32 Release"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Purify"

!ELSEIF  "$(CFG)" == "Winrd - Win32 No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Profile"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha No_CORBA"

!ELSEIF  "$(CFG)" == "Winrd - Win32 Alpha Debug No_CORBA"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rdEncodeHexT.h
# End Source File
# Begin Source File

SOURCE=.\rdEncoder.h
# End Source File
# Begin Source File

SOURCE=.\rdEncodeRRE.h
# End Source File
# Begin Source File

SOURCE=.\rdEncodeTight.h
# End Source File
# Begin Source File

SOURCE=.\rdEncodeZlib.h
# End Source File
# Begin Source File

SOURCE=.\rdEncodeZlibHex.h
# End Source File
# Begin Source File

SOURCE=.\rdHTTPConnect.h
# End Source File
# Begin Source File

SOURCE=.\rdInstHandler.h
# End Source File
# Begin Source File

SOURCE=.\rdKeymap.h
# End Source File
# Begin Source File

SOURCE=.\rdMenu.h
# End Source File
# Begin Source File

SOURCE=.\rdPasswd.h
# End Source File
# Begin Source File

SOURCE=.\rdProperties.h
# End Source File
# Begin Source File

SOURCE=.\rdRegion.h
# End Source File
# Begin Source File

SOURCE=.\rdServer.h
# End Source File
# Begin Source File

SOURCE=.\rdService.h
# End Source File
# Begin Source File

SOURCE=.\rdSockConnect.h
# End Source File
# Begin Source File

SOURCE=.\rdTimedMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\RectList.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\rfb.h
# End Source File
# Begin Source File

SOURCE=.\rfbproto.h
# End Source File
# Begin Source File

SOURCE=.\stdhdrs.h
# End Source File
# Begin Source File

SOURCE=.\translate.h
# End Source File
# Begin Source File

SOURCE=.\VSocket.h
# End Source File
# Begin Source File

SOURCE=.\VTypes.h
# End Source File
# Begin Source File

SOURCE=.\Winrd.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\tightrd.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Winrd.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\BUILDING.txt
# End Source File
# Begin Source File

SOURCE=.\History.txt
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\rd.bmp
# End Source File
# End Target
# End Project
