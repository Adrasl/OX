var global_timerid=null;

jQuery.noConflict();
jQuery(document).ready(function($){
	/*
	$.each($('.text_02, .text_02_b, .head_01, .devlogin, .text_02_head, .text_02_head_license'), function(){
		$('<span class="shadow">' + this.innerHTML + '</span>').appendTo(this);
	});
	*/

	$.each(['awards_menu', 'downloads_menu'], function(){
		var div = $('div.' + this),
			ul = $('ul.' + this),
			ulPadding = 0;
	
		var divWidth = div.width();
	
		div.css({overflow: 'hidden'});
		
		var lastLi = ul.find('li:last-child');
		
		div.mousemove(function(e){

			var ulWidth = lastLi[0].offsetLeft + lastLi.outerWidth() + ulPadding;	
			var left = (e.pageX - div.offset().left) * (ulWidth-divWidth) / divWidth;
			div.scrollLeft(left);
		});		
	});
});

function ce3_rpcRequest(target, service, login, password) {
	
	new Ajax.Request(target,{
		parameters  : {'service':service, 'procedure':'checkAuth', 'username':login, 'action':service, 'login': login, 'password':password},			
		onSuccess: function(transport){
			// Error occured
			if(service=='JIRA' || service=='CONFLUENCE')
			{
				
				if(transport.responseJSON.Exception) {
					if(service=='Confluence') 
						alert('Confluence can not authenticate this user: '+transport.responseJSON.Exception.message); 
					else if(service=='JIRA')
						alert('JIRA can not authenticate this user: '+transport.responseJSON.Exception.message);
					
					//location.href='/';
					
					return false;   
				}
				
			}
			
			if(transport.responseText==false)
			{
				document.getElementById('incorrectlogin').style.display='block';
				if(document.getElementById('flashcontent') && document.getElementById('flashcontent').style.display=='none')
					document.getElementById('flashcontent').style.display='block';
				
				if(document.getElementById('messageboxlogin'))
					document.getElementById('messageboxlogin').style.display='none';
				return false;
			}
			
			// No errors, service is accacible with those login/password
			if(service=='Confluence') {
				// Confluence reported the login/password are correct
				
				var confluenceAuthForm = document.getElementById('confluenceAuthForm');
				// Authorizate user on confluence site
				confluenceAuthForm.os_username.value=login;
				confluenceAuthForm.os_password.value=password;
				//confluenceAuthForm.action='http://mycryengine:8090/login.action';
				confluenceAuthForm.submit();
				// Check about JIRA now
				//ce3_rpcRequest('core/subsystemsController.php', 'JIRA', login, password);
				//location.href='/';
				
				global_timerid = window.setTimeout("redirect_login()", 2000);

			} else if(service=='JIRA') {
				// JIRA reported the login/password are correct 
				var jiraAuthForm = document.getElementById('jiraAuthForm');
				jiraAuthForm.os_username.value=login;
				jiraAuthForm.os_password.value=password;
				//jiraAuthForm.action='http://mycryengine:8080/login.action';
				jiraAuthForm.submit();
				
			  	// since everything is ok, we can submit login form on cryengine site
				 CryEngine3AuthForm.submit();
				// location.href='/';
			} else if(service=='login'){
				if(transport.responseText=='termsandconditions')
				{
					document.getElementById('CryEngine3AuthForm').action='/termsandconditions';
					document.getElementById('CryEngine3AuthForm').submit();
					return false;
				}
				ce3_rpcRequest('/core/UserAction.php', 'phpbblogin', login, password);
			} else if(service=='phpbblogin'){
				ce3_rpcRequest('/core/subsystemsController.php', 'Confluence', login, password);
			} else if(service=='termsandconditions'){
				
				ce3_rpcRequest('/core/UserAction.php', 'phpbblogin', login, password);
			}

		 	return false;
		}
	});
	return false;		
}

function ce3_Auth(CryEngine3AuthForm) {
	var login = $('username').value;
	var password = $('password').value;

	if(document.getElementById('flashcontent'))
		document.getElementById('flashcontent').style.display='none';
	
	if(document.getElementById('incorrectlogin').style.display=='block')
		document.getElementById('incorrectlogin').style.display='none';

	if(!document.getElementById('messageboxlogin'))
		document.body.innerHTML=document.body.innerHTML+'<div id="messageboxlogin" style="z-index:9999; position:fixed; top:0px; left:0px; width:100%; height:1000px; border:0px solid white; background-image: url(/img/transparent.png); display:block; text-align:center; padding-top:20%;"><img src="/img/loading.gif"></div>';
	else
		document.getElementById('messageboxlogin').style.display='block';
	
	ce3_rpcRequest('/core/UserAction.php', 'login', login, password);
	return false;
}

function ce3_Auth_termsandconditions(CryEngine3AuthForm) {
	var login = $('username').value;
	var password = $('password').value;

	if(document.getElementById('incorrectlogin').style.display=='block')
		document.getElementById('incorrectlogin').style.display='none';

	if(!document.getElementById('messageboxlogin'))
		document.body.innerHTML=document.body.innerHTML+'<div id="messageboxlogin" style="z-index:9999; position:fixed; top:0px; left:0px; width:100%; height:1000px; border:0px solid white; background-image: url(/img/transparent.png); display:block; text-align:center; padding-top:20%;"><img src="/img/loading.gif"></div>';
	else
		document.getElementById('messageboxlogin').style.display='block';
	
	ce3_rpcRequest('/core/UserAction.php', 'termsandconditions', login, password);
	return false;
}

