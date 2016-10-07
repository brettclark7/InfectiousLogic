/*jslint node:true, vars:true, bitwise:true, unparam:true */
/*jshint unused:true */
/*jshint esnext:true */
/*jshint esversion:6 */

/* Environment Variables */
const PORT = process.env.PORT;
const MAPKEY = process.env.MAPKEY;
console.log("Express will listen on port: "+PORT);
console.log("Google Maps API Key: "+MAPKEY);

/* Module Imports */
const fs = require('fs');
const express = require('express');
const pug = require('pug');
const serveIndex = require('serve-index');

/* Enable Express & Pug */
var app = express();
app.set('view engine', 'pug');
app.set('views', __dirname+'/views');
app.set('MAPKEY', MAPKEY);

/* Configure access to data logs & js */
app.use('/scripts', express.static(__dirname+'/public/scripts'));
app.use('/ftp', express.static(__dirname+'/public/data'));

/* Create directory listing for data logs */
app.use('/ftp', serveIndex(__dirname+'/public/data'));

/*********************************************************/
/*                       Routing                         */
/*********************************************************/

app.get('/', function(request, response) {
    response.render('index', {Title: 'Home'});
    console.log("Routing Visitor " + request.connection.remoteAddress + " to Index.");
});

app.get('/accel*', function(request, response) {
    response.render('accel', {Title: 'Accel'});
    console.log("Routing Visitor " + request.connection.remoteAddress + " to Accel.");
});

app.get('/gps*', function(request, response) {
    response.render('gps', {Title: 'GPS'});
    console.log("Routing Visitor " + request.connection.remoteAddress + " to GPS.");
});

app.get('/logs*', function(request, response) {
    response.render('logs', {Title: 'Data Logs'});
    console.log("Routing Visitor " + request.connection.remoteAddress + " to Logs.");
});

app.get('/config*', function(request, response) {
    response.render('config', {Title: 'Config'});
    console.log("Routing Visitor " + request.connection.remoteAddress + " to Config.");
});

/*********************************************************/
/*                   Start the server                    */
/*********************************************************/

app.listen(PORT, function() {
    console.log('Listening on '+PORT);
});