#! /usr/bin/perl
use Tk;
use English;
use Time::Local;
use Tk::FileSelect;

$| =1;

# $DEBUG=0 for running, =1 for testing
$DEBUG = 0;

# Specify "d" or "debug" on command line for debug mode...
$_ = substr($ARGV[0],0,1);
if ($_ eq "-") {        # in case someone uses "-d(D)"
	$_ = substr($ARGV[0],1,1);
}
s/d/D/i;
if ($_ eq "D") {
	$DEBUG = 1;
}

# 400 = 2006, 2007
# 500 = 2008
# 600 = 2009
# 700 = 2010, 2011
# 800 = 2012
# 900 = 2013
# We will change file name scheme to - "j"<fillnumber>.<run in the fill> (2015 Igor).
$bindir  = "/home/polarim/bin/";
$inidir  = "/home/polarim/config/";
$datadir = "/home/polarim/hjet/2015/data/";
$rootdir = "/home/polarim/hjet/2015/root/";;
$CheckPhysicsDelta = 600000;    # Check for "Physics Running" every xxx-ms

# Get today's date, we need it for the file name & run number... - not any more, but 
#($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time);
#$nextday = sprintf("x%02d%02d",$mday,$mon+1);

$fillNumberM = GetRhicInfo("ringSpec.yellow", "fillNumberM", 0);

# Reading from run number file
if (open(FNAM, ".runname")) {
  	$runname = <FNAM>;        # j12345.123 
  	close(FNAM);
  	chomp($runname);      # remove the return (\n) character
  	$runheader = substr($runname, 0, index($runname, "."));
  	$runnumber = substr($runname, index($runname, ".") + 1);
	
# Has the fill changed since the last time this was started?
   	checkFill();
} else {
  	open(FNAM, ">.runname") || die "Could not write .runname file.";
  	$runheader = "j".$fillNumberM;
  	$runnumber = 1;
  	$runname = sprintf("%s.%3.3d", $runheader, $runnumber);
  	printf FNAM "$runname\n";
  	close(FNAM);
}

# Read the last Initialization file that was used...
if (open(FINI, ".jetini")) {
  	$inifile = <FINI>;
  	chomp($inifile);
  	close(FINI);
} else {
  	$inifile = $inidir."jet24.ini";
  	open(FINI, ">.jetini") || die "Could not write .jetini file.";
  	printf FINI "$inifile\n";
  	close(FINI);
}

$info    = "not started";
$running = 0;
$runtime = 7200;
$comment = "Enter comments here";

#----------------------------------------
# Main Window 
#----------------------------------------
my $mw=MainWindow->new;
$mw->title("JET DAQ RUN");

$frame=$mw->Frame()->pack(-side=>'top', -fill=>'x');
$frame1=$mw->Frame()->pack(-side=>'top', -fill=>'x');
$frame2=$mw->Frame()->pack(-side=>'top', -fill=>'x');

#----------------------------------------
# RUN FILE HEADER
#----------------------------------------
#($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime(time) ;
#$runheader = sprintf("x%02d%02d",$mday,$mon+1);
$frame->Label(-text=>"RunHEADER:")->pack(-side=>'left', -anchor=>'w');
$frame->Entry(-justify=>'right', -textvariable=>\$runheader, -font=>'r16', -width=>7)->pack(-side=>'left', -anchor=>'w');

#----------------------------------------
# RUN NUMBER
#----------------------------------------
$frame->Label(-text=>"RunNumber: ")->pack(-side=>'left',-anchor=>'w');
$frame->Entry(-justify=>'right',-textvariable=>\$runnumber, -font=>'r16', -width=>4)->pack(-side=>'left', -anchor=>'w');

#----------------------------------------
# MEASUREMENT TIME
#----------------------------------------
$frame->Label(-text=>"  DAQ time: ")->pack(-side=>'left',-anchor=>'w');
$frame->Entry(-justify=>'right', -textvariable=>\$runtime,-font=>'r16', -width=>7)->pack(-side=>'left', -anchor=>'w');
# Time unit option menu...
$frame->Optionmenu(-options => ["SEC", "MIN", "HRS"], -variable =>\$tvar, -command => \&tunit)->pack(-side=>'left', -padx=>1, -anchor=>'w');

