var ACHistory={debug:false,initialize:function(){if(ACUtil.readCookie("ac_history")==null){var a={search:[],kb:[],help:[],psp:[],offer_reason:{},total_count:{}};ACHistory.saveHistory(a)}},addKbView:function(e,f,a,d){var c=ACHistory.getHistory();var b=ACHistory.getEpochUTC();var f=f.substring(0,99);if(c.kb.length>=5){c.kb.pop()}if(!c.total_count.kbs){c.total_count.kbs=0}c.total_count.kbs=c.total_count.kbs+1;c.total_count.last_kb=b;c.kb.unshift([e,f,a,b,d]);ACHistory.saveHistory(c);if(typeof console!="undefined"&&ACHistory.debug==true){console.log(c.kb);console.log("# kbs: "+c.total_count.kbs)}},addHelpView:function(b,c){var a=ACHistory.getHistory();if(a.help.length>=5){a.help.pop()}a.help.unshift([b,ACHistory.getEpochUTC(),c]);ACHistory.saveHistory(a);if(typeof console!="undefined"&&ACHistory.debug==true){console.log(a.help)}},addPspView:function(a,c){var b=ACHistory.getHistory();if(b.psp.length>=5){b.psp.pop()}b.psp.unshift([a,ACHistory.getEpochUTC()]);ACHistory.saveHistory(b);if(typeof console!="undefined"&&ACHistory.debug==true){console.log(b.psp)}},addSearch:function(d,a){var c=ACHistory.getHistory();var b=ACHistory.getEpochUTC();var d=d.substring(0,99);if(!c.total_count.searches){c.total_count.searches=0}c.total_count.searches=c.total_count.searches+1;c.total_count.last_search=b;if(c.search.length>=5){c.search.pop()}c.search.unshift([d,a,b]);ACHistory.saveHistory(c);if(typeof console!="undefined"&&ACHistory.debug==true){console.log(c.search);console.log("# queries: "+c.total_count.searches)}},addOfferReason:function(a){var c=ACHistory.getHistory();var e="";var d="";var b=window.location.href;c.offer_reason.url=b;c.offer_reason.locale=a;c.offer_reason.timestamp=ACHistory.getEpochUTC();ACHistory.saveHistory(c);if(typeof console!="undefined"&&ACHistory.debug==true){console.log(c.offer_reason)}var f=document.forms.postOfferForm;f.action="https://getsupport.apple.com";f.method="POST";f.submit()},getEpochUTC:function(){var a=Date.parse(new Date().toGMTString());return a},saveHistory:function(a){var b=Object.toJSON(a);b=b.replace(/" /g,'"');b=b.replace(/\": /g,'":');b=b.replace(/\, \"/g,',"');ACUtil.writeCookie("ac_history",b,true)},getHistory:function(){var a=ACUtil.readCookie("ac_history");return a.evalJSON(true)}};ACHistory.initialize();