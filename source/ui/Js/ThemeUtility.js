// Factory function that returns a function that conditionally returns a style.
//
// This function takes a base scope in an object hierarchy (typically the Theme)
// and a property to watch (such as whether or not a checkbox is checked) and
// the name of a scope direclty beneath the base scope. It then returns a
// function that takes a dot-delimtied nested-scope name (e.g. 'foo.bar.baz'
// means foo[bar][baz]) whose base will either be the given base scope `base`,
// or `base[alternativeScope]`, if the `determiningProperty` evaluates to true.
//
// \param base The base scope object.
// \param determiningProperty A boolean(-convertible) variable.
// \param alternativeScope The name of a scope directly beneath `base`.
// \returns A new function, taking only a single string parameter.
function dynamicThemeFactory(base, determiningProperty, alternativeScope) {
  // All the parameters given to this function remain
  // in the closure of the returned function.
  return function(children) {
    var node = determiningProperty ? base[alternativeScope] : base;

    children = children.split('.');
    for (var index = 0; index < children.length; ++index) {
      node = node[children[index]];
    }

    return node;
  };
}
