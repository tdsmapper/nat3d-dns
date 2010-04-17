<?php
session_start();
include('config.php');
include('checklogin.php');
include('dnscommands.php');

$username = $_SESSION[$cookieusername];
$id       = $_SESSION[$cookieid];
$errMsgs = array();
$general_error = "";
processPostData($id, $general_error, $errMsgs);
printForm($id, $errMsgs);
?>

<?php

/* See if this is a postback and accordingly process the form! */
function processPostData($id, &$general_error, &$errMsgs)
{
   include('config.php');
   if (isset($_POST['submit_dyndns']))
   {
      $post = $_POST;
      //print_r($post);
      $delcount = count($post['del']);
      $postcount = count($post['ext']);

      /* Process any deletions first */
      for ($i = 0; $i < $delcount; $i++)
      {
         $domname = $post['del'][$i];
         if (isset($domname))
         {
            deleteDomain($domname, $errMsgs);
         }
      }

      /* Process any modifications */
      for ($i = 0; $i < $postcount; $i++)
      {
         $name = $post['name'][$i];
         $ext = $post['ext'][$i];
         $int= $post['int'][$i];
         if (isChanged($name, $ext, $int, $errMsgs))
         {
            modifyDomain($id, $name, $ext, $int, $errMsgs);
         }
      }

      /* Do we have any new domains added? */
      if (strlen($_POST['newname']) > 0)
      {
         $query = "SELECT * FROM `$tabledns` where `$colid`='$id';";
         $result = mysql_query($query);
         $numdoms = mysql_numrows($result);
         if ($numdoms < $maxdomainsperuser)
         {
            $name = $_POST['newname'];
            $ext  = $_POST['newext'];
            $int  = $_POST['newint'];
            if (!addDomain($id, $name, $ext, $int, $errMsgs))
            {
               $general_error = $errMsgs[$name];
            }
         }
         else
         {
            $general_error = "You have exceeded maximum domains per user!";
         }
      }
   }
}

function isChanged($name, $ext, $int, &$errMsgs)
{
   include('config.php');
   $query = "SELECT * FROM `$tabledns` where `$coldomname`='$name';";
   $result = mysql_query($query);
   $nrows = mysql_numrows($result);
   if ($result && ($nrows > 0))
   {
      $dbext = mysql_result($result, 0, $colexternal);
      $dbint = mysql_result($result, 0, $colinternal);
      if ((strcmp($dbext, $ext) == 0) &&
            (strcmp($dbint, $int) == 0))
      {
         return false;
      }
      else
      {
         return true;
      }
   }
   $errMsgs[$name] = "No such domain!";
   return false;
}



/* Print the domains form for $id user */
function printForm($id, $errMsgs)
{
   ?>
   <form method="post" action="" id="myform">
   <table>
   <?php
   include('config.php');
   $query = "SELECT * FROM `$tabledns` WHERE `$colid` = '$id';";
   $numdomains = 0;
   $result = mysql_query($query);
   /* Print the table and rows of domains owned */
   if ($result)
   {
      printTableHeader();
      $numdomains = mysql_numrows($result);
      for ($i = 0; $i < $numdomains; $i++)
      {
         $domname = mysql_result($result, $i, $coldomname);
         $external = mysql_result($result, $i, $colexternal);
         $internal = mysql_result($result, $i, $colinternal);
         $error = $errMsgs[$domname];
         printRow($domname, $internal, $external, $error, $i);
      }
   }

   if ($numdomains < $maxdomainsperuser)
   {
      printNewRow();
   }
   ?>
   </table>
   <div style="text-align: center;">
   <input type="submit" name="submit_dyndns" value="Update"/>
   </div>
   </form>
   <?
}
echo $general_error;
/* assumes results start at 0! */
function printRow($domname, $internal, $external, $error, $i)
{
   include('config.php');

   echo "<tr>\n";
   echo "<td class=\"generic\" >\n";
   echo "<input type=\"checkbox\" name=\"del[]\" value=\"$domname\"\" />\n";
   echo "</td>\n";

   echo "<td class=\"generic\" >\n";
   echo "<input type=\"hidden\" name=\"name[]\" value=\"";
   echo $domname;
   echo "\" />\n";
   echo $domname.".".$dnszone;
   echo "</td>\n";
   
   echo "<td class=\"generic\" >\n";
   echo "<input type=\"text\" name=\"ext[]\" value=\"";
   echo $external;
   echo "\" />\n";
   echo "</td>\n";

   echo "<td class=\"generic\" >\n";
   echo "<input type=\"text\" name=\"int[]\" value=\"";
   echo $internal;
   echo "\" />\n";
   echo "</td>\n";
   if ($i == $maxdomainsperuser - 1)
   {
      echo "<td class=\"generic\" >(Maximum domains per user reached!\n</td>";
   }
   echo "<td>$error</td>";
   echo "</tr>\n";
}

function printTableHeader()
{
   echo "<tr>\n";
   echo "<td class=\"generic\" >Delete</td>\n";
   echo "<td class=\"generic\" >Domain name</td>\n";
   echo "<td class=\"generic\" >External IP</td>\n";
   echo "<td class=\"generic\" >Internal IP</td>\n";
   echo "</tr>\n";
}

function printNewRow()
{
   include('config.php');
   echo "<tr>\n";
   echo "<td class=\"generic\"  />\n";
   echo "<td class=\"generic\" >\n";
   echo "<input type=\"text\" name=\"newname\" />";
   echo ".".$dnszone;
   echo "</td>\n";
   
   echo "<td class=\"generic\" >\n";
   echo "<input type=\"text\" name=\"newext\" />";
   echo "</td>\n";

   echo "<td class=\"generic\" >\n";
   echo "<input type=\"text\" name=\"newint\" />";
   echo "<td>(Enter a new domain here)</td>";
   echo "</tr>\n";
}


?>
