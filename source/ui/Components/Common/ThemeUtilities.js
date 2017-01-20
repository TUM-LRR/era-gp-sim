
// Returns a style depending on a boolean condition. For example,
// dynamicStyle('foo.bar') will return Theme.checkbox.active.foo.bar when
// the checkbox is active, and Theme.checkbox.foo.bar otherwise.
function dynamicStyle(children) {
  var node = checkbox.checked ? Theme.checkbox.active : Theme.checkbox;

  children = children.split('.');
  for (var index = 0; index < children.length; ++index) {
    node = node[children[index]];
  }

  return node;
}
