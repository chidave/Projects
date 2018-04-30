<?php 
	if($_GET){
		$error=$_GET['error'];
	} else {
		$error='';
	}
?>

<!DOCTYPE html>
<html>
<head>
	<title>Test-Drive:  - To test out my coding skills</title>
	<meta charset="utf-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" type="text/css" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
	<link rel="stylesheet" type="text/css" href="CSS/style.css">
	<link rel="stylesheet" type="text/css" href="https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css">
	<script src="https://code.jquery.com/jquery-3.3.1.min.js" integrity="sha256-FgpCb/KJQlLNfOu91ta32o/NMZxltwRo8QtmkMRdAu8=" crossorigin="anonymous"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>
</head>

<body id="indexbody">
	<br><br><br><br>
	<div class="header_class"><h1 id="header1">TEST-DRIVE</h1></div>
	<div class="form_box"><br>
		<div class="header_class"><p id="logintitle">LOGIN</p><br></div>
		<form id="loginform">
			Username(or email):<p id="asterisk">* </p><br><input type="text" name="username" id="username"><br><br>
			Password:<p id="asterisk">* </p><br><input type="password" name="password" id="password"><br><br>
			<div class="login_bottom">
				<input type="submit" value="Login" id="login"><br>
				<a href="#">Forgot Password?</a><br>
				<a href="html/register.php">New User? Create an account here</a><br><br><br>
				<span id="error"><?php echo "<font color=\"red\">$error</font>"; ?></span>
			</div>
		</form>
	</div>
	
	<script src="./JS/index.js"></script>
</body>

</html>
