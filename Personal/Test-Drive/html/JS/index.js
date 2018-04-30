$(document).ready(function() {
	
	
	
});

$('#loginform').submit(function(e) {
	
	//e.preventDefault();
	var usr = document.getElementById("username");
	var pwd = document.getElementById("password");
	
	if(usr.value == "") {
		alert("Please enter your username or email.");
		return false;
	}
	if(pwd.value == "") {
		alert("Please enter your password");
		return false;
	}
	
});

$('#registerform').submit(function(e) {
	
	//e.preventDefault();
	var usr = document.getElementById("username");
	var pwd = document.getElementById("password");
	var pwd2 = document.getElementById("password2");
	var email = document.getElementById("email");
	
	if(usr.value == "") {
		alert("Please choose a username.");
		return false;
	}
	if(pwd.value == "") {
		alert("Please enter a password");
		return false;
	}
	if(pwd2.value == "") {
		alert("Please confirm your password");
		return false;
	}
	if(email.value == "") {
		alert("Please enter your email address");
		return false;
	}
	if(pwd.value != pwd2.value) {
		alert("The passwords you entered don't match");
		return false;
	}
	
	
});
