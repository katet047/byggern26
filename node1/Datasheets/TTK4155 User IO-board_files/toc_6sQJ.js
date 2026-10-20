
// Adapted from https://github.com/JanLoebel/showdown-toc
function toc() {

  function getHeaderEntries(sourceHtml) {
    // Generate dummy element
    var source = document.createElement('div');
    source.innerHTML = sourceHtml;

    // Find headers
    var headers = source.querySelectorAll('h3, h4, h5, h6');
    var headerList = [];
    for (var i = 0; i < headers.length; i++) {
      var el = headers[i];
      headerList.push(new TocEntry(el.tagName, el.textContent, el.id));
    }

    return headerList;
  }

  function TocEntry(tagName, text, anchor) {
    this.tagName = tagName;
    this.text = text;
    this.anchor = anchor;
    this.children = [];
  }

  TocEntry.prototype.childrenToString = function() {
    if (this.children.length === 0) {
      return "";
    }
    var result = "<ul>\n";
    for (var i = 0; i < this.children.length; i++) {
      result += this.children[i].toString();
    }
    result += "</ul>\n";
    return result;
  };

  TocEntry.prototype.toString = function() {
    var result = "<li>";
    if (this.text) {
      result += "<a href=\"#" + this.anchor + "\">" + this.text + "</a>";
    }
    result += this.childrenToString();
    result += "</li>\n";
    return result;
  };

  function sortHeader(tocEntries, level) {
    level = level || 1;
    var tagName = "H" + (level+2),
      result = [],
      currentTocEntry;

    function push(tocEntry) {
      if (tocEntry !== undefined) {
        if (tocEntry.children.length > 0) {
          tocEntry.children = sortHeader(tocEntry.children, level + 1);
        }
        result.push(tocEntry);
      }
    }

    for (var i = 0; i < tocEntries.length; i++) {
      var tocEntry = tocEntries[i];
      if (tocEntry.tagName.toUpperCase() !== tagName) {
        if (currentTocEntry === undefined) {
          currentTocEntry = new TocEntry();
        }
        currentTocEntry.children.push(tocEntry);
      } else {
        push(currentTocEntry);
        currentTocEntry = tocEntry;
      }
    }

    push(currentTocEntry);
    return result;
  }

  return {
    type: 'output',
    filter: function(sourceHtml) {
      var headerList = getHeaderEntries(sourceHtml);

      // No header found
      if (headerList.length === 0) {
        return sourceHtml;
      }

      // Sort header
      headerList = sortHeader(headerList);

      // Build result and replace all [toc]
      var result = '<div class="toc">\n<ul>\n' + headerList.join("") + '</ul>\n</div>\n';
      return sourceHtml.replace(/\[toc\]/gi, result);
    }
  };
}