#----------------------------------------
# START BUTTON
#----------------------------------------
$start_b = $frame ->Button(-text => "Start Run", -activebackground=>'gray60', -command=>\&popup_start)->pack(-side=>'left', -anchor=>'w');
$start_b->configure(-foreground=>'ForestGreen');

#----------------------------------------
# STOP BUTTON
#----------------------------------------
$stop_b = $frame->Button(-text => "Stop Run", -activebackground=>'gray60', -command=>\&popup_stop)->pack(-side=>'left', -anchor=>'w');

#$stop_b->configure(-state=>"disabled");

#----------------------------------------
# Auto Cycle Check Button
#----------------------------------------
$frame->Checkbutton(-text=>" Cycle ", -variable=>\$autorestart, -indicatoron=>0,-pady=>5, -selectcolor=>"LightGreen")->pack(-side=>'left', -anchor=>'w');

#----------------------------------------
# EXIT BUTTON
#----------------------------------------
$frame->Button(-text => "exit", -activebackground=>'gray60', -command=>\&popup_exit)->pack(-side=>'right', -anchor=>'w');

#------------------------------------------
#  Text Message
#------------------------------------------
my $text = $mw->Scrolled("Text", -width=>80, -height=>20, -font=>'r16')->pack(-fill=>'both', -expand=>1);

#----------------------------------------
# Specify initialization file
#----------------------------------------
$frame1->Label(-text=>"Config file: ")->pack(-side=>'left', -anchor=>'w');
$frame1->Entry(-textvariable=>\$inifile, -font=>'r16', -background=>'gray80', -width=> 60)->pack(-side=>'left', -pady=>2, -anchor=>'w');
$frame1->Button(-text => "Browse", -activebackground=>'gray60', -command=>\&popup_file_browser)->pack(-side=>'left', -padx=>2, -anchor=>'w');

#----------------------------------------
# Comment Message
#----------------------------------------
$frame2->Label(-text=>"Comment:  ")->pack(-side=>'left', -pady=>1, -anchor=>'w');
$frame2->Entry(-textvariable=>\$comment, -font=>'r16', -background=>'gray80', -width=>60)->pack(-side=>'left', -pady=>2, -anchor=>'w'); #, -expand=>1);

#----------------------------------------------------------
# Enable CheckPhysicsRunning Auto Sart/Stop Check Button
#----------------------------------------------------------
$frame2->Checkbutton(-text=>" Auto Start/Stop ", -variable=>\$ALPHA, -indicatoron=>0, -pady=>5, -selectcolor=>"LightGreen")->pack(-side=>'left', -padx=>2, -anchor=>'w');

#----------------------------------------
# message bar
#----------------------------------------
$mw->Label(-textvariable=>\$info, -relief=>'ridge')->pack(-side=>'bottom', -fill=>'x');

# Show warning if DEBUG mode is on...
if ($DEBUG == 1) {
	$text->insert('end', " **WARNING: DEBUG mode is on.**\n");
  	$mw->idletasks;
}

$autorestart = 0;
$ALPHA = 0;

# Schedule first check for Physics Running...
$PhysRunId = $mw->after($CheckPhysicsDelta, \&CheckPhysicsRunning);

MainLoop;

##=========================================================================
##=========================================================================
##=========================================================================

