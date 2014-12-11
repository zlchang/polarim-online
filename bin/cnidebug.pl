#! /usr/bin/perl
use Tk;
use English;
$| = 1;

$t = $ARGV[0];
$t =~ s/-t//;

print "Start - run for $t seconds.\n";

for ($i=0; $i<$t; $i++) {
    sleep (1);
    print $i." \n";
}

print ">>>  Done.\n";
