
use XML::Simple;
use Data::Dumper;
use Cwd;


my $file = 'tri86.vcproj';
my $xs1 = XML::Simple->new();
my $doc = $xs1->XMLin( $file, forcearray => 1, keyattr => ['Name'] );

# Get the project name
$name = $doc->{Name};

# Get the current directory as the project directory
$dir = cwd();

#Only increment build number if the project is being built - Ignore cleans..
if ( $#ARGV + 1 < 1 )
{
	#Increment the build number first
	$buildnumFile=".\\include\\buildnum.h";
	open(BNUM, $buildnumFile) || die("Could not open buildnum.h");
	@buildNumRaw=<BNUM>; 
	close(BNUM);

	chomp($buildNumRaw[0]);
	($temp1,$temp2,$buildNum)=split(/ /,$buildNumRaw[0]);

	open(BNUM, ">.\\include\\buildnum.h");
	print BNUM $temp1, " ", $temp2, " ", (int($buildNum) + 1), "\n";
	close(BNUM);
}

# Get the output file name
$outputFile = $doc->{Configurations}[0]->{Configuration}->{"Debug\|Win32"}->{Tool}->{VCNMakeTool}->{Output};

$srcStr = "";

# Get the source file names
@srcs = ();
foreach my $val (@{$doc->{Files}[0]->{Filter}->{Source}->{File}})
{
	$fn = $val->{RelativePath};
	$fn =~ s/\\/\//g;
	$fn =~ s/\.c/\.o/g;
	push( @srcs, $fn );
	$srcStr = $srcStr . "$fn ";
}

# Create makefile
open( MAKEFILE, ">makefile"); #open for write, overwrite

# *** Print freeform text, semicol required ***
print MAKEFILE <<"EndLabel";
# makefile configuration
NAME_OLD        = $name\_oldBL
NAME			= $name
OBJECTS         = $srcStr
CPU             = msp430x247

ASFLAGS         = -mmcu=\$(CPU) -x assembler-with-cpp -D_GNU_ASSEMBLER_ -c
CFLAGS          = -mmcu=\$(CPU) -O2 -Wall -Wunused -I.\/include
LDFLAGS  		= -mmcu=\$(CPU) -Wl,-Map=\$*.map

#switch the compiler (for the internal make rules)
CC              = msp430-gcc
AS              = msp430-gcc

.PHONY: all

#all should be the first target. it's built when make is run without args
all: \$(NAME_OLD).a43 \$(NAME_OLD).lst \$(NAME).a43 \$(NAME).lst

#Rules to generate files
%.elf: \$(OBJECTS)
	\$(CC) \$(LDFLAGS) -T \$*.x -o \$@ \$(OBJECTS)
	\@echo ======================================================
	msp430-size \$@
	\@echo ======================================================

%.a43: %.elf
	msp430-objcopy -O ihex \$^ \$@

%.lst: %.elf
	msp430-objdump -dSt \$^ >\$@

clean:
	rm -f *.elf *.a43 *.lst *.map dependencies.in \$(OBJECTS)

#automatic collection of dependencies in the source files.
#it's only updated the first time, after that it must be done maually
#with "make depend"
#the dependecies are included from a separate file:
ifneq (\$(MAKECMDGOALS), clean)
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
	\$(CC) -MM \$(CFLAGS) \$(patsubst %.o,%.c,\$(OBJECTS)) >\$@

EndLabel


# *** Close the file ***
close( MAKEFILE );