function ce3_logoutRequest(target, service) {
	
	if(document.getElementById('flashcontent') && document.getElementById('flashcontent').style.display=='block')
		document.getElementById('flashcontent').style.display='none';
	
	new Ajax.Request(target,{
		parameters  : {'service':service, 'action':service},			
		onSuccess: function(transport){
			// Error occured
			if(service=='JIRA' || service=='Confluence')
			{
				
				if(transport.responseJSON.Exception) {
					if(service=='Confluence') 
						alert('Confluence can not authenticate this user: '+transport.responseJSON.Exception.message); 
					else if(service=='JIRA')
						alert('JIRA can not authenticate this user: '+transport.responseJSON.Exception.message);
					
			//		location.href='/';
					
					//return false;   
				}
				
			}
			
			// No errors, service is accacible with those login/password
			if(service=='phpbblogout'){
				//var confluenceAuthForm = document.getElementById('confluenceAuthForm');
				// Authorizate user on confluence site
				//confluenceAuthForm.action='http://mycryengine:8090/logout.action';
				document.getElementById('confluenceLogoutForm').submit();
				// Check about JIRA now
				//ce3_rpcRequest('core/subsystemsController.php', 'JIRA', login, password);
				
				/*
				// JIRA reported the login/password are correct 
				var jiraAuthForm = document.getElementById('jiraAuthForm');
				jiraAuthForm.action='http://mycryengine:8080/logout.action';
				jiraAuthForm.submit();
				*/
				
				//ce3_logoutRequest('http://mycryengine:8090/logout.action', 'Confluence');
				//location.href='/';
				
				global_timerid = window.setTimeout("redirect_logout()", 2000);
				//ce3_logoutRequest('core/UserAction.php', 'phpbblogout');
				/*
			} else if(service=='phpbblogout'){
				ce3_logoutRequest('/core/UserAction.php', 'logout');
				*/
			}

		 	return false;
		}
	});
	return false;		
}

function logout(CryEngine3AuthForm, sid) {
	
	if(!document.getElementById('messageboxlogin'))
		document.body.innerHTML=document.body.innerHTML+'<div id="messageboxlogin" style="z-index:9999; position:fixed; top:0px; left:0px; width:100%; height:1000px; border:0px solid white; background-image: url(/img/transparent.png); display:block; text-align:center; padding-top:20%;"><img src="/img/loading.gif"></div>';
	else
		document.getElementById('messageboxlogin').style.display='block';
	
	ce3_logoutRequest('/forums/ucp.php?mode=logout&sid='+sid, 'phpbblogout');
	return false;
}

function DeleteFieldContent(field)
{
	if(field.value=='Username' || field.value=='Password')
		field.value="";
}

function redirect_login()
{
	
	
	if(document.getElementById('CryEngine3AuthForm'))
	{
		document.getElementById('CryEngine3AuthForm').submit();
	}
	
	window.clearTimeout(global_timerid);
}

function redirect_logout()
{
	
	
	if(document.getElementById('CryEngine3LogoutForm'))
	{
		document.getElementById('CryEngine3LogoutForm').submit();
	}
	
	window.clearTimeout(global_timerid);
}

var videoinner=null;

function show_image(imageurl)
{
	if(document.getElementById('div_video'))
	{
		videoinner=document.getElementById('div_video').innerHTML;
		document.getElementById('div_video').innerHTML=' ';
//		document.getElementById('div_video').style.width='448px';
		document.getElementById('div_video').style,display='none';
	}
	document.getElementById('div_image_slideshow').innerHTML='<div style="position:relative; top:10%; vertical-align:middle; text-align:center;"><img id="image_slideshow_image" src="'+imageurl+'?id='+Math.random()+'" style="cursor: pointer;"></div>';
	document.getElementById('div_image_slideshow').style.display='block';
	
} 

function ChangePositionTextBoxState()
{
	var value = document.userform.Position.value.toUpperCase();	
	
	if (value == "[OTHER]")
		document.userform.PositionText.disabled = false;
	else
	{
		document.userform.PositionText.disabled = true;
		document.userform.PositionText.value = "";
		
	}
}
/****************************************************************************/
var http=null;
var id=null;

function connect_to_ajax_script(ajax_file)
{
	if (window.XMLHttpRequest) {
   		http = new XMLHttpRequest();
	} else if (window.ActiveXObject) {
   		http = new ActiveXObject("Microsoft.XMLHTTP");
	}
	
	if (http != null) {
		http.open("POST", ajax_file, true);
		//http.onreadystatechange = ausgeben;
		http.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
		id = window.setTimeout("cancel()", 5500);
	}
	
	return http;
}

function cancel() {
	http.abort();
}

function getpagecontent(contentid, divid)
{
	http=connect_to_ajax_script("/custom_ajax_functions.php");
	http.onreadystatechange = function() {
		setpagecontent(http, contentid, divid);
	};
	http.send("step=getcontent&contentid="+contentid);
}

function setpagecontent(http, contentid, divid) {
	if (http.readyState == 4) {
		//document.getElementById(picid_to_delete).innerHTML="DELETED";
		//document.getElementById("edittext").value=http.responseText;
		document.getElementById("page_name").value=http.responseText;
		
		document.getElementById('contenteditdiv').style.left='0px';
		document.getElementById('contenteditdiv').style.width='100%';

		window.clearTimeout(id);
	}
}