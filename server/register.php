<?php
session_start();
include('functions.php');
include('config.php');
$username = $_POST['Username'];
$password = $_POST['Password'];

/* Check for corrupt cookies or stuff */
if (isset($_SESSION[$cookiesecret]) || isset($_SESSION[$cookieusername]))
{
include('database.php');
   if (is_logged_in())
   {
      include('closedatabase.php');
      header("Location:$defaultredirect"); 
      exit();
   }
   else
   {
      include('closedatabase.php');
      header("Location:logout.php");
      exit();
   }
}
$usercreated = false;
/* Do we already have such a user? */
if (strlen($username) > 0)
{
   include('database.php');
   $query = "SELECT * FROM `$tableusers` WHERE `$colusername`='$username';";
   $result = mysql_query($query);
   if ($result)
   {
      $nusers = mysql_numrows($result);
      if ($nusers >= 1)
      {
         $loginerror = "Username already taken!";
      }
      else
      {
         $password2 = $_POST['Password2'];
         if (strcmp($password2, $password) == 0)
         {
            $query = "INSERT INTO `$tableusers`(`$colusername`, `$colpassword`) VALUES('$username', '$password');";
            $result = mysql_query($query);
            if ($result)
            {
               $loginerror = "User created. Please click <a href=\"index.php\"> here to go to the login page</a><br />";
               $usercreated = true;
            }
         }
         else
         {
            $loginerror = "Passwords do not match!\n";
         }
      }
   }
   include('closedatabase.php');
}
   
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>NAT3 DynDNS Service </title>
<link rel="stylesheet" type="text/css" href="DocumentStyle.css">
<link rel="stylesheet" type="text/css" href="forms.css">
</head>
<body>
<div class="page">
<h2>Welcome to the NAT3 DynDNS service </h2>
<?php echo $loginerror; ?>
<?php 
if (!$usercreated)
{
   ?>
Please log in below:<br /><br /> 
<form id="loginform" method="post" action="">
       <table class="outer">
       <tr>
         <td class="lc">
            Username:
         </div>    
         <td class="rc">
            <input type="text" name="Username" style="width:99%; font-size:12; color:#550000">
         </td>
       </tr>

       <tr>
         <td class="lc">
            Password:
         </td>    
         <td class="rc">
            <input type="password" name="Password" style="width:99%; font-size:12; color:#550000">
         </td>
       </tr>

        <tr>
         <td class="lc">
            Re-enter Password:
         </td>    
         <td class="rc">
            <input type="password" name="Password2" style="width:99%; font-size:12; color:#550000">
         </td>
       </tr>

       </table>
<div style="text-align:center">
<input type="submit" value="Log in" name="submitbutton" class="center">
</div>
</form>
<?php
} // end if not created user
?>
<?php
?>
</div>
</body>
</html>
