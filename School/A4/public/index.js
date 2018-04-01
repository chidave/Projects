// Put all onload AJAX calls here, and event listeners

$(document).ready(function() {
	
	function clearStat() {
		
		document.getElementById('status_panel').value = "";
		document.getElementById('console_panel').value = "";
		
	}
	clearStat();
	
    // On page-load AJAX Example
    $.ajax({
        type: 'POST',            //Request type
        //dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/upload',   //The server endpoint we are connecting to
        success: function (data) {
            
            console.log(data); 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });

    var modal = document.getElementById('dbInfo');	
    var span = document.getElementsByClassName("close")[0];
    span.onclick = function() {
		alert("You cannot exit this window");
	}
	
	$('#DBForm').submit(function(e){
		e.preventDefault();
		var username = document.forms['DBForm']['username'];
		var password = document.forms['DBForm']['password'];
		var dbname = document.forms['DBForm']['DBName'];
		
		if(username.value == ""){
			alert("Please enter your username");
			return false;
		}
		
		if(password.value == "") {
			alert("Please enter your password");
			return false;
		}
		
		if(dbname.value == "") {
			alert("Please enter the name of the database");
			return false;
		}
		
		check2(username.value, password.value, dbname.value);
		
		modal.style.display = "none";
	});
	
	function check2(username, password, dbname) {
		
		$.ajax({
			type: 'POST', 
			url: '/store',
			data: { Username: username , Password: password , DBname: dbname },
			success: function (data) {
				
				alert(data);
				if(data == "Unable to connect") {
					location.reload();
				}
					 
			},
			fail: function(error) {
					
				console.log(error); 
			}
		});
		
	}
		
	function setStatus() {
		
		var data = sessionStorage.getItem('key');
		document.getElementById('status_panel').value = data;
		sessionStorage.setItem("key", "");
				
	}
	setStatus();
		
	function getFiles() {
		
		$.ajax({
			type: 'GET', 
			url: '/retrieve',
			success: function (data) {
				dropDown(data);
				 
			},
			fail: function(error) {
				
				console.log(error); 
			}
		});
		
		
	}
	getFiles();
	
	function fileLog() {
		
		$.ajax({
			type: 'GET', 
			url: '/retrieve',
			success: function (data) {
				tabulate(data);
				 
			},
			fail: function(error) {
				
				console.log(error); 
			}
		});
		
	}
	fileLog();
	
	function tabulate(file) {
		
		var table = document.getElementById("file_log");
		while ( table.rows.length > 1 ) {
			table.deleteRow(1);
		}
		if(file.length == 0) {
			document.getElementById("storage").disabled = true; 
			document.getElementById("clear").disabled = true;
		}
		for(var i = 0; i < file.length; i++) {
			$.ajax({
				type: 'POST', 
				url: '/parse', //change this
				data: { filename: file[i] },
				success: function (data) {
					
					var obj = JSON.parse(data);
					var row = table.insertRow(1);
					var cell1 = row.insertCell(0);
					var cell2 = row.insertCell(1);
					var cell3 = row.insertCell(2);
					var cell4 = row.insertCell(3);
					var cell5 = row.insertCell(4);
					var cell6 = row.insertCell(5);
					var cell7 = row.insertCell(6);
					var cell8 = row.insertCell(7);
					
					var link = document.createElement('a');
					link.textContent = obj.filename;
					var ref = 'uploads/'+obj.filename;
					link.href = ref;
					//document.getElementById('file_log').insertRow(1).insertCell(0).appendChild(link);
					
					cell1.appendChild(link);
					//cell1.innerHTML = obj.filename;
					cell2.innerHTML = obj.source;
					cell3.innerHTML = obj.version;
					cell4.innerHTML = obj.encoding;
					cell5.innerHTML = obj.subName;
					cell6.innerHTML = obj.subAddress;
					cell7.innerHTML = obj.numOfInd;
					cell8.innerHTML = obj.numOfFam;
					
					if(obj.source == "") {
						var panel = document.getElementById('status_panel').value;
						panel = panel + "Unable to parse file "+obj.filename + "\n";
						document.getElementById('status_panel').value = panel;
						//console.log("Unable to parse file "+obj.filename);
					}
						
					
				},
				fail: function(error) {
					
					console.log(error); 
				}
			});
		}
		
	}
	
	function dropDown(data) {
		var select = document.getElementById("file1");
		for(var i = 0; i < data.length; i++) {
			var option = document.createElement('option');
			option.text = option.value = data[i];
			select.add(option, 1);
		}
		
		select = document.getElementById("file2");
		for(var i = 0; i < data.length; i++) {
			var option = document.createElement('option');
			option.text = option.value = data[i];
			select.add(option, 1);
		}
		
		select = document.getElementById("file3");
		for(var i = 0; i < data.length; i++) {
			var option = document.createElement('option');
			option.text = option.value = data[i];
			select.add(option, 1);
		}
		
		select = document.getElementById("file4");
		for(var i = 0; i < data.length; i++) {
			var option = document.createElement('option');
			option.text = option.value = data[i];
			select.add(option, 1);
		}
	}

	$('#add_indiv').submit(function(e){
		
		e.preventDefault();
		var elem = document.getElementById("file1");
		var selection = elem.options[elem.selectedIndex].text;
		
		if(selection == "-----Select GEDCOM file-----") {
			alert("Please select the name of the file you wish to add to");
			document.add_indiv.file1.focus() ;
			return false;
		}
		
		var count = 0;
		var given = document.getElementById("givenName").value;
		var surn = document.getElementById("lastname").value;
		var elem2 = document.getElementById("sex");
		var gender = "";
		if(elem2.value != "-----Choose sex-----") {
			gender = elem2.value;
		}
		
		if(given == "") {
			count++;
		}
		if(surn == "") {
			count++;
		}
		
		if(count == 2) {
			alert("Please enter a name or a surname");
			document.add_indiv.file1.focus() ;
			return false;
		}
		
		$.ajax({
			type: 'POST', 
			url: '/addIndiv',
			data: { filename: elem.value , firstName: given , surname: surn , sex: gender },
			success: function (data) {
				
				if(data == "SUCCESS") {
					fileLog();
					var panel = document.getElementById('status_panel').value;
					panel = panel + "Added individual to "+elem.value + "\n";
					document.getElementById("status_panel").focus();
					document.getElementById("status_panel").blur();
					document.getElementById('status_panel').value = panel;
					sessionStorage.setItem("key", panel);
					document.getElementById('file1').value = "-----Select GEDCOM file-----";
					document.getElementById('givenName').value = "";
					document.getElementById('lastname').value = "";
					document.getElementById('sex').value = "-----Choose sex-----";
				}
				else{
					var panel = document.getElementById('status_panel').value;
					panel = panel + "Unable to add individual to "+elem.value + "\n";
					document.getElementById("status_panel").focus();
					document.getElementById("status_panel").blur();
					document.getElementById('status_panel').value = panel;
					sessionStorage.setItem("key", panel);
					document.getElementById('file1').value = "-----Select GEDCOM file-----";
					document.getElementById('givenName').value = "";
					document.getElementById('lastname').value = "";
					document.getElementById('sex').value = "-----Choose sex-----";
				}
					 
			},
			fail: function(error) {
					
				console.log(error); 
			}
		});
		
	});
	
	$('#upload_form').submit(function(e){
	
		//e.preventDefault();
		if(document.upload_form.uploadFile.value == "") {
			alert("Please provide the name of the file you wish to upload");
			document.upload_form.uploadFile.focus() ;
			return false;
		}else{
			var file = document.upload_form.uploadFile.value;
			var allowedExtensions = /(\.ged)$/i;
			if(!allowedExtensions.exec(file)){
				alert('Filenames can have extensions .ged only.');
				document.upload_form.uploadFile.focus();
				file.value = '';
				return false;
			}
		}
		
		//fileLog();
		var panel = document.getElementById('status_panel').value;
		panel = panel + "Successfully uploaded "+document.upload_form.uploadFile.value + "\n";
		document.getElementById('status_panel').value = panel;
		sessionStorage.setItem("key", panel);
		//return false;
		
	});
	
		
});

$('#show_files').submit(function(e){
	
	e.preventDefault();
	var elem = document.getElementById("file4");
	var selection = elem.options[elem.selectedIndex].text;
	
	if(selection == "-----Select GEDCOM file-----") {
		alert("Please select the name of the file you wish to view contents of");
		document.show_files.file4.focus() ;
		return false;
	}
	else{
		var file = {};
		file.filename = selection;
		
		
		$.ajax({
			type: 'POST', 
			url: '/viewTab',
			data: { filename: elem.value },
			success: function (data) {
				
				if(data != "FAILED") {
					var obj = JSON.parse(data);
					var table = document.getElementById("view_panel");
					while ( table.rows.length > 1 ) {
						table.deleteRow(1);
					}
					for(var i = 0; i < obj.length; i++) {
						var row = table.insertRow(1);
						var cell1 = row.insertCell(0);
						var cell2 = row.insertCell(1);
						var cell3 = row.insertCell(2);
						cell1.innerHTML = obj[i].givenName;
						cell2.innerHTML = obj[i].surname;
						cell3.innerHTML = obj[i].sex;
						
					}
				}
				else{
					var panel = document.getElementById('status_panel').value;
					panel = panel + "Unable to parse "+elem.value + "\n";
					document.getElementById("status_panel").focus();
					document.getElementById("status_panel").blur();
					document.getElementById('status_panel').value = panel;
					document.getElementById('file4').value = "-----Select GEDCOM file-----";
					sessionStorage.setItem("key", panel);
				}
				 
			},
			fail: function(error) {
				
				console.log(error); 
			}
		});
	}
	
});

function test() {
	
	var select = document.getElementById("file2");
	
	if(select.value != "-----Select GEDCOM file-----") {
	
		$.ajax({
			type: 'POST', 
			url: '/getIndiv', 
			data: { filename: select.value },
			success: function (data) {
				
				if(data == "FAILED") {
					var select = document.getElementById("indiv");
					while(select.length > 1) {
						select.remove(select.length-1);
					}
					alert("Error. Unable to retrieve individuals from selected file");
				}
				else{
					//console.log(data);
					var obj = JSON.parse(data);
					//console.log(obj[0].givenName);
					
					var select = document.getElementById("indiv");
					while(select.length > 1) {
						select.remove(select.length-1);
					}
					
					for(var i = 0; i < obj.length; i++) {
						var option = document.createElement('option');
						var stringVal = obj[i].givenName;
						stringVal = stringVal.concat(" ");
						stringVal = stringVal.concat(obj[i].surname);
						option.text = option.value = stringVal;
						select.add(option, 1);
					}			
				 }
			},
			fail: function(error) {
				// Non-200 return, do something with error
				console.log(error); 
			}
		});
		
	}else{
		
		var select = document.getElementById("indiv");
		while(select.length > 1) {
			select.remove(select.length-1);
		}
		
	}
	
}

function test2() {
	
	var select = document.getElementById("file3");
	
	if(select.value != "-----Select GEDCOM file-----") {
	
		$.ajax({
			type: 'POST', 
			url: '/getIndiv', 
			data: { filename: select.value },
			success: function (data) {
				
				if(data == "FAILED") {
					var select = document.getElementById("indiv2");
					while(select.length > 1) {
						select.remove(select.length-1);
					}
					alert("Error. Unable to retrieve individuals from selected file");
				}
				else{
					//console.log(data);
					var obj = JSON.parse(data);
					//console.log(obj[0].givenName);
					
					var select = document.getElementById("indiv2");
					while(select.length > 1) {
						select.remove(select.length-1);
					}
					
					for(var i = 0; i < obj.length; i++) {
						var option = document.createElement('option');
						var stringVal = obj[i].givenName;
						stringVal = stringVal.concat(" ");
						stringVal = stringVal.concat(obj[i].surname);
						option.text = option.value = stringVal;
						select.add(option, 1);
					}			
				 }
			},
			fail: function(error) {
				// Non-200 return, do something with error
				console.log(error); 
			}
		});
		
	}else{
		
		var select = document.getElementById("indiv2");
		while(select.length > 1) {
			select.remove(select.length-1);
		}
		
	}
	
}

function addToDrop(id, data) {
	
	var select = document.getElementById(id);
	while(select.length > 1) {
		select.remove(select.length-1);
	}
	
	for(var i = 0; i < data.length; i++) {
		var option = document.createElement('option');
		option.text = option.value = data[i];
		select.add(option, 1);
	}
	
}

function validateCreate() {
	
	if(document.create_form.filename.value == "") {
		alert("Please provide the name of the file you wish to create");
		document.create_form.filename.focus() ;
        return false;
	}else{
		var file = document.create_form.filename.value;
		var allowedExtensions = /(\.ged)$/i;
		if(!allowedExtensions.exec(file)){
			alert('Filenames can have extensions .ged only.');
			document.create_form.filename.focus() ;
			file.value = '';
			return false;
		}
	}
	
	if(document.create_form.subName.value == "") {
		alert("Please provide the name of the submitter of this file");
		document.create_form.subName.focus() ;
        return false;
	}
	
	var panel = document.getElementById('status_panel').value;
	panel = panel + "Successfully created file "+document.create_form.filename.value + "\n";
	document.getElementById("status_panel").focus();
	document.getElementById("status_panel").blur();
	document.getElementById('status_panel').value = panel;
	sessionStorage.setItem("key", panel);
	
	/*var filename = document.getElementById("filename").value;
	console.log(filename);
	
	var data = {};
	data.filename = filename;
	
	$.ajax({
		type: 'POST',
		datatype: 'jsonp',
		data: JSON.stringify(data),
		url: '/create',
		success: function (data) {
            
            //console.log(data); 
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
   });*/
	
}

$('#get_des').submit(function(e){
	
	e.preventDefault();
	var elem = document.getElementById("file2");
	var selection = elem.options[elem.selectedIndex].text;
	
	if(selection == "-----Select GEDCOM file-----") {
		alert("Please select the name of the file you wish to select from");
		document.get_des.file2.focus() ;
		return false;
	}
	
	var elem2 = document.getElementById("indiv");
	var selection2 = elem2.options[elem2.selectedIndex].text;
	
	if(selection2 == "-----Select individual-----") {
		alert("Please select the name of the individual you wish find descendants of");
		document.get_des.indiv.focus() ;
		return false;
	}
	
	$.ajax({
		type: 'POST', 
		url: '/getDesc',
		data: { filename: elem.value , person: elem2.value },
		success: function (data) {
			
			//console.log(data);
			var obj = JSON.parse(data);
			var table = document.getElementById("desc");
			while ( table.rows.length > 1 ) {
				table.deleteRow(1);
			}
			for(var i = 0; i < obj.length; i++) {
				var row = table.insertRow(1+i);
				var cell1 = row.insertCell(0);
				var cell2 = row.insertCell(1);
				cell1.innerHTML = i+1;
				var individuals = "";
				for(var j = 0; j < obj[i].length; j++) {
					individuals = individuals + obj[i][j].givenName + " " + obj[i][j].surname;
					if(j == obj[i].length - 1) {
						break;
					}
					individuals = individuals + ", ";
				}
				cell2.innerHTML = individuals;
			}
				 
		},
		fail: function(error) {
				
			console.log(error); 
		}
	});
	
});

$('#get_ans').submit(function(e){
	
	e.preventDefault();
	var elem = document.getElementById("file3");
	var selection = elem.options[elem.selectedIndex].text;
	
	if(selection == "-----Select GEDCOM file-----") {
		alert("Please select the name of the file you wish to select from");
		document.get_ans.file3.focus() ;
		return false;
	}
	
	var elem2 = document.getElementById("indiv2");
	var selection2 = elem2.options[elem2.selectedIndex].text;
	
	if(selection2 == "-----Select individual-----") {
		alert("Please select the name of the individual you wish find ancestors of");
		document.get_ans.indiv2.focus() ;
		return false;
	}
	
	$.ajax({
		type: 'POST', 
		url: '/getAns',
		data: { filename: elem.value , person: elem2.value },
		success: function (data) {
			
			//console.log(data);
			var obj = JSON.parse(data);
			var table = document.getElementById("ansc");
			while ( table.rows.length > 1 ) {
				table.deleteRow(1);
			}
			for(var i = 0; i < obj.length; i++) {
				var row = table.insertRow(1+i);
				var cell1 = row.insertCell(0);
				var cell2 = row.insertCell(1);
				cell1.innerHTML = i+1;
				var individuals = "";
				for(var j = 0; j < obj[i].length; j++) {
					individuals = individuals + obj[i][j].givenName + " " + obj[i][j].surname;
					if(j == obj[i].length - 1) {
						break;
					}
					individuals = individuals + ", ";
				}
				cell2.innerHTML = individuals;
			}
				 
		},
		fail: function(error) {
				
			console.log(error); 
		}
	});
	
	//return false;
	
});

$('#storage').click(function(){
	clearTables();
	
	$.ajax({
		type: 'GET', 
		url: '/retrieve',
		async: false,
		success: function (data) {

			getFileInfo(data);
			displayDB();
			 
		},
		fail: function(error) {
			
			console.log(error); 
		}
	});
	document.getElementById("clear").disabled = false;
	
});

function clearTables() {
	
	$.ajax({
		type: 'GET', 
		url: '/removeTabs',
		success: function (data) {

			//console.log(data);
			 
		},
		fail: function(error) {
			
			console.log(error); 
		}
	});
	
}

	
function getFileInfo(file) {
		
	var files = "[";
	for(var i = 0; i < file.length; i++) {
		
		$.ajax({
			type: 'POST', 
			url: '/parse', //change this
			data: { filename: file[i] },
			async: false,
			success: function (data) {
				
				//var obj = JSON.parse(data);
				//console.log(data);
				
				files = files + data;
				if(i != (file.length - 1)) {
					files = files + ",";
				}
				else{
					files = files + "]";
					
					populateTable(files);
				}
								
			},
			fail: function(error) {
				
				console.log(error); 
			}
		});
	}
	
	
		
}

function populateTable(files) {
	
	var obj = JSON.parse(files);
	//clear both tables first
	
	for(var i = 0; i < obj.length; i++) {
		$.ajax({
			type: 'POST', 
			url: '/storeFiles',
			data: obj[i],
			async: false,
			success: function (data) {
				
				getIndiFromFile(data);
				 
			},
			fail: function(error) {
				
				console.log(error); 
			}
		});
	}
	
}


function getIndiFromFile(file) {
	
	$.ajax({
		type: 'POST', 
		url: '/viewTab',
		data: { filename: file },
		success: function (data) {
			
			if(data != "FAILED") {
				//console.log(file);
				populateTable2(data, file);
			}
			else{
				//
			}
				 
		},
		fail: function(error) {
			
			console.log(error); 
		}
	});
	
}

function populateTable2(individuals, file) {
	
	var indivs = JSON.parse(individuals);
	
	for(var i = 0; i < indivs.length; i++ ) {
		
		$.ajax({
			type: 'POST', 
			url: '/storeIndivs',
			data: { filename: file , givenName: indivs[i].givenName , surname: indivs[i].surname , sex: indivs[i].sex },
			async: false,
			success: function (data) {
				
				//getIndiFromFile(data);
				 
			},
			fail: function(error) {
				
				console.log(error); 
			}
		});
		
	}
	
}

$('#clear').click(function(){
	
	$.ajax({
		type: 'GET', 
		url: '/clearTables',
		async: false,
		success: function (data) {
			
			displayDB();
				 
		},
		fail: function(error) {
			
			console.log(error); 
		}
	});
	document.getElementById("clear").disabled = true;
	
});

$('#disp').click(function(){
	
	$.ajax({
		type: 'GET', 
		url: '/dispStat',
		success: function (data) {
			
			var obj = JSON.parse(data);
			var panel = "Database has "+obj.fileNum+" files and "+obj.indiNum+" individuals";
			document.getElementById('console_panel').value = panel;
				 
		},
		fail: function(error) {
			
			console.log(error); 
		}
	});
	
});

function displayDB() {
	
	$.ajax({
		type: 'GET', 
		url: '/dispStat',
		success: function (data) {
			
			var obj = JSON.parse(data);
			var panel = "Database has "+obj.fileNum+" files and "+obj.indiNum+" individuals";
			document.getElementById('console_panel').value = panel;
				 
		},
		fail: function(error) {
			
			console.log(error); 
		}
	});
	
}

$('#subQuery').click(function(){
	
	if(document.querySelector('input[name="query"]:checked').value == "1") {
		
		nameSort();
		
	}
	
});

function nameSort() {
	
	$.ajax({
		type: 'GET', 
		url: '/nameSort',
		async: false,
		success: function (data) {
			
			
				 
		},
		fail: function(error) {
			
			console.log(error); 
		}
	});
	
}


//make function that deletes tables only when store files is clicked

