!function(n,r){"object"==typeof exports&&"object"==typeof module?module.exports=r():"function"==typeof define&&define.amd?define([],r):"object"==typeof exports?exports.LibSampleRate=r():n.LibSampleRate=r()}(this,(function(){return(()=>{var n={17:(n,r,e)=>{var t;function o(n){return(o="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(n){return typeof n}:function(n){return n&&"function"==typeof Symbol&&n.constructor===Symbol&&n!==Symbol.prototype?"symbol":typeof n})(n)}n=e.nmd(n);var i,a=(i=(i="undefined"!=typeof document&&document.currentScript?document.currentScript.src:void 0)||__filename,function(n){var r,t,a=void 0!==(n=n||{})?n:{};a.ready=new Promise((function(n,e){r=n,t=e}));var u,f={};for(u in a)a.hasOwnProperty(u)&&(f[u]=a[u]);var c=[],s=function(n,r){throw r},l=!1,p=!1,d=!1,h=!1;l="object"===("undefined"==typeof window?"undefined":o(window)),p="function"==typeof importScripts,d="object"===("undefined"==typeof process?"undefined":o(process))&&"object"===o(process.versions)&&"string"==typeof process.versions.node,h=!l&&!d&&!p;var v,y,m,g,w="";function b(n){return a.locateFile?a.locateFile(n,w):w+n}d?(w=p?e(622).dirname(w)+"/":__dirname+"/",v=function(n,r){return m||(m=e(747)),g||(g=e(622)),n=g.normalize(n),m.readFileSync(n,r?null:"utf8")},y=function(n){var r=v(n,!0);return r.buffer||(r=new Uint8Array(r)),E(r.buffer),r},process.argv.length>1&&process.argv[1].replace(/\\/g,"/"),c=process.argv.slice(2),process.on("uncaughtException",(function(n){if(!(n instanceof Mr))throw n})),process.on("unhandledRejection",hn),s=function(n){process.exit(n)},a.inspect=function(){return"[Emscripten Module object]"}):h?("undefined"!=typeof read&&(v=function(n){return read(n)}),y=function(n){var r;return"function"==typeof readbuffer?new Uint8Array(readbuffer(n)):(E("object"===o(r=read(n,"binary"))),r)},"undefined"!=typeof scriptArgs?c=scriptArgs:void 0!==arguments&&(c=arguments),"function"==typeof quit&&(s=function(n){quit(n)}),"undefined"!=typeof print&&("undefined"==typeof console&&(console={}),console.log=print,console.warn=console.error="undefined"!=typeof printErr?printErr:print)):(l||p)&&(p?w=self.location.href:"undefined"!=typeof document&&document.currentScript&&(w=document.currentScript.src),i&&(w=i),w=0!==w.indexOf("blob:")?w.substr(0,w.lastIndexOf("/")+1):"",v=function(n){var r=new XMLHttpRequest;return r.open("GET",n,!1),r.send(null),r.responseText},p&&(y=function(n){var r=new XMLHttpRequest;return r.open("GET",n,!1),r.responseType="arraybuffer",r.send(null),new Uint8Array(r.response)})),a.print||console.log.bind(console);var A,_,T,S=a.printErr||console.warn.bind(console);for(u in f)f.hasOwnProperty(u)&&(a[u]=f[u]);f=null,a.arguments&&(c=a.arguments),a.thisProgram&&a.thisProgram,a.quit&&(s=a.quit),a.wasmBinary&&(A=a.wasmBinary),a.noExitRuntime&&(_=a.noExitRuntime),"object"!==("undefined"==typeof WebAssembly?"undefined":o(WebAssembly))&&hn("no native wasm support detected");var C=!1;function E(n,r){n||hn("Assertion failed: "+r)}var R="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0;function P(n,r,e){for(var t=r+e,o=r;n[o]&&!(o>=t);)++o;if(o-r>16&&n.subarray&&R)return R.decode(n.subarray(r,o));for(var i="";r<o;){var a=n[r++];if(128&a){var u=63&n[r++];if(192!=(224&a)){var f=63&n[r++];if((a=224==(240&a)?(15&a)<<12|u<<6|f:(7&a)<<18|u<<12|f<<6|63&n[r++])<65536)i+=String.fromCharCode(a);else{var c=a-65536;i+=String.fromCharCode(55296|c>>10,56320|1023&c)}}else i+=String.fromCharCode((31&a)<<6|u)}else i+=String.fromCharCode(a)}return i}function k(n,r){return n?P(j,n,r):""}function W(n,r,e,t){if(!(t>0))return 0;for(var o=e,i=e+t-1,a=0;a<n.length;++a){var u=n.charCodeAt(a);if(u>=55296&&u<=57343&&(u=65536+((1023&u)<<10)|1023&n.charCodeAt(++a)),u<=127){if(e>=i)break;r[e++]=u}else if(u<=2047){if(e+1>=i)break;r[e++]=192|u>>6,r[e++]=128|63&u}else if(u<=65535){if(e+2>=i)break;r[e++]=224|u>>12,r[e++]=128|u>>6&63,r[e++]=128|63&u}else{if(e+3>=i)break;r[e++]=240|u>>18,r[e++]=128|u>>12&63,r[e++]=128|u>>6&63,r[e++]=128|63&u}}return r[e]=0,e-o}function x(n,r,e){return W(n,j,r,e)}function F(n){for(var r=0,e=0;e<n.length;++e){var t=n.charCodeAt(e);t>=55296&&t<=57343&&(t=65536+((1023&t)<<10)|1023&n.charCodeAt(++e)),t<=127?++r:r+=t<=2047?2:t<=65535?3:4}return r}var I,O,j,U,D,M,B,L,N,H="undefined"!=typeof TextDecoder?new TextDecoder("utf-16le"):void 0;function q(n,r){for(var e=n,t=e>>1,o=t+r/2;!(t>=o)&&D[t];)++t;if((e=t<<1)-n>32&&H)return H.decode(j.subarray(n,e));for(var i="",a=0;!(a>=r/2);++a){var u=U[n+2*a>>1];if(0==u)break;i+=String.fromCharCode(u)}return i}function V(n,r,e){if(void 0===e&&(e=2147483647),e<2)return 0;for(var t=r,o=(e-=2)<2*n.length?e/2:n.length,i=0;i<o;++i){var a=n.charCodeAt(i);U[r>>1]=a,r+=2}return U[r>>1]=0,r-t}function z(n){return 2*n.length}function Y(n,r){for(var e=0,t="";!(e>=r/4);){var o=M[n+4*e>>2];if(0==o)break;if(++e,o>=65536){var i=o-65536;t+=String.fromCharCode(55296|i>>10,56320|1023&i)}else t+=String.fromCharCode(o)}return t}function Q(n,r,e){if(void 0===e&&(e=2147483647),e<4)return 0;for(var t=r,o=t+e-4,i=0;i<n.length;++i){var a=n.charCodeAt(i);if(a>=55296&&a<=57343&&(a=65536+((1023&a)<<10)|1023&n.charCodeAt(++i)),M[r>>2]=a,(r+=4)+4>o)break}return M[r>>2]=0,r-t}function G(n){for(var r=0,e=0;e<n.length;++e){var t=n.charCodeAt(e);t>=55296&&t<=57343&&++e,r+=4}return r}function X(n,r){return n%r>0&&(n+=r-n%r),n}function Z(n){I=n,a.HEAP8=O=new Int8Array(n),a.HEAP16=U=new Int16Array(n),a.HEAP32=M=new Int32Array(n),a.HEAPU8=j=new Uint8Array(n),a.HEAPU16=D=new Uint16Array(n),a.HEAPU32=B=new Uint32Array(n),a.HEAPF32=L=new Float32Array(n),a.HEAPF64=N=new Float64Array(n)}a.INITIAL_MEMORY;var J,$=[],K=[],nn=[],rn=[];function en(){if(a.preRun)for("function"==typeof a.preRun&&(a.preRun=[a.preRun]);a.preRun.length;)un(a.preRun.shift());Sn($)}function tn(){Sn(K)}function on(){Sn(nn)}function an(){if(a.postRun)for("function"==typeof a.postRun&&(a.postRun=[a.postRun]);a.postRun.length;)fn(a.postRun.shift());Sn(rn)}function un(n){$.unshift(n)}function fn(n){rn.unshift(n)}var cn=0,sn=null,ln=null;function pn(n){cn++,a.monitorRunDependencies&&a.monitorRunDependencies(cn)}function dn(n){if(cn--,a.monitorRunDependencies&&a.monitorRunDependencies(cn),0==cn&&(null!==sn&&(clearInterval(sn),sn=null),ln)){var r=ln;ln=null,r()}}function hn(n){a.onAbort&&a.onAbort(n),S(n+=""),C=!0,n="abort("+n+"). Build with -s ASSERTIONS=1 for more info.";var r=new WebAssembly.RuntimeError(n);throw t(r),r}function vn(n,r){return String.prototype.startsWith?n.startsWith(r):0===n.indexOf(r)}a.preloadedImages={},a.preloadedAudios={};var yn="data:application/octet-stream;base64,";function mn(n){return vn(n,yn)}var gn="file://";function wn(n){return vn(n,gn)}var bn="glue.wasm";function An(n){try{if(n==bn&&A)return new Uint8Array(A);if(y)return y(n);throw"both async and sync fetching of the wasm failed"}catch(n){hn(n)}}function _n(){return A||!l&&!p||"function"!=typeof fetch||wn(bn)?Promise.resolve().then((function(){return An(bn)})):fetch(bn,{credentials:"same-origin"}).then((function(n){if(!n.ok)throw"failed to load wasm binary file at '"+bn+"'";return n.arrayBuffer()})).catch((function(){return An(bn)}))}function Tn(){var n={a:Ir};function r(n,r){var e=n.exports;a.asm=e,Z((T=a.asm.q).buffer),J=a.asm.r,dn()}function e(n){r(n.instance)}function o(r){return _n().then((function(r){return WebAssembly.instantiate(r,n)})).then(r,(function(n){S("failed to asynchronously prepare wasm: "+n),hn(n)}))}if(pn(),a.instantiateWasm)try{return a.instantiateWasm(n,r)}catch(n){return S("Module.instantiateWasm callback failed with error: "+n),!1}return(A||"function"!=typeof WebAssembly.instantiateStreaming||mn(bn)||wn(bn)||"function"!=typeof fetch?o(e):fetch(bn,{credentials:"same-origin"}).then((function(r){return WebAssembly.instantiateStreaming(r,n).then(e,(function(n){return S("wasm streaming compile failed: "+n),S("falling back to ArrayBuffer instantiation"),o(e)}))}))).catch(t),{}}function Sn(n){for(;n.length>0;){var r=n.shift();if("function"!=typeof r){var e=r.func;"number"==typeof e?void 0===r.arg?J.get(e)():J.get(e)(r.arg):e(void 0===r.arg?null:r.arg)}else r(a)}}function Cn(n,r,e,t){hn("Assertion failed: "+k(n)+", at: "+[r?k(r):"unknown filename",e,t?k(t):"unknown function"])}function En(n){switch(n){case 1:return 0;case 2:return 1;case 4:return 2;case 8:return 3;default:throw new TypeError("Unknown type size: "+n)}}function Rn(){for(var n=new Array(256),r=0;r<256;++r)n[r]=String.fromCharCode(r);Pn=n}mn(bn)||(bn=b(bn));var Pn=void 0;function kn(n){for(var r="",e=n;j[e];)r+=Pn[j[e++]];return r}var Wn={},xn={},Fn={},In=48,On=57;function jn(n){if(void 0===n)return"_unknown";var r=(n=n.replace(/[^a-zA-Z0-9_]/g,"$")).charCodeAt(0);return r>=In&&r<=On?"_"+n:n}function Un(n,r){return n=jn(n),new Function("body","return function "+n+'() {\n    "use strict";    return body.apply(this, arguments);\n};\n')(r)}function Dn(n,r){var e=Un(r,(function(n){this.name=r,this.message=n;var e=new Error(n).stack;void 0!==e&&(this.stack=this.toString()+"\n"+e.replace(/^Error(:[^\n]*)?\n/,""))}));return e.prototype=Object.create(n.prototype),e.prototype.constructor=e,e.prototype.toString=function(){return void 0===this.message?this.name:this.name+": "+this.message},e}var Mn=void 0;function Bn(n){throw new Mn(n)}var Ln=void 0;function Nn(n){throw new Ln(n)}function Hn(n,r,e){function t(r){var t=e(r);t.length!==n.length&&Nn("Mismatched type converter count");for(var o=0;o<n.length;++o)qn(n[o],t[o])}n.forEach((function(n){Fn[n]=r}));var o=new Array(r.length),i=[],a=0;r.forEach((function(n,r){xn.hasOwnProperty(n)?o[r]=xn[n]:(i.push(n),Wn.hasOwnProperty(n)||(Wn[n]=[]),Wn[n].push((function(){o[r]=xn[n],++a===i.length&&t(o)})))})),0===i.length&&t(o)}function qn(n,r,e){if(e=e||{},!("argPackAdvance"in r))throw new TypeError("registerType registeredInstance requires argPackAdvance");var t=r.name;if(n||Bn('type "'+t+'" must have a positive integer typeid pointer'),xn.hasOwnProperty(n)){if(e.ignoreDuplicateRegistrations)return;Bn("Cannot register type '"+t+"' twice")}if(xn[n]=r,delete Fn[n],Wn.hasOwnProperty(n)){var o=Wn[n];delete Wn[n],o.forEach((function(n){n()}))}}function Vn(n,r,e,t,o){var i=En(e);qn(n,{name:r=kn(r),fromWireType:function(n){return!!n},toWireType:function(n,r){return r?t:o},argPackAdvance:8,readValueFromPointer:function(n){var t;if(1===e)t=O;else if(2===e)t=U;else{if(4!==e)throw new TypeError("Unknown boolean type size: "+r);t=M}return this.fromWireType(t[n>>i])},destructorFunction:null})}var zn=[],Yn=[{},{value:void 0},{value:null},{value:!0},{value:!1}];function Qn(n){n>4&&0==--Yn[n].refcount&&(Yn[n]=void 0,zn.push(n))}function Gn(){for(var n=0,r=5;r<Yn.length;++r)void 0!==Yn[r]&&++n;return n}function Xn(){for(var n=5;n<Yn.length;++n)if(void 0!==Yn[n])return Yn[n];return null}function Zn(){a.count_emval_handles=Gn,a.get_first_emval=Xn}function Jn(n){switch(n){case void 0:return 1;case null:return 2;case!0:return 3;case!1:return 4;default:var r=zn.length?zn.pop():Yn.length;return Yn[r]={refcount:1,value:n},r}}function $n(n){return this.fromWireType(B[n>>2])}function Kn(n,r){qn(n,{name:r=kn(r),fromWireType:function(n){var r=Yn[n].value;return Qn(n),r},toWireType:function(n,r){return Jn(r)},argPackAdvance:8,readValueFromPointer:$n,destructorFunction:null})}function nr(n){if(null===n)return"null";var r=o(n);return"object"===r||"array"===r||"function"===r?n.toString():""+n}function rr(n,r){switch(r){case 2:return function(n){return this.fromWireType(L[n>>2])};case 3:return function(n){return this.fromWireType(N[n>>3])};default:throw new TypeError("Unknown float type: "+n)}}function er(n,r,e){var t=En(e);qn(n,{name:r=kn(r),fromWireType:function(n){return n},toWireType:function(n,r){if("number"!=typeof r&&"boolean"!=typeof r)throw new TypeError('Cannot convert "'+nr(r)+'" to '+this.name);return r},argPackAdvance:8,readValueFromPointer:rr(r,t),destructorFunction:null})}function tr(n,r){if(!(n instanceof Function))throw new TypeError("new_ called with constructor type "+o(n)+" which is not a function");var e=Un(n.name||"unknownFunctionName",(function(){}));e.prototype=n.prototype;var t=new e,i=n.apply(t,r);return i instanceof Object?i:t}function or(n){for(;n.length;){var r=n.pop();n.pop()(r)}}function ir(n,r,e,t,o){var i=r.length;i<2&&Bn("argTypes array size mismatch! Must at least get return value and 'this' types!");for(var a=null!==r[1]&&null!==e,u=!1,f=1;f<r.length;++f)if(null!==r[f]&&void 0===r[f].destructorFunction){u=!0;break}var c="void"!==r[0].name,s="",l="";for(f=0;f<i-2;++f)s+=(0!==f?", ":"")+"arg"+f,l+=(0!==f?", ":"")+"arg"+f+"Wired";var p="return function "+jn(n)+"("+s+") {\nif (arguments.length !== "+(i-2)+") {\nthrowBindingError('function "+n+" called with ' + arguments.length + ' arguments, expected "+(i-2)+" args!');\n}\n";u&&(p+="var destructors = [];\n");var d=u?"destructors":"null",h=["throwBindingError","invoker","fn","runDestructors","retType","classParam"],v=[Bn,t,o,or,r[0],r[1]];for(a&&(p+="var thisWired = classParam.toWireType("+d+", this);\n"),f=0;f<i-2;++f)p+="var arg"+f+"Wired = argType"+f+".toWireType("+d+", arg"+f+"); // "+r[f+2].name+"\n",h.push("argType"+f),v.push(r[f+2]);if(a&&(l="thisWired"+(l.length>0?", ":"")+l),p+=(c?"var rv = ":"")+"invoker(fn"+(l.length>0?", ":"")+l+");\n",u)p+="runDestructors(destructors);\n";else for(f=a?1:2;f<r.length;++f){var y=1===f?"thisWired":"arg"+(f-2)+"Wired";null!==r[f].destructorFunction&&(p+=y+"_dtor("+y+"); // "+r[f].name+"\n",h.push(y+"_dtor"),v.push(r[f].destructorFunction))}return c&&(p+="var ret = retType.fromWireType(rv);\nreturn ret;\n"),p+="}\n",h.push(p),tr(Function,h).apply(null,v)}function ar(n,r,e){if(void 0===n[r].overloadTable){var t=n[r];n[r]=function(){return n[r].overloadTable.hasOwnProperty(arguments.length)||Bn("Function '"+e+"' called with an invalid number of arguments ("+arguments.length+") - expects one of ("+n[r].overloadTable+")!"),n[r].overloadTable[arguments.length].apply(this,arguments)},n[r].overloadTable=[],n[r].overloadTable[t.argCount]=t}}function ur(n,r,e){a.hasOwnProperty(n)?((void 0===e||void 0!==a[n].overloadTable&&void 0!==a[n].overloadTable[e])&&Bn("Cannot register public name '"+n+"' twice"),ar(a,n,n),a.hasOwnProperty(e)&&Bn("Cannot register multiple overloads of a function with the same number of arguments ("+e+")!"),a[n].overloadTable[e]=r):(a[n]=r,void 0!==e&&(a[n].numArguments=e))}function fr(n,r){for(var e=[],t=0;t<n;t++)e.push(M[(r>>2)+t]);return e}function cr(n,r,e){a.hasOwnProperty(n)||Nn("Replacing nonexistant public symbol"),void 0!==a[n].overloadTable&&void 0!==e?a[n].overloadTable[e]=r:(a[n]=r,a[n].argCount=e)}function sr(n,r,e){return e&&e.length?a["dynCall_"+n].apply(null,[r].concat(e)):a["dynCall_"+n].call(null,r)}function lr(n,r,e){return-1!=n.indexOf("j")?sr(n,r,e):J.get(r).apply(null,e)}function pr(n,r){E(n.indexOf("j")>=0,"getDynCaller should only be called with i64 sigs");var e=[];return function(){e.length=arguments.length;for(var t=0;t<arguments.length;t++)e[t]=arguments[t];return lr(n,r,e)}}function dr(n,r){var e=-1!=(n=kn(n)).indexOf("j")?pr(n,r):J.get(r);return"function"!=typeof e&&Bn("unknown function pointer with signature "+n+": "+r),e}var hr=void 0;function vr(n){var r=Dr(n),e=kn(r);return jr(r),e}function yr(n,r){var e=[],t={};throw r.forEach((function n(r){t[r]||xn[r]||(Fn[r]?Fn[r].forEach(n):(e.push(r),t[r]=!0))})),new hr(n+": "+e.map(vr).join([", "]))}function mr(n,r,e,t,o,i){var a=fr(r,e);n=kn(n),o=dr(t,o),ur(n,(function(){yr("Cannot call "+n+" due to unbound types",a)}),r-1),Hn([],a,(function(e){var t=[e[0],null].concat(e.slice(1));return cr(n,ir(n,t,null,o,i),r-1),[]}))}function gr(n,r,e){switch(r){case 0:return e?function(n){return O[n]}:function(n){return j[n]};case 1:return e?function(n){return U[n>>1]}:function(n){return D[n>>1]};case 2:return e?function(n){return M[n>>2]}:function(n){return B[n>>2]};default:throw new TypeError("Unknown integer type: "+n)}}function wr(n,r,e,t,o){r=kn(r),-1===o&&(o=4294967295);var i=En(e),a=function(n){return n};if(0===t){var u=32-8*e;a=function(n){return n<<u>>>u}}var f=-1!=r.indexOf("unsigned");qn(n,{name:r,fromWireType:a,toWireType:function(n,e){if("number"!=typeof e&&"boolean"!=typeof e)throw new TypeError('Cannot convert "'+nr(e)+'" to '+this.name);if(e<t||e>o)throw new TypeError('Passing a number "'+nr(e)+'" from JS side to C/C++ side to an argument of type "'+r+'", which is outside the valid range ['+t+", "+o+"]!");return f?e>>>0:0|e},argPackAdvance:8,readValueFromPointer:gr(r,i,0!==t),destructorFunction:null})}function br(n,r,e){var t=[Int8Array,Uint8Array,Int16Array,Uint16Array,Int32Array,Uint32Array,Float32Array,Float64Array][r];function o(n){var r=B,e=r[n>>=2],o=r[n+1];return new t(I,o,e)}qn(n,{name:e=kn(e),fromWireType:o,argPackAdvance:8,readValueFromPointer:o},{ignoreDuplicateRegistrations:!0})}function Ar(n,r){var e="std::string"===(r=kn(r));qn(n,{name:r,fromWireType:function(n){var r,t=B[n>>2];if(e)for(var o=n+4,i=0;i<=t;++i){var a=n+4+i;if(i==t||0==j[a]){var u=k(o,a-o);void 0===r?r=u:(r+=String.fromCharCode(0),r+=u),o=a+1}}else{var f=new Array(t);for(i=0;i<t;++i)f[i]=String.fromCharCode(j[n+4+i]);r=f.join("")}return jr(n),r},toWireType:function(n,r){r instanceof ArrayBuffer&&(r=new Uint8Array(r));var t="string"==typeof r;t||r instanceof Uint8Array||r instanceof Uint8ClampedArray||r instanceof Int8Array||Bn("Cannot pass non-string to std::string");var o=(e&&t?function(){return F(r)}:function(){return r.length})(),i=Ur(4+o+1);if(B[i>>2]=o,e&&t)x(r,i+4,o+1);else if(t)for(var a=0;a<o;++a){var u=r.charCodeAt(a);u>255&&(jr(i),Bn("String has UTF-16 code units that do not fit in 8 bits")),j[i+4+a]=u}else for(a=0;a<o;++a)j[i+4+a]=r[a];return null!==n&&n.push(jr,i),i},argPackAdvance:8,readValueFromPointer:$n,destructorFunction:function(n){jr(n)}})}function _r(n,r,e){var t,o,i,a,u;e=kn(e),2===r?(t=q,o=V,a=z,i=function(){return D},u=1):4===r&&(t=Y,o=Q,a=G,i=function(){return B},u=2),qn(n,{name:e,fromWireType:function(n){for(var e,o=B[n>>2],a=i(),f=n+4,c=0;c<=o;++c){var s=n+4+c*r;if(c==o||0==a[s>>u]){var l=t(f,s-f);void 0===e?e=l:(e+=String.fromCharCode(0),e+=l),f=s+r}}return jr(n),e},toWireType:function(n,t){"string"!=typeof t&&Bn("Cannot pass non-string to C++ string type "+e);var i=a(t),f=Ur(4+i+r);return B[f>>2]=i>>u,o(t,f+4,i+r),null!==n&&n.push(jr,f),f},argPackAdvance:8,readValueFromPointer:$n,destructorFunction:function(n){jr(n)}})}function Tr(n,r){qn(n,{isVoid:!0,name:r=kn(r),argPackAdvance:0,fromWireType:function(){},toWireType:function(n,r){}})}function Sr(n){n>4&&(Yn[n].refcount+=1)}function Cr(n,r){var e=xn[n];return void 0===e&&Bn(r+" has unknown type "+vr(n)),e}function Er(n,r){return Jn((n=Cr(n,"_emval_take_value")).readValueFromPointer(r))}function Rr(){hn()}function Pr(n,r,e){j.copyWithin(n,r,r+e)}function kr(){return j.length}function Wr(n){try{return T.grow(n-I.byteLength+65535>>>16),Z(T.buffer),1}catch(n){}}function xr(n){n>>>=0;var r=kr(),e=2147483648;if(n>e)return!1;for(var t=1;t<=4;t*=2){var o=r*(1+.2/t);if(o=Math.min(o,n+100663296),Wr(Math.min(e,X(Math.max(16777216,n,o),65536))))return!0}return!1}Rn(),Mn=a.BindingError=Dn(Error,"BindingError"),Ln=a.InternalError=Dn(Error,"InternalError"),Zn(),hr=a.UnboundTypeError=Dn(Error,"UnboundTypeError"),K.push({func:function(){Or()}});var Fr,Ir={a:Cn,k:Vn,j:Kn,h:er,d:mr,c:wr,b:br,f:Ar,e:_r,l:Tr,m:Qn,n:Sr,i:Er,g:Rr,o:Pr,p:xr},Or=(Tn(),a.___wasm_call_ctors=function(){return(Or=a.___wasm_call_ctors=a.asm.s).apply(null,arguments)}),jr=(a._main=function(){return(a._main=a.asm.t).apply(null,arguments)},a._free=function(){return(jr=a._free=a.asm.u).apply(null,arguments)}),Ur=a._malloc=function(){return(Ur=a._malloc=a.asm.v).apply(null,arguments)},Dr=a.___getTypeName=function(){return(Dr=a.___getTypeName=a.asm.w).apply(null,arguments)};function Mr(n){this.name="ExitStatus",this.message="Program terminated with exit("+n+")",this.status=n}function Br(n){var r=a._main;try{Nr(r(0,0),!0)}catch(n){if(n instanceof Mr)return;if("unwind"==n)return void(_=!0);var e=n;n&&"object"===o(n)&&n.stack&&(e=[n,n.stack]),S("exception thrown: "+e),s(1,n)}}function Lr(n){function e(){Fr||(Fr=!0,a.calledRun=!0,C||(tn(),on(),r(a),a.onRuntimeInitialized&&a.onRuntimeInitialized(),Hr&&Br(),an()))}n=n||c,cn>0||(en(),cn>0||(a.setStatus?(a.setStatus("Running..."),setTimeout((function(){setTimeout((function(){a.setStatus("")}),1),e()}),1)):e()))}function Nr(n,r){r&&_&&0===n||(_||(a.onExit&&a.onExit(n),C=!0),s(n,new Mr(n)))}if(a.___embind_register_native_and_builtin_types=function(){return(a.___embind_register_native_and_builtin_types=a.asm.x).apply(null,arguments)},ln=function n(){Fr||Lr(),Fr||(ln=n)},a.run=Lr,a.preInit)for("function"==typeof a.preInit&&(a.preInit=[a.preInit]);a.preInit.length>0;)a.preInit.pop()();var Hr=!0;return a.noInitialRun&&(Hr=!1),_=!0,Lr(),n.ready});"object"===o(r)&&"object"===o(n)?n.exports=a:void 0===(t=function(){return a}.apply(r,[]))||(n.exports=t)},498:(n,r,e)=>{"use strict";e.r(r),e.d(r,{ConverterType:()=>c,create:()=>s});var t=e(17),o=e.n(t);function i(n,r){for(var e=arguments.length>2&&void 0!==arguments[2]?arguments[2]:null,t=null===e?new Float32Array(n):e,o=0;o<n;o++)t[o]=r[o];return t}function a(n,r){for(var e=0;e<r.length;e++){var t=r[e];t.enumerable=t.enumerable||!1,t.configurable=!0,"value"in t&&(t.writable=!0),Object.defineProperty(n,t.key,t)}}var u=1008e3,f=function(){function n(r,e,t,o,i){!function(n,r){if(!(n instanceof r))throw new TypeError("Cannot call a class as a function")}(this,n),this.module=r,this.converterType=e,this.nChannels=t,this.inputSampleRate=o,this.outputSampleRate=i,this.ratio=i/o,this.isDestroyed=!1,r.init(t,e,o,i),this.sourceArray=r.sourceArray(u),this.targetArray=r.targetArray(u)}var r,e;return r=n,(e=[{key:"simple",value:function(n){return this._resample(this.module.simple,n)}},{key:"full",value:function(n){var r=arguments.length>1&&void 0!==arguments[1]?arguments[1]:null;return this._resample(this.module.full,n,r)}},{key:"destroy",value:function(){!0===this.isDestroyed?console.warn("destroy() has already been called on this instance"):(this.module.destroy(),this.isDestroyed=!0)}},{key:"_chunkAndResample",value:function(n){for(var r=0,e=[],t=function(n,r,e){for(var t=0,o=[],i=0;i<n.length;i+=r){var a=Math.min(r,n.length-t),u=new e(n.buffer,t*n.BYTES_PER_ELEMENT,a);t+=r,o.push(u)}return o}(n,this.inputSampleRate/10*this.nChannels,Float32Array),o=0;o<t.length;o++){var i=this._resample(this.module.full,t[o]);r+=i.length,e.push(i)}for(var a=new Float32Array(r),u=0,f=0;f<e.length;f++)for(var c=0;c<e[f].length;c++)a[u++]=e[f][c];return a}},{key:"_resample",value:function(n,r){var e=arguments.length>2&&void 0!==arguments[2]?arguments[2]:null;if(this.inputSampleRate===this.outputSampleRate)return r;if(null!==e&&e.length<this.ratio*r.length)throw"dataOut must be at least ceil(srcRatio * dataIn.length) samples long";var t=Math.ceil(r.length*this.ratio);if(t>u)return this._chunkAndResample(r);this.sourceArray.set(r);var o=n(r.length,this.nChannels,this.converterType,this.inputSampleRate,this.outputSampleRate);return i(o*this.nChannels,this.targetArray,e)}}])&&a(r.prototype,e),n}(),c={SRC_SINC_BEST_QUALITY:0,SRC_SINC_MEDIUM_QUALITY:1,SRC_SINC_FASTEST:2,SRC_ZERO_ORDER_HOLD:3,SRC_LINEAR:4};function s(n,r,e){var t=arguments.length>3&&void 0!==arguments[3]?arguments[3]:{},i=void 0===t.converterType?c.SRC_SINC_FASTEST:t.converterType,a=t.wasmPath||"/libsamplerate.wasm";l(n,r,e,i);var u={locateFile:function(){return a}};return new Promise((function(t,a){o()(u).then((function(o){var a=new f(o,i,n,r,e);t(a)})).catch((function(n){a(n)}))}))}function l(n,r,e,t){if(void 0===n)throw"nChannels is undefined";if(void 0===r)throw"inputSampleRate is undefined";if(void 0===e)throw"outputSampleRate is undefined";if(n<1||n>128)throw"invalid nChannels submitted";if(r<1||r>192e3)throw"invalid inputSampleRate";if(e<1||e>192e3)throw"invalid outputSampleRate";if(t<c.SRC_SINC_BEST_QUALITY||t>c.SRC_LINEAR)throw"invalid converterType"}},747:n=>{"use strict";n.exports=require("fs")},622:n=>{"use strict";n.exports=require("path")}},r={};function e(t){if(r[t])return r[t].exports;var o=r[t]={id:t,loaded:!1,exports:{}};return n[t](o,o.exports,e),o.loaded=!0,o.exports}return e.n=n=>{var r=n&&n.__esModule?()=>n.default:()=>n;return e.d(r,{a:r}),r},e.d=(n,r)=>{for(var t in r)e.o(r,t)&&!e.o(n,t)&&Object.defineProperty(n,t,{enumerable:!0,get:r[t]})},e.o=(n,r)=>Object.prototype.hasOwnProperty.call(n,r),e.r=n=>{"undefined"!=typeof Symbol&&Symbol.toStringTag&&Object.defineProperty(n,Symbol.toStringTag,{value:"Module"}),Object.defineProperty(n,"__esModule",{value:!0})},e.nmd=n=>(n.paths=[],n.children||(n.children=[]),n),e(498)})()}));