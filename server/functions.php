<?php

function generate_secret()
{
   include('config.php');
   $length = $secretlength;
   $code = md5(uniqid(rand(), true));
   if ($length != "") return substr($code, 0, $length);
   else return $code;

}

function is_logged_in()
{
   include('config.php');
   $username = $_SESSION[$cookieusername];
   $secret   = $_SESSION[$cookiesecret];
   $query = "SELECT `$colsecret` from `$tableusers` WHERE `$colusername`='$username';";
   $result = mysql_query($query);

   $nrows = mysql_numrows($result);
   if ($nrows > 0)
   {
      $dbsecret = mysql_result($result,0, $colsecret);

      if (strcmp($secret, $dbsecret) == 0)
      {
         return true;
      }
   }
   return false;

}

function check_login($username, $password, $createsecret)
{
   include('config.php');
   $query = "SELECT `$colpassword`, `$colid` from `$tableusers` WHERE `$colusername`='$username';";
   $result = mysql_query($query);

   $nrows = mysql_numrows($result);
   if ($nrows > 0)
   {
      $dbpassword = mysql_result($result,0, $colpassword);
      $id         = mysql_result($result,0, $colid);

      if (strcmp($password, $dbpassword) == 0)
      {
         if ($createsecret)
         {
            $secret = generate_secret();
            $query = "UPDATE `$tableusers` SET `$colsecret` = '$secret' WHERE `$colid`='$id';";
            $result = mysql_query($query);
            if ($result)
            {
               $_SESSION[$cookieusername] = $username;
               $_SESSION[$cookiesecret]   = $secret;
               $_SESSION[$cookieid]       = $id;
               return true;
            }
         }
         else
         {
            return true;
         }
      }
   }
   return false;
}

//function to validate ip address format in php by Roshan Bhattarai(http://roshanbh.com.np)
function validateIpAddress($ip_addr)
{
  //first of all the format of the ip address is matched
  if(preg_match("/^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/",$ip_addr))
  {
    //now all the intger values are separated
    $parts=explode(".",$ip_addr);
    //now we need to check each part can range from 0-255
    foreach($parts as $ip_parts)
    {
      if(intval($ip_parts)>255 || intval($ip_parts)<0)
      return false; //if number is not within range of 0-255
    }
    return true;
  }
  else
    return false; //if format of ip address doesn't matches
}

?>
