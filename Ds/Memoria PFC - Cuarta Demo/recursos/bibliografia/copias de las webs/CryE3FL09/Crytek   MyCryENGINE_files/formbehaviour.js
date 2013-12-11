window.onload = function(){

	if($('nlName').value==''){$('nlName').value = "your name";}
	$('nlName').onfocus=function(){if(this.value=='your name'){this.value='';this.style.color='#5b5b5b';}}
	$('nlName').onblur=function(){if(this.value==''){this.value='your name';this.style.color='#5b5b5b';}}
	
	if($('nlEmail').value==''){$('nlEmail').value = "your email";}
	$('nlEmail').onfocus=function(){if(this.value=='your email'){this.value='';this.style.color='#5b5b5b';}}
	$('nlEmail').onblur=function(){if(this.value==''){this.value='your email';this.style.color='#5b5b5b';}}

	
}

function blurMail(mail)	{
	if(mail.value==''){
		mail.value='your email';
		mail.style.color='#5b5b5b';
	}
}

function blurName(name)	{
	if(name.value==''){
		name.value='your name';
		name.style.color='#5b5b5b';
	}
}

function focusMail(mail)	{
	if(mail.value=='your email'){
		mail.value='';
		mail.style.color='#5b5b5b';
	}
}

function focusName(name)	{
	if(name.value=='your name'){
		name.value='';
		name.style.color='#5b5b5b';
	}
}