<?php
include('config.php');
session_start();
if (isset($_SESSION[$cookieusername]))
{
   echo "Thank you for using NAT3D DNS Service! have a nice day!";
   $_SESSION = array();
   session_destroy();
}
else
{
   header("Location: index.php");
}
?>
