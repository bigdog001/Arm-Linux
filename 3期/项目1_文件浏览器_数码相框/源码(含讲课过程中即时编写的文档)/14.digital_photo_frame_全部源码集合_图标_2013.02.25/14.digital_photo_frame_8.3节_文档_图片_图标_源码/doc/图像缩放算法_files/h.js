(function(){var c={id:"0ee5e8cdc4d43389b3d1bfd76e83216b",dm:["chinaunix.net","chinaunix.com"],etrk:[],js:"tongji.baidu.com/hm-web/js/",icon:'',br:false,ctrk:false,align:-1,nv:0,vdur:1800000,age:31536000000,se:[[1,'baidu.com','word|wd|w',1,'news,tieba,zhidao,,image,video,hi,baike,wenku,opendata,jingyan'],[2,'google.com','q',0,'tbm=isch,tbm=vid,tbm=nws|source=newssearch,tbm=blg,tbm=frm'],[4,'sogou.com','query|keyword',1,'news,mp3,pic,v,gouwu,zhishi,blogsearch'],[6,'search.yahoo.com','p',1,'news,images,video'],[7,'yahoo.cn','q',1,'news,image,music'],[8,'soso.com','w|key',1,'image,video,music,sobar,wenwen,news,baike'],[11,'youdao.com','q',1,'image,news,,mp3,video'],[12,'gougou.com','search',1,',movie,,,,,video'],[13,'bing.com','q',2,'images,videos,news'],[14,'so.com','q',1,'video,news'],[15,'jike.com','q',1,'news,image,video'],[16,'qihoo.com','kw',0,''],[17,'etao.com','q',1,'s,8'],[18,'soku.com','keyword',2,'a'],[19,'easou.com','q',0,''],[20,'glb.uc.cn','keyword|word|q',0,'']]};var g=true,h=null,i=false;function k(a,b,d){var e;d.h&&(e=new Date,e.setTime(e.getTime()+d.h));document.cookie=a+"="+b+(d.domain?"; domain="+d.domain:"")+(d.path?"; path="+d.path:"")+(e?"; expires="+e.toGMTString():"")+(d.N?"; secure":"")};(function(){function a(){if(!a.c){a.c=g;for(var b=0,d=e.length;b<d;b++)e[b]()}}function b(){try{document.documentElement.doScroll("left")}catch(d){setTimeout(b,1);return}a()}var d=i,e=[],f;document.addEventListener?f=function(){document.removeEventListener("DOMContentLoaded",f,i);a()}:document.attachEvent&&(f=function(){document.readyState==="complete"&&(document.detachEvent("onreadystatechange",f),a())});(function(){if(!d)if(d=g,document.readyState==="complete")a.c=g;else if(document.addEventListener)document.addEventListener("DOMContentLoaded",
f,i),window.addEventListener("load",a,i);else if(document.attachEvent){document.attachEvent("onreadystatechange",f);window.attachEvent("onload",a);var e=i;try{e=window.frameElement==h}catch(j){}document.documentElement.doScroll&&e&&b()}})();return function(b){a.c?b():e.push(b)}})().c=i;function m(a,b){if(window.sessionStorage)try{window.sessionStorage.setItem(a,b)}catch(d){}}function n(a){return window.sessionStorage?window.sessionStorage.getItem(a):h};function o(a,b){var d=a.match(RegExp("(^|&|\\?|#)("+b+")=([^&#]*)(&|$|#)",""));return d?d[3]:h}function p(a){var b;return(b=(a=a.match(/^(https?:\/\/)?([^\/\?#]*)/))?a[2].replace(/.*@/,""):h,a=b)?a.replace(/:\d+$/,""):a};var q=navigator.cookieEnabled,r=navigator.javaEnabled(),s=navigator.language||navigator.browserLanguage||navigator.systemLanguage||navigator.userLanguage||"",t=window.screen.width+"x"+window.screen.height,u=window.screen.colorDepth;function v(a,b){var d=new Image,e="mini_tangram_log_"+Math.floor(Math.random()*2147483648).toString(36);window[e]=d;d.onload=d.onerror=d.onabort=function(){d.onload=d.onerror=d.onabort=h;d=window[e]=h;b&&b(a)};d.src=a};function w(a,b,d){a.attachEvent?a.attachEvent("on"+b,function(b){d.call(a,b)}):a.addEventListener&&a.addEventListener(b,d,i)};var x;function y(){if(!x)try{x=document.createElement("input"),x.type="hidden",x.style.display="none",x.addBehavior("#default#userData"),document.getElementsByTagName("head")[0].appendChild(x)}catch(a){return i}return g};c.se.push([14,"so.360.cn","q",1,",news"]);c.se.push([14,"v.360.cn","q",1,"so"]);var z=0,B=Math.round(+new Date/1E3),C=window.location.protocol=="https:"?"https:":"http:",D="cc,cf,ci,ck,cl,cm,cp,cw,ds,ep,et,fl,ja,ln,lo,lt,nv,rnd,sb,se,si,st,su,sw,sse,v,cv,lv,u,api".split(",");function E(){if(typeof window["_bdhm_loaded_"+c.id]=="undefined")window["_bdhm_loaded_"+c.id]=g,this.a={},this.L=[],this.m={},this.e=0,this.C()}
E.prototype={A:function(){var a="";try{external.twGetVersion(external.twGetSecurityID(window))&&external.twGetRunPath.toLowerCase().indexOf("360se")>-1&&(a=17)}catch(b){}return a},i:function(a,b){var a="."+a.replace(/:\d+/,""),b="."+b.replace(/:\d+/,""),d=a.indexOf(b);return d>-1&&d+b.length==a.length},n:function(a,b){a=a.replace(/^https?:\/\//,"");return a.indexOf(b)==0},b:function(a){for(var b=0;b<c.dm.length;b++)if(c.dm[b].indexOf("/")>-1){if(this.n(a,c.dm[b]))return g}else{var d=p(a);if(d&&this.i(d,
c.dm[b]))return g}return i},k:function(){for(var a=window.location.hostname,b=0,d=c.dm.length;b<d;b++)if(this.i(a,c.dm[b]))return c.dm[b].replace(/(:\d+)?[\/\?#].*/,"");return a},l:function(){for(var a=0,b=c.dm.length;a<b;a++){var d=c.dm[a];if(d.indexOf("/")>-1&&this.n(window.location.href,d))return d.replace(/^[^\/]+(\/.*)/,"$2")+"/"}return"/"},B:function(){if(document.referrer)for(var a=0,b=c.se.length;a<b;a++){if(RegExp("(^|\\.)"+c.se[a][1].replace(/\./g,"\\.")).test(p(document.referrer))){var d=
o(document.referrer,c.se[a][2])||"";if(d||!(c.se[a][0]!=2&&c.se[a][0]!=14&&c.se[a][0]!=17)){c.se[a][0]==1&&document.referrer.indexOf("cpro.baidu.com")>-1&&(d="");this.a.se=c.se[a][0];for(var b=this.a,e=c.se[a],a=0,f=e[3]==2?e[1]+"\\/":"",l=e[3]==1?"\\."+e[1]:"",e=e[4].split(","),j=0,A=e.length;j<A;j++)if(e[j]!==""&&RegExp(f+e[j]+l).test(document.referrer)){a=j+1;break}b.sse=a;this.a.sw=d;return 2}}}else return B-z>c.vdur?1:4;d=i;return(d=this.b(document.referrer)&&this.b(window.location.href)?g:this.i(p(document.referrer)||
"",window.location.hostname))?B-z>c.vdur?1:4:3},getData:function(a){try{var b,d=RegExp("(^| )"+a+"=([^;]*)(;|$)").exec(document.cookie);if(!(b=d?d[2]:h)){var e;if(!(e=n(a)))a:{if(window.localStorage){var f=window.localStorage.getItem(a);if(f){var l=f.indexOf("|"),j=f.substring(0,l)-0;if(j&&j>(new Date).getTime()){e=f.substring(l+1);break a}}}else if(y())try{x.load(window.location.hostname);e=x.getAttribute(a);break a}catch(A){}e=h}b=e}return b}catch(H){}},setData:function(a,b,d){try{if(k(a,b,{domain:this.k(),
path:this.l(),h:d}),d){var e=new Date;e.setTime(e.getTime()+d||31536E6);try{if(window.localStorage)b=e.getTime()+"|"+b,window.localStorage.setItem(a,b);else if(y())x.expires=e.toUTCString(),x.load(window.location.hostname),x.setAttribute(a,b),x.save(window.location.hostname)}catch(f){}}else m(a,b)}catch(l){}},F:function(a){try{if(k(a,"",{domain:this.k(),path:this.l(),h:-1}),window.sessionStorage&&window.sessionStorage.removeItem(a),window.localStorage)window.localStorage.removeItem(a);else if(y())try{x.load(window.location.hostname),
x.removeAttribute(a),x.save(window.location.hostname)}catch(b){}}catch(d){}},J:function(){var a,b,d,e,f;z=this.getData("Hm_lpvt_"+c.id)||0;z.length==13&&(z=Math.round(z/1E3));b=this.B();a=b!=4?1:0;if(d=this.getData("Hm_lvt_"+c.id)){e=d.split(",");for(f=e.length-1;f>=0;f--)e[f].length==13&&(e[f]=""+Math.round(e[f]/1E3));for(;B-e[0]>2592E3;)e.shift();f=e.length<4?2:3;for(a===1&&e.push(B);e.length>4;)e.shift();d=e.join(",");e=e[e.length-1]}else d=B,e="",f=1;this.setData("Hm_lvt_"+c.id,d,c.age);this.setData("Hm_lpvt_"+
c.id,B);d=B==this.getData("Hm_lpvt_"+c.id)?"1":"0";if(c.nv==0&&this.b(window.location.href)&&(document.referrer==""||this.b(document.referrer)))a=0,b=4;this.a.nv=a;this.a.st=b;this.a.cc=d;this.a.lt=e;this.a.lv=f},I:function(){for(var a=[],b=0,d=D.length;b<d;b++){var e=D[b],f=this.a[e];typeof f!="undefined"&&f!==""&&a.push(e+"="+encodeURIComponent(f))}return a.join("&")},K:function(){this.J();this.a.si=c.id;this.a.su=document.referrer;this.a.ds=t;this.a.cl=u+"-bit";this.a.ln=s;this.a.ja=r?1:0;this.a.ck=
q?1:0;this.a.lo=typeof _bdhm_top=="number"?1:0;var a=this.a,b="";if(navigator.plugins&&navigator.mimeTypes.length){var d=navigator.plugins["Shockwave Flash"];d&&d.description&&(b=d.description.replace(/^.*\s+(\S+)\s+\S+$/,"$1"))}else if(window.ActiveXObject)try{if(d=new ActiveXObject("ShockwaveFlash.ShockwaveFlash"))(b=d.GetVariable("$version"))&&(b=b.replace(/^.*\s+(\d+),(\d+).*$/,"$1.$2"))}catch(e){}a.fl=b;this.a.sb=this.A();this.a.v="1.0.37";this.a.cv=decodeURIComponent(this.getData("Hm_cv_"+c.id)||
"");this.a.api=0;a=window.location.href;this.a.cm=o(a,"hmmd")||"";this.a.cp=o(a,"hmpl")||"";this.a.cw=o(a,"hmkw")||"";this.a.ci=o(a,"hmci")||"";this.a.cf=o(a,"hmsr")||""},C:function(){var a=this;try{a.K();a.a.nv==0?a.H():a.j(".*");a.t();a.z();a.s&&a.s();a.r&&a.r();a.g=new F;w(window,"beforeunload",a.G());if(a.D()){var b=window._hmt;if(b&&b.length)for(var d=0,e=b.length;d<e;d++)a.q.call(a,b[d]);a.m.push=function(){a.q.apply(a,arguments)};window._hmt=a.m}if(typeof window._bdhm_autoPageview==="undefined"||
window._bdhm_autoPageview===g)a.a.et=0,a.a.ep="",a.d()}catch(f){b=[],b.push("si="+c.id),b.push("n="+encodeURIComponent(f.name)),b.push("m="+encodeURIComponent(f.message)),b.push("r="+encodeURIComponent(document.referrer)),v(C+"//hm.baidu.com/hm.gif?"+b.join("&"))}},G:function(){var a=this;return function(){a.a.et=3;a.a.ep=(new Date).getTime()-a.g.o+","+((new Date).getTime()-a.g.f+a.g.p);a.d()}},d:function(){var a=this;a.a.rnd=Math.round(Math.random()*2147483647);a.a.api=a.a.api||a.e?a.a.api+"_"+a.e:
"";var b=C+"//hm.baidu.com/hm.gif?"+a.I();a.a.api=0;a.e=0;a.w(b);v(b,function(b){a.j(b)})},t:function(){if(c.icon!=""){var a,b=c.icon.split("|"),d="http://tongji.baidu.com/hm-web/welcome/ico?s="+c.id;a=(C=="http:"?"http://eiv":"https://bs")+".baidu.com"+b[0]+"."+b[1];switch(b[1]){case "swf":var e=b[2],b=b[3],d="s="+d,f="HolmesIcon"+B;a='<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" id="'+f+'" width="'+e+'" height="'+b+'"><param name="movie" value="'+a+'" /><param name="flashvars" value="'+
d+'" /><param name="allowscriptaccess" value="always" /><embed type="application/x-shockwave-flash" name="'+f+'" width="'+e+'" height="'+b+'" src="'+a+'" flashvars="'+d+'" allowscriptaccess="always" /></object>';break;case "gif":a='<a href="'+d+'" target="_blank"><img border="0" src="'+a+'" width="'+b[2]+'" height="'+b[3]+'"></a>';break;default:a='<a href="'+d+'" target="_blank">'+b[0]+"</a>"}document.write(a)}},z:function(){var a=window.location.hash.substring(1),b=RegExp(c.id),d=document.referrer.indexOf("baidu.com")>
-1?g:i;a&&b.test(a)&&d&&(b=document.createElement("script"),b.setAttribute("type","text/javascript"),b.setAttribute("charset","utf-8"),b.setAttribute("src",C+"//"+c.js+o(a,"jn")+"."+o(a,"sx")+"?"+this.a.rnd),a=document.getElementsByTagName("script")[0],a.parentNode.insertBefore(b,a))},q:function(a){if(function(a){if(Object.prototype.toString.call(a)!=="[object Array]")return i;for(var b=a.length-1;b>=0;b--){var d=a[b];if(!("[object Number]"==Object.prototype.toString.call(d)&&isFinite(d))&&"[object String]"!=
Object.prototype.toString.call(d)&&d!==g&&d!==i)return i}return g}(a)){var b=function(a){return a.replace?a.replace(/'/g,"'0").replace(/\*/g,"'1").replace(/!/g,"'2"):a};switch(a[0]){case "_trackPageview":if(a.length>1&&a[1].charAt&&a[1].charAt(0)=="/"){this.a.api|=4;this.a.et=0;this.a.ep="";var b=this.a.u,d=this.a.su;this.a.u=C+"//"+window.location.host+a[1];this.a.su=window.location.href;this.d();this.a.u=b;this.a.su=d}break;case "_trackEvent":if(a.length>2)this.a.api|=8,this.a.et="4",this.a.ep=
b(a[1])+"*"+b(a[2])+(a[3]?"*"+b(a[3]):"")+(a[4]?"*"+b(a[4]):""),this.d();break;case "_setCustomVar":if(a.length<4)break;var d=a[1],e=a[4]||3;if(d>0&&d<6&&e>0&&e<4){this.e++;for(var f=(this.a.cv||"*").split("!"),l=f.length;l<d-1;l++)f.push("*");f[d-1]=e+"*"+b(a[2])+"*"+b(a[3]);this.a.cv=f.join("!");a=this.a.cv.replace(/[^1](\*[^!]*){2}/g,"*").replace(/((^|!)\*)+$/g,"");a!==""?this.setData("Hm_cv_"+c.id,encodeURIComponent(a),c.age):this.F("Hm_cv_"+c.id)}}}},D:function(){if(window._bdhm_account&&window._bdhm_account!==
c.id)return i;var a=window._hmt;if(!a||!a.length)return g;for(var b=g,d=0;d<a.length;d++){var e=a[d];switch(e[0]){case "_setAccount":if(e.length>1&&/^[0-9a-z]{32}$/.test(e[1]))this.a.api|=1,window._bdhm_account=e[1],e[1]!==c.id&&(b=i);break;case "_setAutoPageview":if(e.length>1&&(e=e[1],i===e||g===e))this.a.api|=2,window._bdhm_autoPageview=e}}return b},w:function(a){var b=n("Hm_unsent_"+c.id)||"",d=this.a.u?"":"&u="+encodeURIComponent(window.location.href),b=encodeURIComponent(a.replace(/^https?:\/\//,
"")+d)+(b?","+b:"");m("Hm_unsent_"+c.id,b)},j:function(a){var b=n("Hm_unsent_"+c.id)||"";b&&((b=b.replace(RegExp(encodeURIComponent(a.replace(/^https?:\/\//,"")).replace(/([\*\(\)])/g,"\\$1")+"(%26u%3D[^,]*)?,?","g"),"").replace(/,$/,""))?m("Hm_unsent_"+c.id,b):window.sessionStorage&&window.sessionStorage.removeItem("Hm_unsent_"+c.id))},H:function(){var a=this,b=n("Hm_unsent_"+c.id);if(b)for(var b=b.split(","),d=0,e=b.length;d<e;d++)v(C+"//"+decodeURIComponent(b[d]).replace(/^https?:\/\//,""),function(b){a.j(b)})}};
function F(){this.f=this.o=(new Date).getTime();this.p=0;typeof document.M=="object"?(w(document,"focusin",G(this)),w(document,"focusout",G(this))):(w(window,"focus",G(this)),w(window,"blur",G(this)))}function G(a){return function(b){if(!(b.target&&b.target!=window)){if(b.type=="blur"||b.type=="focusout")a.p+=(new Date).getTime()-a.f;a.f=(new Date).getTime()}}}new E;})();
