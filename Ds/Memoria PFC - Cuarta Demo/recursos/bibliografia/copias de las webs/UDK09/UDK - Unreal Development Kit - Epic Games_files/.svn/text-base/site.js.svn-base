// JavaScript Document


//create a variable to hold the file name of the HTML page
var filename = location.pathname.substr(location.pathname.lastIndexOf("/")+1,location.pathname.length);


//loop thru array of links on page and add "selected" class to any link that matches the file name
$(document).ready(function() {
	if (filename && filename != "") {
		$('a[href*=' + filename + ']').addClass('selected');
	}
});

function getCookie(name) {
   return $.cookie(name);
}

function setCookie(name, value) {
   
   $.cookie(name, value);
}