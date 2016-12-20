
function getLineEndForLine(text, lineNumber) {
    var position = getPositionOfOccurence(text, "\n", lineNumber+1);
    // Is last line.
    if (position === 0) { position = text.length; }
    return getLineEndForPosition(text, position);
}

// Returns the position of the end of the line (i.e. the next character would be the newline character)
// relative to the entire text.
function getLineEndForPosition(text, position) {
    var lineEnd = textArea.text.slice(position).indexOf("\n") + position;
    if (lineEnd == -1) { lineEnd = text.length; }
    return lineEnd;
}

// Returns the position of the ``n``-th occurence of the string ``searchString`` inside ``text``.
function getPositionOfOccurence(string, searchString, n) {
    return string.split(searchString, n).join(searchString).length;
}

