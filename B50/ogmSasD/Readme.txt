
Building the 32-Bit library variants:
-------------------------------------

Use the build wizard (Tools\buildwiz.exe) to define the
features that should go into the libraries. Then start
Developer Studio and load buildw.mdp (or buildw.dsw).

In Developer Studio you can choose which libraries to
build by changing the build configuration menu item.

You have following options: lib

- Win32 OGLIB MFCLIB Release
- Win32 OGLIB MFCLIB Debug
- Win32 OGLIB MFCDLL Release
- Win32 OGLIB MFCDLL Debug
- Win32 OGDLL MFCDLL Release
- Win32 OGDLL MFCDLL Debug
- Win32 OGLIB MFCLIB Unicode Release
- Win32 OGLIB MFCLIB Unicode Debug
- Win32 OGLIB MFCDLL Unicode Release
- Win32 OGLIB MFCDLL Unicode Debug
- Win32 OGDLL MFCDLL Unicode Release
- Win32 OGDLL MFCDLL Unicode Debug
- Win32 All
- Win32 Ansi All
- Win32 Unicode All

You can also run build.mak directly from command line.

Usage: NMAKE /f build.mak CLEAN   (removes all intermediary files)
   or: NMAKE /f build.mak         (builds all library variants (see below))
   or: NMAKE /f build.mak Target  (builds a specific library variant)

'Target' can be are one of:
   "og"     32-Bit MFC Lib, OG Lib, Release
   "ogd"    32-Bit MFC Lib, OG Lib, Debug
   "oga"    32-Bit MFC DLL, OG Lib, Release
   "ogad"   32-Bit MFC DLL, OG Lib, Debug
   "ogas"   32-Bit MFC DLL, OG DLL, Release
   "ogasd"  32-Bit MFC DLL, OG DLL, Debug
   "ogu"    32-Bit MFC Lib, OG Lib, Uncidoe, Release
   "ogud"   32-Bit MFC Lib, OG Lib, Unicode, Debug
   "ogau"   32-Bit MFC DLL, OG Lib, Unicode, Release
   "ogaud"  32-Bit MFC DLL, OG Lib, Unicode, Debug
   "ogasu"  32-Bit MFC DLL, OG DLL, Unicode, Release
   "ogasud" 32-Bit MFC DLL, OG DLL, Unicode, Debug

Example:
NMAKE /f build.mak ansi




