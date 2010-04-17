#!/usr/bin/perl

use strict;
use Socket;

die "Usage: $0 <name> <zone> <external> <port> <internal>\n" unless @ARGV >= 5;

my ($name, $zone, $ext, $port, $int) = @ARGV;

my $num = type_number();

my $e = encode_ip($ext);
my $i = encode_ip($int);
my $p = encode_port($port);

print join (' ', $name.".".$zone.".", '60', "TYPE$num", '\# 10', $e, $p, $i);

sub type_number {
   return 65324;
 }

sub encode_ip {
    my $ip = inet_aton($_[0]) or die "$_[0] is not a valid IP address\n";
    return join ' ', (unpack 'H2 H2 H2 H2', $ip);
}

sub encode_port {
    return join ' ', (unpack 'H2 H2', (pack 'n', $_[0]));
}
