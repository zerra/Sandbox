var prevMatchers = new Array(
  new LinkHrefMacher("/prev$"),                     // goo blog
  new LinkHrefMacher("&direction=prev&"),           // mixi
  new LinkParentNodeMatcher("前の記事<br>"),        // bogus news
  new LinkParentClassMatcher("forwardEntry"),       // yahoo jp blog
  new LinkInnerHTMLMacher("img src.*back.gif"),     // coco blog
  new LinkRelMatcher("prev"),                       // livedoor blog
  new LinkInnerHTMLMacher("&lt;&lt; 前へ"),         // rakuten blog
  new LinkClassNameMatcher("blog-pager-older-link"),// blogger
  new LinkClassNameMatcher("nextPage"),             // ameba blog
  null
)
var nextMatchers = new Array(
  new LinkHrefMacher("/next$"),                     // goo blog
  new LinkHrefMacher("&direction=next&"),           // mixi
  new LinkInnerHTMLMacher("img src.*next.gif"),     // coco blog
  new LinkParentNodeMatcher("次の記事<br>"),        // bogus news
  new LinkParentClassMatcher("nextEntry"),          // yahoo jp blog
  new LinkRelMatcher("next"),                       // livedoor blog
  new LinkInnerHTMLMacher("次へ &gt;&gt;"),         // rakuten blog
  new LinkClassNameMatcher("blog-pager-newer-link"),// blogger
  new LinkClassNameMatcher("previousPage"),         // ameba blog
  null
)

function LinkHrefMacher(pattern) {
  this.match = function(link) {
    var re = new RegExp(pattern);
    return (re.exec(link.href) != null);
  };
}
function LinkInnerHTMLMacher(pattern) {
  this.match = function(link) {
    var re = new RegExp(pattern);
    return (re.exec(link.innerHTML) != null);
  };
}
function LinkParentNodeMatcher(pattern) {
  this.match = function(link) {
    var re = new RegExp(pattern);
    return (re.exec(link.parentNode.innerHTML) != null);
  };
}
function LinkParentClassMatcher(pattern) {
  this.match = function(link) {
    var re = new RegExp(pattern);
    return (re.exec(link.parentNode.className) != null);
  };
}
function LinkIdMatcher(pattern) {
  this.match = function(link) {
    var re = new RegExp(pattern);
    return (re.exec(link.id) != null);
  };
}
function LinkClassNameMatcher(pattern) {
  this.match = function(link) {
    var re = new RegExp(pattern);
    return (re.exec(link.className) != null);
  };
}
function LinkRelMatcher(pattern) {
  this.match = function(link) {
    var re = new RegExp(pattern);
    return (re.exec(link.rel) != null);
  };
}

function openMatchedLink(matchers, debug) {
  links = document.getElementsByTagName("a");

  for (i = 0; i < matchers.length; i++) {
    if (matchers[i] == null) {
      continue;
    }
    for (j = 0; j < links.length; j++) {
      link = links[j];
      if (link.href == "") {
        continue;
      }
      if (matchers[i].match(link)) {
        if (debug) {
          alert("link.href=" + link.href);
          alert("link.innerHTML=" + link.innerHTML);
          alert("link.id=" + link.id);
          alert("link.className=" + link.className);
        } else {
          window.location.href = link.href;
        }
      }
    }
  }
}

function goPrev(doc) {
  openMatchedLink(prevMatchers, false);
}
function goNext(doc) {
  openMatchedLink(nextMatchers, false);
}
