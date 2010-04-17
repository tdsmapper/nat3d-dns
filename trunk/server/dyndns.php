<?php
session_start();
include('database.php');
include('functions.php');
include('checklogin.php');

$username = $_SESSION[$cookieusername];
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
<h2>Welcome to the NAT3 DynDNS management page</h2>
<?php echo "<p style=\"margin-bottom:20px;\">Welcome $username! Please find a list of your domains below.</p>";?>

<?php
include('listdomains.php');
?>
<a href="logout.php">Logout</a>
</div>
</body>
</html>
<?php
include('closedatabase.php');
?>