# startrun
sub startrun {
  	$command = "./rhicpol -J -P -e0 -Thjet";
  	$datafile = "/home/hjet/2015/data/$Runn.data";
  	$logfile = "/home/hjet/2015/log/$Runn.log";
  	$ENV{'HJET_ROOTFILE'} = "/home/hjet/2015/root/$Runn.root";
  	$ENV{'HJET_DATAFILE'} = $datafile;
  	$ENV{'HJET_DAQ'} = $Runn;

  	if (-e $datafile) {
		warning_file();
    	# Enable the Start Run button...
    		$start_b->configure(-state=>"normal");
  	} else {
    		if (-e $logfile) {
      			$commandline = "mv $logfile $logfile".".old";
      			system "$commandline";
		}
    	}
#
# Get some information about the beam from CDEV
#
    	#   Fill number (could also use ringSpec.blue)
    	$fillNumberM = GetRhicInfo("ringSpec.yellow", "fillNumberM", 0);
    	$text->insert('end',"Fill number = $fillNumberM\n");
    	$mw->idletasks;

    #   Beam energy (could also use ringSpec.blue)
    	$beamEnergyM = GetRhicInfo("ringSpec.yellow", "beamEnergyM", 0);
    	$str = sprintf("%6.2f", $beamEnergyM);
    	$text->insert('end',"Beam energy = $str GeV/c\n");
    	$mw->idletasks;

    #   Beam intensity
#    $wcmBeamMy = GetRhicInfo("yi2-wcm3", "wcmBeamM", 0);
#    $wcmBeamMb = GetRhicInfo("bo2-wcm3", "wcmBeamM", 0);
    	$wcmBeamMy = GetRhicInfo("wcm.yellow", "wcmBeamM", 0);
    	$wcmBeamMb = GetRhicInfo("wcm.blue", "wcmBeamM", 0);
    	$text->insert('end',"Yellow ions = $wcmBeamMy\n");
    	$text->insert('end',"Blue ions = $wcmBeamMb\n");
    	$mw->idletasks;

    #   Jet base position
    	$absolutePosition = GetRhicInfo("stepper.12a-hjet.A.U", "absolutePosition", 0);
    	$text->insert('end',"Jet base position = $absolutePosition\n");
    	$mw->idletasks;
    
    #   Beam polarization
    	$polarizationMb = GetRhicInfo("polarimeter.blu", "polarizationM", 0);
    	$polarizationMy = GetRhicInfo("polarimeter.yel", "polarizationM", 0);
    	$text->insert('end',"Polarization BLUE   = $polarizationMb\n");
    	$text->insert('end',"Polarization YELLOW = $polarizationMy\n");
    	$mw->idletasks;
    
    #   Beam position at IP 12
    	$bxpos = BeamPosition("bx");
    	$bypos = BeamPosition("by");
    	$yxpos = BeamPosition("yx");
    	$yypos = BeamPosition("yy");
    	$text->insert('end',"Beam position BLUE   = $bxpos (x) $bypos (y)\n");
    	$text->insert('end',"Beam position YELLOW = $yxpos (x) $yypos (y)\n\n");
    	$mw->idletasks;
    
# start program
	$running = 1;
    	$rtime = $runtime * $timeunits;
# Removed -T $tshift because had no efect
    	$commandline = "cd $bindir; $command -i $inifile -l $logfile -f $datafile -t $rtime -c \"$comment\" &";

    	if ($DEBUG == 0) {
      		$text->insert('end',"$commandline\n");
      		$mw->idletasks;
	  	system "$commandline";
      		sleep 2;
      		$pid_rpol = substr(`ps ax | grep rhicpol | grep $datafile`, 0, 5);
      		sleep 5;
      		$pid_tail = open(MESS, "tail -f -q $logfile |");
      		system "xterm -iconic -e root -l jetrun.C &";
      		sleep 2;
      		$pid_root = substr(`ps ax | grep xterm | grep jetrun`, 0, 5);
    	} else {
      		$text->insert('end',"DEBUG: $commandline\n");
      		$mw->idletasks;
      		system "./cnidebug.pl -t$rtime > .daqmessage &";
      		sleep 1;
      		$pid_tail = open(MESS,"tail -f -q .daqmessage |");
    	}

# Set up message file callback...
    	$mw->fileevent(MESS,'readable', [\&insert_text]);
	$starttimeI = time();
	$starttime = localtime($starttimeI);

# Always dis-enable Auto Restart and enable the "Stop Run" button...
    	$autorestart = 0;
	$autorestart_chk = 0;
    	$stop_b->configure(-state=>"normal");
    	$stop_b->configure(-foreground=>'Red');
    	$mw->idletasks;

	TimeInfoMessage();
	RunWatchDog();
}

