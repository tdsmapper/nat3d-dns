<?php

// Allow only one connection to the database
if ($GLOBALS['DBOPEN']!= 1)
{
   include('config.php');
   mysql_connect(localhost, $dbdbusername, $dbdbpassword) or die('db login fail!');
   @mysql_select_db($dbdbdatabase) or die( "Unable to select database");
   $DBOPEN  = 1;
}
?>
