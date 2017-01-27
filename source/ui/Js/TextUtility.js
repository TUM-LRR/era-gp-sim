
function getLineStartForLine(text, lineNumber) {
    var position = getPositionOfOccurence(text, "\n", lineNumber+1);
    return getLineStartForPosition(text, position);
}

function getLineEndForLine(text, lineNumber) {
    var position = getPositionOfOccurence(text, "\n", lineNumber+1);
    // Is last line.
    if (position === 0) position = text.length;
    return getLineEndForPosition(text, position);
}

// Returns the number of the line the given position belongs to.
function getLineNumberForPosition(text, position) {
    return numberOfOccurences(text.slice(0, position), "\n");
}

// Returns the line text of the line the given position belongs to.
function getLineForPosition(text, position) {
    var lineStart = getLineStartForPosition(text, position);
    var lineEnd = getLineEndForPosition(text, position);
    return text.slice(lineStart, lineEnd);
}

// Returns the position of the start of the line relative to the entire editor text.
function getLineStartForPosition(text, position) {
    var lineStart = text.slice(0, position).lastIndexOf("\n") + 1;
    if (lineStart === -1) { lineStart = 0; }
    return lineStart;
}

// Returns the position of the end of the line (i.e. the next character would be the newline character)
// relative to the entire  text.
function getLineEndForPosition(text, position) {
    var lineEnd = text.slice(position).indexOf("\n") + position;
    return (lineEnd === -1) ? text.length : lineEnd;
}

// Converts a given position relative to the entire text to a position relative to the line of the
// given position.
function getPositionRelativeToLineForPosition(text, position) {
    var lineStart = getLineStartForPosition(text, position);
    return position - lineStart;
}

// Returns the position of the ``n``-th occurence of the string ``searchString`` inside ``text``.
function getPositionOfOccurence(string, searchString, n) {
    return string.split(searchString, n).join(searchString).length;
}

// Returns a string with the substring beginning at a (inclusive) and ending at b (exclusive) removed from text.
function remove(string, a, b) {
    return string.slice(0, a) + string.slice(b, string.length);
}

// Returns the number of occurences of a given searchString inside a given text.
function numberOfOccurences(string, searchString) {
    if (typeof string !== "string" || typeof searchString !== "string") {
        return 0;
    }
    if (searchString.length <= 0) return string.length + 1;

    var index = -1;
    var offset = 0;
    var count = 0;
    while ((index = string.indexOf(searchString, offset)) !== -1) {
        count++;
        offset = index + searchString.length;
    }
    return count;
}

// Converts a given dec/hex/bin-string to an integer.
function convertStringToInteger(input) {
    var base = 10;
    if (input.indexOf("0x") === 0) {
        base = 16;
        input = input.slice(2);
    } else if (input.indexOf("0b") === 0) {
        input = input.slice(2);
        base = 2;
    }
    return parseInt(input, base);
}