sub insert_text {
  	my $curline;
  	if ($curline = <MESS>) {
		$text->insert('end',$curline);
		$text->yview('moveto',1.0);
    		if ($curline =~ m/RHICPOL-FATAL/) {
      			$autoresrart = 0;
      			$text->insert('end',"THIS IS BAD - Find out what is wrong before restarting!\nIt\'s probably time to call an Expert!\n");
      			$text->yview('moveto', 1.0);
    		}
  	}
}

# stoprun 
sub stoprun {
  	if ($running != 0) {
# kill the program
    		$info = "Stopping run ".$Runn.". Please wait...";
    		$text->insert('end',"Stopping run. Please wait..\n");
    		$text->yview('moveto',1.0);
    		$mw->idletasks;

    		if ($DEBUG == 0) {
      			if (`ps -p $pid_rpol --noheader` > 0) {
        			system "kill -s SIGINT $pid_rpol";
        			sleep 5;
	  		}
      			if (`ps -p $pid_root --noheader` > 0) {
        			system "kill -s SIGINT $pid_root";
        			sleep 2;
      			}
		} else {
      			sleep 2;
    		}
	}
	  
# But always do this...
# The order here is IMPORTANT, and necessary.
  	$mw->fileevent(MESS, 'readable',"");   # First, cancel the callback
  	if ($DEBUG == 0) {
    		if (`ps -p $pid_tail --noheader` > 0) {
      			system "kill -s SIGINT $pid_tail";       # Second, kill the tail process
    		}
  	}
  	close(MESS);                          # Third, close the "MESS" file

# Make sure rhicpol has stopped. Check a few times, then give up...
  	if ($DEBUG == 0) {
    		$n = 0;
    		while ((`ps -p $pid_rpol --noheader` > 0) && ($n < 6)) {
      			$text->insert('end',"**WARNING: Failed to kill rhicpol. Waiting 5 seconds...\n");
      			$text->yview('moveto',1.0);
      			$mw->idletasks;
      			sleep 5;
      			$n++;
    		}
  	}

  	$tdiff = time() - $starttimeI;
  	$info = "Stopped...  Run Time was ".$tdiff." sec.";
  	$text->insert('end',"Stopped. Run Time was ".$tdiff." sec.\n\n");
  	$text->yview('moveto',1.0);

  	$running = 0;

  	summary();

  	$start_b->configure(-state=>"normal");
  	$start_b->configure(-foreground=>'ForestGreen');
	$mw->idletasks;

# Update run number and write the number in file...
  	$runnumber += 1;
  	open(FNAM, ">.runname") || die "Could not write to .runname file.";
  	$runname = sprintf("%s.%3.3d", $runheader, $runnumber);
  	printf FNAM "$runname\n";
  	close(FNAM);
	    
# Update RUN FILE HEADER, if next fill
  	checkFill();
  	$mw->idletasks;

  	if ($autorestart == 1) {
    		$text->insert('end', "Auto Restart next run.\n");
    		$text->yview('moveto',1.0);
    		$mw->idletasks;

    		sleep 1;
    		if ($ALPHA == 1) {
     			CheckPhysicsRunning();
    		} else {
     			popup_start();
    		}
  	}
}

sub warning_file() {
  	$w1=$mw->Toplevel();
  	$w1->title("WARNING");
  	$w1->Label(-text=>"!! WARNING !!")->pack(-side=>'top');
  	$w1->Label(-text=>"FILE EXISTS")->pack(-side=>'top');
  	$w1->Label(-text=>"Change the run number and try again.")->pack(-side=>'top');
  	$w1->Button(-text=>"Close",-command=>sub{$w1->withdraw})->pack;
}

