<?php

/* DynDNS config details */
$maxdomainsperuser = 5;


$dbdbusername = "nat3d";
$dbdbpassword = "passvarthai";
$dbdbdatabase = "nat3d";

// Users table
$tableusers  = 'users';
$colid       = 'id';
$colusername = 'username';
$colpassword = 'password';
$colsecret   = 'secret';

// DNS Table
$tabledns = 'dns';
$colexternal = 'externalip';
$colinternal = 'internalip';
$coldomname  = 'name';

// Cookie details
$secretlength = 10;
$cookieusername = "username";
$cookiesecret   = "notsosecret";
$cookieid       = "id";

$defaultredirect = 'dyndns.php';

// DNS server configuation
$dnsserver = "localhost";
$dnszone   = "nattt";
$nat3dport  = 100;
$gen_nat3_pl = 'perl /scratch/gen_nat3.pl';
$nsupdate = "nsupdate";
?>
