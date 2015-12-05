# makefile configuration
NAME_OLD        = tri86_oldBL
NAME			= tri86
OBJECTS         = ./src/can.o ./src/flash.o ./src/gauge.o ./src/pedal.o ./src/switch.o ./src/tri86.o ./src/usci.o ./src/variant.o 
CPU             = msp430x247

ASFLAGS         = -mmcu=$(CPU) -x assembler-with-cpp -D_GNU_ASSEMBLER_ -c
CFLAGS          = -mmcu=$(CPU) -O2 -Wall -Wunused -I./include
LDFLAGS  		= -mmcu=$(CPU) -Wl,-Map=$*.map

#switch the compiler (for the internal make rules)
CC              = msp430-gcc
AS              = msp430-gcc

.PHONY: all

#all should be the first target. it's built when make is run without args
all: $(NAME_OLD).a43 $(NAME_OLD).lst $(NAME).a43 $(NAME).lst

#Rules to generate files
%.elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -T $*.x -o $@ $(OBJECTS)
	@echo ======================================================
	msp430-size $@
	@echo ======================================================

%.a43: %.elf
	msp430-objcopy -O ihex $^ $@

%.lst: %.elf
	msp430-objdump -dSt $^ >$@

clean:
	rm -f *.elf *.a43 *.lst *.map dependencies.in $(OBJECTS)

#automatic collection of dependencies in the source files.
#it's only updated the first time, after that it must be done maually
#with "make depend"
#the dependecies are included from a separate file:
ifneq ($(MAKECMDGOALS), clean)
-include dependencies.in
endif
#target to update the file, it's removed first
depend: rmdepend dependencies.in
#remove the file
rmdepend:
	rm -f dependencies.in
#build the file that contains the dependencies. no deps in this rule.
#if there were deps it would be rebuilt every chnage, which is unneded:
dependencies.in:
	$(CC) -MM $(CFLAGS) $(patsubst %.o,%.c,$(OBJECTS)) >$@