sub popup_start() {
# First, disable the Start Run button...
  	$start_b->configure(-state=>"disabled");

# Update RUN FILE HEADER, if next fill...
  	checkFill();
  	$Runn = sprintf("%s.%03d", $runheader, $runnumber);

  	$text->delete("1.0",'end');
  	$info = "Starting run. Please wait...";
  	$text->insert('end', "Starting run. Please wait...\n");
  	$mw->idletasks;

  	startrun();
}

sub popup_stop() {
  	if ($running != 0) {
    		$wstop=$mw->Toplevel();
    		$wstop->geometry("300x80+200+200");
    		$wstop->title("STOP RUN -- ARE YOU SURE?");
    		$wstop->Label(-text=>" Do you want to stop this run now ? ")->pack(-side=>'top', -expand=>1, -fill=>none);
    		$wstop->Button(-text=>"YES", -command=>sub{$wstop->withdraw; $autorestart=0; &stoprun();})->pack(-side=>'top', -expand=>1, -fill=>none);
    		$wstop->Button(-text=>"NO (continue measurement)", -command=>sub{$wstop->withdraw})->pack(-side=>'left', -expand=>1, -fill=>none);
  	}
}

sub popup_exit() {
  	if ($running != 0) {
    		$wstop=$mw->Toplevel();
    		$wstop->geometry("300x80+200+200");
    		$wstop->title("STOP RUN and EXIT -- ARE YOU SURE?");
    		$wstop->Label(-text=>" Do you want to stop this run now ? ")->pack(-side=>'top', -expand=>1, -fill=>none);
    		$wstop->Button(-text=>"YES (Stop and Exit Now)", -command=>sub{$wstop->withdraw; $autorestart=0; &stoprun(); exit;})->pack(-side=>'top', -expand=>1, -fill=>none);
    		$wstop->Button(-text=>"NO (Continue Measurement)", -command=>sub{$wstop->withdraw})->pack(-side=>'left',-expand=>1, -fill=>none);
  	} else {
    		exit;
  	}
}

sub popup_file_browser() {
  	$afile = $mw->getOpenFile(-defaultextension=>".ini", -filetypes=>[['INI Files','.ini'], ['Text Files',['.txt', '.text']], ['All Files','*']],
     		-initialdir=>$inidir, -initialfile=>$inifile, -title=>"Select Configuration File");
  	if ($afile) {
    		$inifile = $afile;
    		if (open(FINI,">.jetini")) {
      			printf FINI "$inifile\n";
      			close(FINI);
    		}
  	}
  	$mw->idletasks;
}

sub CheckPhysicsRunning() {
# Check to see if physics running is on... 
	$PhysRunId->cancel;
 	$sta = `cdevCommand ringSpec.yellow get ringStateS`;
 	$sln = length($sta) - 1;
 	if ($sln > 15) { $sln = 15; }
 	$ystate = substr($sta, 0, $sln);

 	$sta = `cdevCommand ringSpec.blue get ringStateS`;
 	$sln = length($sta) - 1;
 	if ($sln > 15) { $sln = 15; }
 	$bstate = substr($sta, 0, $sln);

 	$info = "RHIC state check : Yellow = ".$ystate."   Blue = ".$bstate;

# Schedule the next callback...
 	$PhysRunId = $mw->after($CheckPhysicsDelta, \&CheckPhysicsRunning);

 	if ($ALPHA == 1) {
  		if ($ystate eq "Physics Running" && $bstate eq "Physics Running") {    
# If already running, do nothing. If jet is not running, start a run...
   			if ($running == 0) {
    				popup_start();  
   			}
  		} else {
# Not in Physics Running state. If jet is running, stop the run...
   			if ($running == 1) {
    				stoprun();
    				$text->insert('end',"Physics Running is OFF - Stopping run\n");
    				$text->yview('moveto',1.0);
    				$mw->idletasks;
   			}
  		}
 	}
	checkFill();
}

