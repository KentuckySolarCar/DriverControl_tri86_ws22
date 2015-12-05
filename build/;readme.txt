TRI86 EV driver controls MSP430 firmware README - 2015
=============================================================================
- This firmware is written to use the GNU GCC3 toolchain, please read the install instructions below.
- Refer to the board schematics, available on the Tritium website, for pinouts and device functionality.
- The firmware is licenced using the BSD licence.  There is no obligation to publish modifications.
- If you've written something cool and would like to share, please let us know about it!
- Contact James Kennedy with any questions or comments: james@tritium.com.au


Toolchain Installation walkthrough (Windows MSPGCC instructions)
----------------------------------------------------------------------------
- Download and install the 2008-12-30 release of MSPGCC3:  (don't install giveio unless you actually need it)
  http://sourceforge.net/projects/mspgcc/files/Outdated/mspgcc-win32/snapshots/mspgcc-20081230.exe/download
  The Tritium bootloader tool requires this specific version of GCC, as it has not yet been updated to use the
  output from the current GCC4 tools.
  
- Install mspgcc_postInstall.exe. In the Extract prompt choose to extract to "C:\". Also say "yes" to file replacement requests
  This will patch a number of mspgcc headers found to have errors in them. Also provides the missing msvcrXX.dll file

- Download and install the current MinGW, and use the install options for the C compliler + MSYS.  This gets you 'make', 'rm', etc 
  http://sourceforge.net/projects/mingw/files/

- Add to the end of your path environment variable (control panel -> system -> advanced -> environment variables -> path -> edit or similar) the two things bellow:  
  " ;c:\MinGW\bin; c:\MinGW\msys\1.0\bin " or whatever directory you installed MinGW to. 
  
- Download and install ActiveState Perl. Perl is used to generate the project makefile and pass build errors to Visual Studio 2005
  http://www.activestate.com/activeperl/downloads

 
Project compilation walkthrough (Windows Visual Studio 2005):
----------------------------------------------------------------------------
NB: Project will compile with newer versions of VS. But error reporting will not work, manually check build_msp430.log for compilation errors and warnings

- Open the project "tri86.sln" in Visual Studio
  
- Add any new .c or .h files to the project tree. .c files should be saved in src/ folder, .h files should be saved in include/ folder

- To Build the project click Build -> Build Solution. The make file will be updated to include any new .c or .h files and the build number will be incremented
  The device .a43 binary and final .tsf output files will be automatically generated
  
- To Clean the project click Build -> Clean Solution. All device binaries and final tsf's will be deleted


Project compilation walkthrough (Windows Command Prompt):
----------------------------------------------------------------------------
- Open a cmd prompt window, change to the project root directory where this readme exists

- To Build the project execute "build_msp430.bat". The make file will be generated and the build number will be incremented
  The device .a43 binary and final .tsf output files will be automatically generated
  
- To clean the project execute "build_msp430.bat clean". All device binaries and final tsf's will be deleted

- To add any new .c file to the project you will have to manually add it to the makefile "OBJECTS" list, otherwise it will not be compiled
  !!!If you edit the makefile directly you must comment/delete lines 35-111 in "build_msp430.pl" otherwise your changes will be overriden!!!
  .c files should be saved in src/ folder, .h files should be saved in include/ folder
  
  
Project General:
----------------------------------------------------------------------------
- To change the Hardware Version that the code is compiled for open "build_msp430.bat"
  Adjust the brdVer value at the top of the file to mach your driver controls hardware version number 

- Use the triFwLoad tool from the Tritium website to bootload the .tsf file into the driver controls device over the CAN bus

- You will notice that two versions of .a43 binaries and .tsf's will be generated. A new version bootloader is now installed on devices shipped from tritium
  The triFwLoad program will notify you if you try to load the wrong version .tsf to the device
  *Devices shipped after the start of 2015 use the tri86.tsf filename
  *Older devices use the tri86_oldBL.tsf
