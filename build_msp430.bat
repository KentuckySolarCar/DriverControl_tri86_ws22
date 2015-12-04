@echo off

REM SET THE TARGET HARDWARE VERSION OF THE BOARD HERE
set brdVer=4

rem Pass any extra make commands to perl script so the build number isn't incremented
perl build_msp430.pl %1

rem Compile project and redirect stderr to log
rm -f *.a43
make %1 2> build_msp430.log

rem Process any compile errors or warnings
@echo on
perl errproc_msp430.pl
@echo off

set /p key=<tri86.key.txt
rem Dont create tsf's if doing clean or error was found during compile
if "%1" == "" (
	if %errorlevel%==0 (
		if EXIST *.a43 (
			msp430encrypt_Public.exe tri86.a43 tri86.tsf 0x00001002 %brdVer%
			msp430encrypt.exe tri86_oldBL.a43 tri86_oldBL.tsf 0x00001002 %brdVer% %key% 0x0001
		) ELSE (
			echo BUILD: error: MAKE failed to generate output binaries! Check output for warnings that may need attention!
		)
	)
) ELSE (
	rm -f *.tsf build_msp430.log BuildLog.htm dependencies.in *.a43.revec
)