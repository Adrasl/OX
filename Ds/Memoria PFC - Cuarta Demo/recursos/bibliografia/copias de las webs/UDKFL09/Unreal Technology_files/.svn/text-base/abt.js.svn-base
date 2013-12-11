/* 

Atlantic BT base scripts
v. 1.01

last mod: 2-28-07

*/




// This takes care of those ugly dotted borders that show up when you click on a link
var theahrefs = document.getElementsByTagName('a');
//fix dotted line thing when link is OnClicked
for(var x=0;x!=theahrefs.length;x++){
	theahrefs[x].onfocus = function stopLinkFocus(){this.hideFocus=true;};
}
//a:focus { -moz-outline-style: none; }   <-- this is found in the moz.css file to take care of firefox



	/*
    Written by Jonathan Snook, http://www.snook.ca/jonathan
    Add-ons by Robert Nyman, http://www.robertnyman.com
	*/
	
	function getElementsByClassName(oElm, strTagName, strClassName){
		var arrElements = (strTagName == "*" && document.all)? document.all : oElm.getElementsByTagName(strTagName);
		var arrReturnElements = new Array();
		strClassName = strClassName.replace(/\-/g, "\\-");
		var oRegExp = new RegExp("(^|\\s)" + strClassName + "(\\s|$)");
		var oElement;
		for(var i=0; i<arrElements.length; i++){
			oElement = arrElements[i];      
			if(oRegExp.test(oElement.className)){
				arrReturnElements.push(oElement);
			}   
		}
		return (arrReturnElements)
	}
	
	function getElementsByAttribute(oElm, strTagName, strAttType, strAttName){
		var arrElements = (strTagName == "*" && document.all)? document.all : oElm.getElementsByTagName(strTagName);
		var arrReturnElements = new Array();
		strAttName = strAttName.replace(/\-/g, "\\-");
		var oRegExp = new RegExp("(^|\\s)" + strAttName + "(\\s|$)");
		var oElement;
		for(var i=0; i<arrElements.length; i++){
			oElement = arrElements[i];      
			if(oRegExp.test(oElement.getAttribute(strAttType))){
				arrReturnElements.push(oElement);
			}   
		}		
		return (arrReturnElements)
	}
	
	function toggle(obj) {
		var el = document.getElementById(obj);
		if ( el.style.display != 'none' ) {
			el.style.display = 'none';
		}
		else {
			el.style.display = '';
		}
	}
	
	/* mod: 2-28-07 */	
	function filterFor(strFilter, parentElm, elmType, elmClass, elmAttr){
		
		// strFilter = string to search and filter for, can be any text within any attribute
		
		// parentElm = parent node/element to start searching from
		
		// elmType = type of element to search for (i.e. div, li, dl, etc)
		// elmClass = class of element to search for
		// elmAttr = attribute name in element
		
		/* example of use:
		
		onclick="filterFor('ship-tracking','tools-box','div','sub-box','id'); setOffState('tools-nav','li','on'); this.parentNode.className = 'on'; return false;"
		
		1. this will filter through all <div>'s within <tools-box> that have a class of 'sub-box' looking for the <div> that has an 'id' that contains 'ship-tracking'.
		2. then it will set all <li> that has a class of 'on' in the element named 'tools-nav' to have a class of ''
		3. then this.parentNode.className will set the <li> that contains the <a> being clicked to 'on'
		4. then return false; prevents the # in the href from being followed
		
		*/
		
		elmList = document.getElementById(parentElm);
		var arrElements =  getElementsByClassName(document.getElementById(parentElm), elmType, elmClass);
		
		for(var i=0; i<arrElements.length; i++){
			oElement = arrElements[i];
			oElement.style.display = 'none';
		}
		if (strFilter != "$recent") {
			var arrSelectElements = getElementsByAttribute(elmList, elmType, elmAttr, strFilter);
			for(var i=0; i<arrSelectElements.length; i++){
				oElement = arrSelectElements[i];
				oElement.style.display = '';
			}
		} else {
			var today = new Date();
			var thisyear = today.getFullYear();
			var lastyear = today.getFullYear() - 1;
			var arrSelectElements = getElementsByAttribute(elmList, elmType, elmAttr, String(thisyear));
			for(var i=0; i<arrSelectElements.length; i++){
				oElement = arrSelectElements[i];
				oElement.style.display = '';
			}
			var arrSelectElements = getElementsByAttribute(elmList, elmType, elmAttr, String(lastyear));
			for(var i=0; i<arrSelectElements.length; i++){
				oElement = arrSelectElements[i];
				oElement.style.display = '';
			}
		}
	}
	
	
	function setOffState(parentName, elmType, elmClass) {
		
		currentOns = getElementsByClassName(document.getElementById(parentName), elmType, elmClass);
		for (i=0;i<currentOns.length;i++){
			currentOns[i].className = "";
		}
	}
	
	

	
	
// By Matt Read (http://mattread.com/archives/2005/10/faking-two-column-text/) //	

// --- Event Handler Functions --- //
// from: http://www.scottandrew.com/weblog/articles/cbs-events
function addEvent(obj, evType, fn, useCapture) {
	if (obj.addEventListener) {
		obj.addEventListener(evType, fn, useCapture);
		return true;
	}
	else if (obj.attachEvent) {
		var r = obj.attachEvent("on"+evType, fn);
		return r;
	}
	else
		alert("Handler could not be attached");
}

function removeEvent(obj, evType, fn, useCapture) {
	if (obj.removeEventListener) {
		obj.removeEventListener(evType, fn, useCapture);
		return true;
	}
	else if (obj.detachEvent) {
		var r = obj.detachEvent("on"+evType, fn);
		return r;
	}
	else
		alert("Handler could not be removed");
}

// Other functions //
function $(id) {
	return document.getElementById(id);
}

function $2(id) {
	return document.getElementsByClassName(document.all,'*',id);
}

function $tags(name) {
	return document.getElementsByTagName(name);
}

function $new(type, id) {
	var element = document.createElement(type);
	if (id) element.id = id;
	return element;
}

function $text(text) {
	return document.createTextNode(text);
}