sub TimeInfoMessage() {
  	if ($running == 1) {
		$tdiff = time() - $starttimeI;
		$info = "Running .... data ".$Runn."      Started : ".$starttime."   (".$tdiff." sec completed so far)";
    		if ($DEBUG != 0) {
      			if ($tdiff >= $rtime) {stoprun();}
    		}
    		$mw->after( 2000, \&TimeInfoMessage);
    		if ($tdiff >= 30 && $autorestart_chk == 0) {
      			if (`ps -p $pid_rpol --noheader` > 0) {
        			$autorestart = 1;
				$autorestart_chk = 1;
      			}
    		}
  	}
}

sub RunWatchDog() {
  	if ($running == 1) {
		if ($DEBUG == 0) {
      			$process = `ps -p $pid_rpol --noheader`;
      			if ($process <= 0) {
        			sleep 3;
        			stoprun();
      			} else {
        			$mw->after( 10000, \&RunWatchDog);
      			}
		} else {
# Debug mode...
      			$tdiff = time() - $starttimeI;
      			if ($tdiff >= $rtime+4) {
        			stoprun();
      			} else {
        			$mw->after( 10000, \&RunWatchDog);
      			}
		}
  	} # else go out
}

sub tunit() {
  	$timeunits = 1;
  	if ($tvar eq "SEC") {$timeunits = 1;}
  	if ($tvar eq "MIN") {$timeunits = 60;}
  	if ($tvar eq "HRS") {$timeunits = 3600;}
}

sub checkFill() {
	$fillNumberM = GetRhicInfo("ringSpec.yellow", "fillNumberM", 0);
	$newheader = "j".$fillNumberM;
  	if ($newheader ne $runheader) {
    		$runheader = $newheader;
    		$runnumber = 1;

# write the number in file
    		open(FNAM,">.runname") || die "Could not write to .runname file.";
    		$runname = sprintf("%s.%3.3d", $runheader, $runnumber);
    		printf FNAM "$runname\n";
    		close(FNAM);
  	}
}

#-----------------------------------------------------
# Make a file with a summary of information from the 
# log file. Do this at the end of each run.
#-----------------------------------------------------
sub summary() {
	if (! open(FLOG, $logfile)) {
    		$text->insert('end', "**WARNING - SUMMARY: Could not open $logfile.\n");
    		$mw->idletasks;
  	} else {
    		while ($mline = <FLOG>) {
      			if ($mline =~ m/Starting measurement/) {
        			$mon = substr($mline, 10, 3);
        			$day = substr($mline, 14, 2);
        			$hr  = substr($mline, 17, 2);
        			$min = substr($mline, 20, 2);
        			$sec = substr($mline, 23, 2);
        			$yr  = substr($mline, 26, 4);
        			$starttime = timelocal($sec, $min, $hr, $day, $mon, $yr);
        			$startstr = $hr.":".$min;
      			} elsif ($mline =~ m/Measurement finished/) {
        			$mon = substr($mline, 8,  3);
        			$day = substr($mline, 12, 2);
			        $hr  = substr($mline, 15, 2);
			        $min = substr($mline, 18, 2);
			        $sec = substr($mline, 21, 2);
			        $yr  = substr($mline, 24, 4);
			        $stoptime = timelocal($sec, $min, $hr, $day, $mon, $yr);
      			}
    		}
    		$eltime = $stoptime - $starttime;

# Get information from the HJET analysis
    		if (-e ".$Runn") {
      			if (! open(FASYM, ".$Runn")) {
        			$text->insert('end', "**WARNING - ASYM: Could not open Jet Asymmetry file.\n");
        			$mw->idletasks;
      			} else {
        			$asymline = <FASYM>;
        			chomp($asymline);
        			close(FASYM);
        			system "rm -f .$Runn &";
      			}
    		}
  
    		if (! open(FSUM, ">>.summary")) {
      			$text->insert('end', "**WARNING - SUMMARY: Could not open summary file.\n");
      			$mw->idletasks;
    		} else {
      			$mline = sprintf("%9s %5s %5d %5d %9.4e %9.4e %5.1f %5.1f %5.1f %5.1f %6.2f %5.1f %5.1f %7d %s\n",
				$runname, $startstr, $fillNumberM, $eltime, $wcmBeamMy, $wcmBeamMb, $yxpos, $yypos, $bxpos, 
				$bypos, $beamEnergyM, $polarizationMy, $polarizationMb, $absolutePosition, $asymline);
      			printf FSUM $mline;
    		}
    		close(FSUM);
  	}
}

