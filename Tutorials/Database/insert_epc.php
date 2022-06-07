<?php

if(isset($_GET["epc"])) {
   $epc = $_GET["epc"]; 

   $servername = "localhost";
   $username = "CarelessWISPer";
   $password = "shep_123";
   $dbname = "db_epc";
   $timeout = "20"; 
   // Create connection
   $conn = new mysqli($servername, $username, $password, $dbname);
   // Check connection
   if ($conn->connect_error) {
      die("Connection failed: " . $conn->connect_error);
   }

   //$sql = "INSERT INTO epc_vals (tag_epc) VALUES ($epc)";
	$sql = "CALL insert_data($epc,$timeout)"; 
   if ($conn->query($sql) === TRUE) {
      echo "New record created successfully";
   } else {
      echo "Error: " . $sql . " => " . $conn->error;
   }

   $conn->close();
} else {
   echo "epc is not set";
}
?>