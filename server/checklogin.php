<?php
include('config.php');
if (!is_logged_in())
{
   header("Location:index.php");
}

?>
