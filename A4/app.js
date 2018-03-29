'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');
const bodyparser = require('body-parser');
const mysql = require('mysql');
var connection;

app.use(fileUpload());
app.use(bodyparser());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
	
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let sharedLib = ffi.Library('./sharedLib', {
	//return type first, argument list second
	//for void input type, leave argumrnt list empty	//int return, int argument
	'getDesc' : [ 'string', [] ],
	'writeFile' : [ 'void', [ 'string', 'string', 'string' ] ],
	'getIndividuals' : [ 'string', [ 'string' ] ],
	'parseFile' : [ 'string', [ 'string' ] ],
	'retrieveIndividuals' : [ 'string', [ 'string' ] ],
	'getIndAsc' : [ 'string', [ 'string', 'string' ] ],
	'getIndDesc' : [ 'string', [ 'string', 'string' ] ],
	'insertIndividual' : [ 'string', [ 'string', 'string', 'string', 'string' ] ],
  
});

//Sample endpoint
app.get('/someendpoint', function(req , res){
  res.send({
    foo: "bar"
  });
});

app.post('/store', (req, res) => {
	
	//console.log("HERE");
	//console.log(req.body.Username);
	//fix res.send()
	
	var username = req.body.Username;
	var password = req.body.Password;
	var dbname = req.body.DBname;
	
	connection = mysql.createConnection({
		host     : 'dursley.socs.uoguelph.ca',
		user     : username,
		password : password,
		database : dbname
	});
	
	var returnVal = "";
	connection.connect(function(err) {
		if(err) {
			console.log("Unable to connect");
			//res.send("Unable to connect");
			returnVal = "Unable to connect";
		}
		else{
			var file = "CREATE TABLE IF NOT EXISTS FILE(file_id INT NOT NULL AUTO_INCREMENT, file_Name VARCHAR(60) NOT NULL, source VARCHAR(250) NOT NULL, version VARCHAR(10) NOT NULL, encoding VARCHAR(10) NOT NULL, sub_name VARCHAR(62) NOT NULL, sub_addr VARCHAR(256) NOT NULL, num_individuals INT, num_families INT, PRIMARY KEY ( file_id ))";
			connection.query(file, function(err, result) {
				if(err) {
					console.log("Unable to create FILE");
					//res.send("Unable to connect");
					//returnVal = "Unable to connect";
				}
				else{
					console.log("Sucessfully created FILE");
					
					var indivs = "CREATE TABLE IF NOT EXISTS INDIVIDUAL(ind_id INT NOT NULL AUTO_INCREMENT, surname VARCHAR(256) NOT NULL, given_name VARCHAR(256) NOT NULL, sex VARCHAR(1), fam_size INT, source_file INT, PRIMARY KEY ( ind_id ), FOREIGN KEY (source_file) REFERENCES FILE(file_id) ON DELETE CASCADE)";
					connection.query(indivs, function(err, result) {
						if(err) {
							console.log("Unable to create INDIVIDUAL");
						}
						else{
							console.log("Successfully created INDIVIDUAL");
						}
					});
				}
			});
			
			//var individuals = 
			
			console.log("Connected successfully");
			res.send("Connected successfully");
			//returnVal = "Connected successfully";
		}
	});
	
	//res.send(returnVal);
	//res.redirect('/'); 
	//res.send("DONE");  
	
});

app.get('/retrieve', function(req , res){
	const fs = require("fs-extra");
	
	let directory = "uploads";
	let dirBuf = Buffer.from(directory);
	
	let files = fs.readdirSync(directory);
	
	res.send(files);
});

app.post('/create', (req, res) => {
	/*console.log(req.body.filename);
	console.log(req.body.subName);
	console.log(req.body.subAddress);*/
	
	sharedLib.writeFile(req.body.filename, req.body.subName, req.body.subAddress);
	
	res.redirect('/');   
	
});

app.post('/getIndiv', (req, res) => {
	
	//console.log(req.body.filename);   
	let indivs = sharedLib.getIndividuals(req.body.filename);
	//console.log(indivs);
	res.send(indivs);
	
});

app.post('/parse', (req, res) => {
	
	let parsed = sharedLib.parseFile(req.body.filename);
	//console.log(parsed);
	
	res.send(parsed);
	
});

app.get('/download', (req, res) => {
	
	//console.log(req.body);
	/*var file = __dirname + '/uploads/' + req.body.filename;
	res.download(file);*/
	
});

app.post('/viewTab', (req, res) => {
	
	//console.log(req.body.filename);
	let people = sharedLib.retrieveIndividuals(req.body.filename);
	//console.log(people);
	
	res.send(people);
	
});

app.post('/getAns', (req, res) => {
	
	let ascendants = sharedLib.getIndAsc(req.body.filename, req.body.person);
	//console.log(ascendants);
	res.send(ascendants);
	
});

app.post('/getDesc', (req, res) => {
	
	let descendants = sharedLib.getIndDesc(req.body.filename, req.body.person);
	//console.log(ascendants);
	res.send(descendants);
	
});

app.post('/addIndiv', (req, res) => {
	
	let reply = sharedLib.insertIndividual(req.body.filename, req.body.firstName, req.body.surname, req.body.sex);
	//console.log(reply);
	
	res.send(reply);
	
});

app.post('/storeFiles', (req, res) => {
	
	var inds = parseInt(req.body.numOfInd);
	var fams = parseInt(req.body.numOfFam);
	
	var filename = "'"+req.body.filename+"'";
	var source = "'"+req.body.source+"'";
	var version = "'"+req.body.version+"'";
	var encoding = "'"+req.body.encoding+"'";
	var subName = "'"+req.body.subName+"'";
	var subAddress = "'"+req.body.subAddress+"'";
	var indivs = "'"+req.body.numOfInd+"'";
	var famils = "'"+req.body.numOfFam+"'";
	
	var add = "INSERT INTO FILE (file_Name, source, version, encoding, sub_name, sub_addr, num_individuals, num_families) VALUES (" +filename+","+ source+","+ version+","+ encoding+","+ subName+","+ subAddress+"," +indivs+","+ famils +")";
	console.log(add);
	connection.query(add, function (err, result) {
		if (err) {
			console.log("ERROR WHILE INSERTING");
		}else{
			console.log("1 record inserted");
		}
  });
	  
	  res.end();
	
});



app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
