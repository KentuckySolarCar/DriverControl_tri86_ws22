# DriverControl_tri86_ws22
This is the repo for the EV Driver controls box that was ordered along with the Tritium Wavesculptor Motor Controller.

Expected DCB wiring:

	ACCEL M12 Connector:
	Blue  - (2) Analog A GND
	Brown - (1) Analog A 5V+
	Black - (20) Analog A Signal
	White - N/C

	REGEN M12 Connector:
	Blue  - (5) Analog C GND
	Brown - (23) Analog C 5V+
	Black - (4) Analog C Signal
	White - N/C

	FWD/REV M12 Connector:
	Blue  - (30) Gear Select GND
	Brown - (7)  Brake Switch (Active 12V)
	Black - (10) Gear Switch 4 (SW_MODE_R Reverse) (Active Low)
	White - (9)  Gear Switch 2 (SW_MODE_B Cruise Start) (Active Low)




=====================================================================================================================================================
To set up your environment:
	1) Follow the directions in the ;readme.txt
			Use Windows. You will need the linux tools ported to windows (MinGW)
			You will need the msp430 toolchain installed (note the particular version) and patched with ;mspgcc_postInstall.exe
			Yuu will also need Perl installed
To build:
	1) open up cmd in this directory and run build_msp430.bat
	
To program the driver control box:
	1) make sure DCB is powered and the CAN/ethernet bridge is connected to your computer
	2) run triFwLoad_1_06.exe (it is included in this directory for convenience)
	3) locate tr86_oldBL.tsf in this directory (the result of the build step)
	4) Press erase/program. It may take a couple of tries for it to work
		I have noticed that the DCB does not want to be discoverable over CAN unless it is powered off of a well-regulated
		12V bus with not much interference. I couldn't get it to program on the car. I had to take the box upstairs to the bench in order to program it...
		
	Note: If programming doesn't work, then sometimes it works to go to "File->Manual Target Device"
	
	The serial number for our DCB is 4294967295
	and the default CAN base address is 500
=====================================================================================================================================================

	
	
Changes made to firmware on 12-9-15:
	tri86.c -- simplified state machine to only include OFF,REVERSE,DRIVE,and CRUISE. Implements cruise control. 
	pedal.c -- changed state machine to implement cruise control functionality
	wiring: changed fwd/rev input to signal the reverse gear instead of signal to drive gear. Changed wiring for cruise control
	
Changes made to firmware on 12-8-15:
	tri86.h -- defined REGEN_ON_BRAKE to enable regen to work
	tri86.c -- commented out all states in the state machine related to the 
				ignition switch and the fuel door. Also changed neutral so
				that it immediately changes to reverse. The idea is that when drive is not
				asserted it will be in reverse, thus operating like the old DCB did.
	pedal.c -- changed regen to always work instead of just when brakes are on, because we currently don't
				send a brake signal to the DCB
	
	

