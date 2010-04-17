<?php
include ('../database.php');
include ('../functions.php');
include ('../config.php');
include ('../dnscommands.php');
/* Our own implementation of a dyndns service */
$username = $_GET['username'];
$password = $_GET['password'];
$intip    = $_GET['intip'];
$domain   = $_GET['domain'];
$extip    = $_GET['extip'];
$port     = $_GET['port'];

$res = check_login($username, $password, false);
if ($res)
{
   $query = "SELECT `$colid` from `$tableusers` WHERE `$colusername`='$username';";
   $result = mysql_query($query);
   if ($result)
   {
      $id = mysql_result($result, 0, $colid);
      if (modifyDomain($id, $domain, $extip, $intip, $errmsg))
      {
         echo "WIN";
      }
      else
      {
         echo "DOMAINFAIL";
      }
   }
   else
   {
      echo "FAIL";
   }
}
else
{
   echo "LOGINFAIL";
}

include ('../closedatabase.php');
?>
