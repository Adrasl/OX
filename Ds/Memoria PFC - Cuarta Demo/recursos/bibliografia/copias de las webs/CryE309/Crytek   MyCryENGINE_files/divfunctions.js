

var http_ce2admin = null;
var timer = null;
var companyid_gl=null;


if (window.XMLHttpRequest) {
	http_ce2admin = new XMLHttpRequest();
} else if (window.ActiveXObject) {
	http_ce2admin = new ActiveXObject("Microsoft.XMLHTTP");
}

function init()
{
	if (http_ce2admin != null) 
	{
		http_ce2admin.open("POST", "ajax_functions.php", true);
		http_ce2admin.setRequestHeader(
		      "Content-Type",
		      "application/x-www-form-urlencoded");
	}
}



function SendToCRM(companyid)
{
	
	if (http_ce2admin != null) 
	{
		init();
		companyid_gl=companyid;
		//parentnamenew=document.getElementById('parentnamenew'+parentidlok).value;
		http_ce2admin.send("step=sendcotocrm&companyid="+companyid);
		http_ce2admin.onreadystatechange = get_request;
		timer = window.setTimeout("cancel()", 5000);
		
	}
	return false;
}

function get_request() {
	if (http_ce2admin.readyState == 4) 
	{
		req=http_ce2admin.responseText;
		if(req.search("SUCCESS")!=-1)
		{
			document.getElementById("coCRM_"+companyid_gl).style.display='none';
		}
		else
		{
			document.getElementById("coCRM_"+companyid_gl).innerHTML=document.getElementById("coCRM_"+companyid_gl).innerHTML+'<br/>'+req;
		}
      	window.clearTimeout(timer);
		return false;
	}
}

function cancel() {
	http_ce2admin.abort();
   alert("NOT SUCCESS");
}