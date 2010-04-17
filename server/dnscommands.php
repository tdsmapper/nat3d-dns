<?php

function modifyDomain($id, $domname, $ext, $int, &$errMsgs)
{
   include('config.php');
   $ok = true;
   $query = "SELECT * FROM `$tabledns` WHERE `$coldomname`='$domname' AND `$colid`='$id';";
   $result = mysql_query($query);
   if (result)
   {
      if (!validateIpAddress($ext))
      {
         $errMsgs[$domname] = "External IP incorrect. ";
         $ok = false; 
      }
      if (!validateIpAddress($int))
      {
         $errMsgs[$domname] = $errMsgs[$domname]."Internal IP incorrect. ";
         $ok = false; 
      }

      if ($ok)
      {
         if (deleteDomain($domname, $errMsgs))
         {
            if (addDomain($id, $domname, $ext, $int, $errMsgs))
            {
               return true;
            }
            else
            {
               $errMsgs[$domname] = "Failed to modify domain(a)!";
            }
         }
         else
         {
            $errMsgs[$domname] = "Failed to modify domain(d)!";
         }
      }
   }
   else
   {
      $errMsgs[$domname] = "Domain does not belong to you!";
   }
   return false;
}


function addDomain($id, $domname, $ext, $int, &$errMsgs)
{
   $ok = true;
   include('config.php');
   if (!validateIpAddress($ext))
   {
      $errMsgs[$domname] = "External IP incorrect. ";
      $ok = false; 
   }
   if (!validateIpAddress($int))
   {
      $errMsgs[$domname] = $errMsgs[$domname]."Internal IP incorrect. ";
      $ok = false;
   }
   if ($ok)
   {
      $query = "SELECT * FROM `$tabledns` WHERE `$colid`='$id'";
      $result = mysql_query($query);
      $numdomains = mysql_numrows($result);
      if ($numdomains < $maxdomainsperuser)
      {
         if ($ok)
         {
            /* Does this domain already exist? */
            $query = "SELECT * FROM `$tabledns` WHERE `$coldomname`='$domname';";
            $result = mysql_query($query);
            $nrows = mysql_numrows($result);
            if ($nrows == 0) // Doesn't exist. So lets add
            {
               $dnsrec = makeNatRecord($domname, $ext, $int);
               if ($dnsrec)
               {
                  if (sendDnsUpdate($dnsrec, "add"))
                  {
                     $query = "INSERT INTO `$tabledns`(`$colid`,`$coldomname`,`$colexternal`,`$colinternal`) ".
                        " VALUES('$id', '$domname', '$ext', '$int');";
                     $result = mysql_query($query);
                     if ($result)
                     {
                        return true;
                     }
                  }
               }
            }
            else
            {
               $errMsgs[$domname] = "Domain alread exists!";
               return false;
            }
         }
      }
   }
   $errMsgs[$domname] = "Unable to modify domain as per your request(a)!";
   return false;
}

function deleteDomain($domname, &$errMsgs)
{
   include('config.php');
   if (sendDnsUpdate($domname.".".$dnszone.".", "delete"))
   {
      $query = "DELETE FROM `$tabledns` WHERE `$coldomname`='$domname';";
      $result = mysql_query($query);
      if ($result)
      {
         return true;
      }
   }
   $errMsgs[$domname] = "Unable to modify domain as per your request(d)!";
   return false;
}

function makeNatRecord($name, $ext, $int)
{
   include('config.php');
   $command =  "$gen_nat3_pl $name $dnszone $ext $nat3dport $int";
   ob_start();
   passthru($command, $retval);
   $dnsrec = ob_get_contents();
   ob_end_clean();
   if ($retval == 0)
   {
      return $dnsrec;
   }
   return false;
}

function sendDnsUpdate($dnsrecord, $type)
{  
   include('config.php');
   if ((strcmp($type, "add") == 0) ||
      (strcmp($type, "delete") == 0))
   {
      $tmpfile = tempnam("/tmp", "nat");
      if ($tmpfile)
      {
         $fh = fopen($tmpfile, "w");
         if ($fh)
         {
            fwrite($fh, "server $dnsserver\n");
            fwrite($fh, "zone $dnszone\n");
            fwrite($fh, "update $type $dnsrecord\n");
            fwrite($fh, "send\n");
            system("cat /tmp/$tmpfile");
            $op = system("nsupdate $tmpfile", $ret);
            fclose($fh);
            unlink($tmpfile);

            if ($ret == 0)
            {
               return true;
            }
         }
      }
   }
   return false;
}

?>
