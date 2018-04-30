<?php
	require '../config.php';

	if(isset($_POST['form_submitted'])){
		
		if (mysqli_connect_errno()) {
			echo "Failed to connect to MySQL: " . mysqli_connect_error();
		}
		else{
			echo "Connected successfully";
		}
		
		$un = $_POST['username'];
		$em = $_POST['email'];
		$pwd = $_POST['password'];
		
	}
	else{
		echo "SHOOTO";
	}
?>