sub BeamPosition()
#
# Calculate the beam position at IP 12 using the BPMs
# bx => blue x-position; by => blue y-position
# yx => yellow x-position; yy => yellow y-position
#
{
 	if ($_[0] =~ /b[xy]/) {
  		$d1 = "rbpm.b-g11-b";
  		$d2 = "rbpm.b-g12-b";
 	} else {
  		$d1 = "rbpm.y-g11-b";
  		$d2 = "rbpm.y-g12-b";
 	}

 	if ($_[0] =~ /[by]x/) {
  		$d1 .= "hx";
  		$d2 .= "hx";
 	} else {
  		$d1 .= "vx";
  		$d2 .= "vx";
 	}
 
 	$pos1 = GetRhicInfo($d1, "avgOrbPositionM", 999.9);
 	if ($pos1 == 999.9) { return 999.9; }

 	$pos2 = GetRhicInfo($d2, "avgOrbPositionM", 999.9);
 	if ($pos2 == 999.9) { return 999.9; }

# calculate the position and return the value
 	return ($pos1 + $pos2)/2000;
}

sub GetRhicInfo()
#
# Send a CDEV request to get the specified numeric information.
# GetRhicInfo(CDEV_device _name, Parameter_name, Error_value)
# The Error_value specifies the number to be returned if there
# is a CDEV error or other non-numeric value from CDEV.
#
{
 	$CDEV_device_name = $_[0];
 	$Parameter_name = $_[1];
 	$Error_value = $_[2];

 	$scdv = `cdevCommand $CDEV_device_name get $Parameter_name`;
 	if (IsNumber($scdv)) {
  		return $scdv;
 	} else {  
		return $Error_value;
 	}
}

sub IsNumber()
{
#
# Look at each character. If a nondigit (\D) appears and is not +-Ee. then it
# cannot possibly be a number. In this case return false (0). Otherwise, check
# further is see if it is a number.
#
# Remove trailing CR & white space & leading white space
 	chomp $_[0];
 	$_[0] =~ s/^\s+//;
 	$_[0] =~ s/\s+$//;
 
 	$slen = length($_[0]);
 	$ndot = 0;     # number of "."s (<=1)
 	$nEe = 0;      # number of "E"s or "D"s (<=1)
 	$npm = 0;      # number of "+"s or "-"s (<=2)
 	for ($i=0; $i<$slen; $i++) {
  		$c = substr($_[0], $i, 1); 
  		if ($c =~ /\D/) { 
   			if ($c =~ /[EeDd+-.]/) {
    				if ($c =~ /[+-]/) {
     					$npm++;
     					if ($npm > 1 && $nEe == 0) { return 0; }
     					if ($npm > 2) { return 0; }
    				}
    				if ($c =~ /[EeDd]/) {
     					$nEe++;
     					if ($nEe > 1) { return 0; }
    				} 
    				if ($c eq ".") {
     					$ndot++;
     					if ($ndot > 1) { return 0; }
    				}
   			} else {
    				return 0;
   			}
  		}
 	}

 	$_[0] =~ /^([+-]?)(?=\d|\.\d)\d*(\.\d*)?([Ee]([+-]?\d+))?$/;
 	if ($_[0] != 0) {  
		return 1; 
	} else { 
  		if ($_[0] eq "0" or $_[0] eq "+0" or $_[0] eq "-0") { 
			return 1; 
		} else { 
			return 0; 
		}
 	}                            
}

