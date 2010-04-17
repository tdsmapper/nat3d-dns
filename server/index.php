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

/* Are the login details right? */
if (strlen($username) > 0)
{
   include('database.php');
   if (check_login($username, $password, true))
   {
      header("Location: $defaultredirect");
      exit();
   }
   else
   {
      $loginerror = "<p style=\"color:red\">Please check your login credentials!</p><br />";
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
       </table>
<div style="text-align:center">
<input type="submit" value="Log in" name="submitbutton" class="center">
</div>
</form>

New user? <a href="register.php">Register!</a> <br />
<?php
?>
</div>
</body>
</html>
