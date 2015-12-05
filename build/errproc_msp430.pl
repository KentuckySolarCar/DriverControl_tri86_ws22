$noErr = 1;
open( LOGFILE, "build_msp430.log" );
while ( <LOGFILE> )
{ 
	if ( $_ =~ m/(.+\.\w+):([0-9]+):( warning+):(.+)/  )
	{
		#This segment handles any build warnings -- So they dont popup as errors(make doesnt terminate on warning)
		print "$1($2): warning:$4\n";
	}
	elsif ( $_ =~ m/(.+\.\w+):([0-9]+):(.+)/  )
	{
		#This segment handles all build outputs from stderr that arent warnings
		
		#Some errors dont output with the word "error:" in them (eg: extra undef stuct members)
		#But dont want to print the word "error:" twice onto the output so check and add if necessary 
		print "$1($2): ";
		if ($3 =~ m/(error\s*:+)(.+)/)
		{
			#Comparison is true.. NB: $1,$2,$3,.... are NOW changed relative to this compare
			#manually inserting "error:" instead of using $1 to ensure conformity incase log has extra spaces (eg: "error :")
			print "error:$2\n";
		}
		else 
		{
			#Comparison is false.. NB: $1,$2,$3,.... remain as they were
			print "error:$3\n";
		}
		$noErr = 0;
	}
}
close( LOGFILE );
if ($noErr){ exit(0);}
else { exit(1);}
