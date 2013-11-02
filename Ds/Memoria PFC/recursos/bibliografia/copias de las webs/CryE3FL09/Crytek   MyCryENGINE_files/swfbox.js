/*
	Slimbox v1.31 - The ultimate lightweight Lightbox clone
	by Christophe Beyls (http://www.digitalia.be) - MIT-style license.
	Inspired by the original Lightbox v2 by Lokesh Dhakar.
	
	SWFBOX mod by 0lds3wl..
*/

var Flashbox = {

	init: function(options){
		this.options = Object.extend({
			resizeDuration: 400,
			resizeTransition: Fx.Transitions.sineInOut,
			initialWidth: 360,
			initialHeight: 202,
			animateCaption: true
		}, options || {});

		this.anchors = [];
		$each(document.links, function(el){
			if (el.rel && el.rel.test(/^flashbox/i)){
				el.onclick = this.click.pass(el, this);
				this.anchors.push(el);
			}
		}, this);
		this.eventPosition = this.position.bind(this);

		this.overlay = new Element('div').setProperty('id', 'flbOverlay').injectInside(document.body);
		this.overlay.onclick = this.close.bind(this);
		
		this.center = new Element('div').setProperty('id', 'flbCenter').setStyles({width: this.options.initialWidth+'px', height: this.options.initialHeight+'px', marginLeft: '-'+(this.options.initialWidth/2)+'px', display: 'none'}).injectInside(document.body);
		this.image = new Element('div').setProperty('id', 'flbSWF').injectInside(this.center);
		
		this.bottomContainer = new Element('div').setProperty('id', 'flbBottomContainer').setStyle('display', 'none').injectInside(document.body);
		this.bottom = new Element('div').setProperty('id', 'flbBottom').injectInside(this.bottomContainer);
		new Element('a').setProperties({id: 'flbCloseLink', href: '#'}).injectInside(this.bottom).onclick = this.overlay.onclick = this.close.bind(this);
		
		this.center.style.backgroundColor = '#000000';
		this.image.style.backgroundColor = 'transparent';

		var nextEffect = this.nextEffect.bind(this);
		this.fx = {
			overlay: this.overlay.effect('opacity', {duration: 500}).hide(),
			resize: this.center.effects({duration: this.options.resizeDuration, transition: this.options.resizeTransition, onComplete: nextEffect}),
			image: this.image.effect('opacity', {duration: 500, onComplete: nextEffect}),
			bottom: this.bottom.effect('margin-top', {duration: 400, onComplete: nextEffect})
		};


	},

	click: function(link){
		if (link.rel.length == 8) return this.show(link.href, link.title);

		var j, imageNum, images = [];
		this.anchors.each(function(el){
			if (el.rel == link.rel){
				for (j = 0; j < images.length; j++) if(images[j][0] == el.href) break;
				if (j == images.length){
					images.push([el.href, el.title]);
					if (el.href == link.href) imageNum = j;
				}
			}
		}, this);
		return this.open(images, imageNum);
	},

	show: function(url, title){
		return this.open([[url, title]], 0);
	},

	open: function(images, imageNum){
		this.images = images;
		this.position();
		this.setup(true);
		this.top = window.getScrollTop() + (window.getHeight() / 15);
		this.center.setStyles({top: this.top+'px', display: ''});
		this.fx.overlay.start(0.8);
		return this.setPlayer(imageNum);
	},

	position: function(){
		//this.center.setStyles({top: window.getScrollTop()+100+'px'}); // scroll video????
		this.overlay.setStyles({top: window.getScrollTop()+'px', height: window.getHeight()+'px'});
	},

	setup: function(open){
		var elements = $A(document.getElementsByTagName('object'));
		elements.extend(document.getElementsByTagName(window.ie ? 'select' : 'embed'));
		elements.each(function(el){
			if (open) el.lbBackupStyle = el.style.visibility;
			el.style.visibility = open ? 'hidden' : el.lbBackupStyle;
		});
		var fn = open ? 'addEvent' : 'removeEvent';
		window[fn]('scroll', this.eventPosition)[fn]('resize', this.eventPosition);
		//document[fn]('keydown', this.eventKeyDown);
		this.step = 0;
		this.width=720;
		this.height=444;

	},
 

	setPlayer: function(imageNum){
		this.step = 1;

		this.center.style.backgroundColor = '';
		this.bottomContainer.style.display = 'none';
		//this.fx.image.hide();
		

		this.fx.resize.start({height: this.height});
		//this.nextEffect();
		return false;
	},

	nextEffect: function(){
		
		switch (this.step++){
		case 1:
			this.image.style.width = this.bottom.style.width = this.width+'px';
			this.fx.resize.start({width: this.width, marginLeft: -this.width/2});
			//this.step++;
			this.center.className = 'flbLoading';
			break;
		case 2:
			this.image.style.marginLeft = this.center.style.marginLeft
			so = new SWFObject("http://newdesign.crytek.de/fileadmin/swf/flvplayer.swf", "flv-viewer", this.width, this.height, "9", "#000000");
			so.addParam( "allowFullScreen", "true");
			so.addParam("scale", "noscale");
			so.addParam("salign", "TL");
			//so.addParam("wmode", "transparent");
			/* Skin Laden Function Spinnt!!!
			   hab den Skin erstmal in die Root gepackt.. so gehts 
			   (keine url?) muss am player code gefixed werden!
			*/
			so.addVariable("skinfile", "video-flash-de-skin.swf");
			so.addVariable("flashvideo", this.images[0][0]);
			//so.addVariable("wmode", "transparent");
			so.addVariable("autoplay", true);
			so.addVariable("loop", true);
			//alert(this.images[0][0]);
			so.write("flbCenter");
			
			
			this.width=document.getElementById("flv-viewer").width;
			this.height=document.getElementById("flv-viewer").height;
			//alert(document.getElementById("flv-viewer").height);
			this.bottom.style.width = (this.width-20)+'px'
			this.bottomContainer.setStyles({top: (this.top + this.center.clientHeight)+'px', height: '0px', marginLeft: this.center.style.marginLeft, display: ''});
			this.fx.bottom.set(-this.bottom.offsetHeight);
			this.bottomContainer.style.height = '';
			this.fx.bottom.start(0);
			
			//this.step++;
			break;
		case 3:

			break;
		case 4:
			break;
		case 5:
		}

		return false;
	},

	close: function(){
		this.center.style.display = 'none';
		this.fx.overlay.chain(this.setup.pass(false, this)).start(0);
		this.center.style.display = this.bottomContainer.style.display = 'none';
		return false;
	}
};

window.addEvent('domready', Flashbox.init.bind(Flashbox));
