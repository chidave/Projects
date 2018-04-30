<!DOCTYPE html>
<html>
<head>
	<title>Create a new account</title>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" type="text/css" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
	<link rel="stylesheet" type="text/css" href="../CSS/style.css">
	<link rel="stylesheet" type="text/css" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css">
	<script src="https://code.jquery.com/jquery-3.3.1.min.js" integrity="sha256-FgpCb/KJQlLNfOu91ta32o/NMZxltwRo8QtmkMRdAu8=" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>
</head>

<body id="registerbody">
	
	<?php
		require '../config.php';
		session_start();
		$error='';

		if(isset($_POST['submit'])){
			
			if (mysqli_connect_errno()) {
				$error = "Failed to connect to MySQL";
			}
			
			if($_SERVER["REQUEST_METHOD"] == "POST") {
				
				$un = mysqli_real_escape_string($db,$_POST['username']);
				$pwd = mysqli_real_escape_string($db,$_POST['password']);
				$email= mysqli_real_escape_string($db,$_POST['email']);
				
				$sql = "SELECT user_id FROM Users WHERE username = '$un'";
				$result = mysqli_query($db,$sql);
				$count = mysqli_num_rows($result);
				
				if ($count == 0) {
					$sql1 = "INSERT INTO Users (username,email,password) VALUES ('$un','$email','$pwd')";
					$result1 = mysqli_query($db,$sql1);
					
					if($result1) {
						$error = "Account successfully created";
						header("Location:../index.php?error=".$error);
					}
					else{
						$error = "One of your fields is invalid";
					}
				}
				else{
					$error = "Sorry, this username is already taken";
				}
				
			}
			
		}
		else{
			//$error = "An error occured";
		}
	?>
	
	<br><br><br>
	<div class="header_class"><h1 id="header2">CREATE A NEW ACCOUNT</h1></div>
	<div class="form_box2"><br>
		<div class="header_class"><p id="logintitle">REGISTER</p><br></div>
		<form id="registerform" action="" method="post">
			Username:<p id="asterisk">* </p><br><input type="text" name="username" id="username"><br><br>
			Email address(for account recovery):<p id="asterisk">* </p><br><input type="text" name="email" id="email"><br><br>
			Password:<p id="asterisk">* </p><br><input type="password" name="password" id="password"><br><br>
			Confirm Password:<p id="asterisk">* </p><br><input type="password" name="password2" id="password2"><br><br>
			<input type="hidden" name="form_submitted" value="1" />
			<div class="login_bottom">
				<br><input type="submit" value="Create account" id="create" name='submit'><br>
				<span><?php echo $error; ?></span>
			</div>
		</form>
	</div>
	
	<script src="../JS/index.js"></script>
</body>

</html>

