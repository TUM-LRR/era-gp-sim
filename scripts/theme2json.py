#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import collections
import enum
import functools
import json
import logging
import os.path
import re
import subprocess
import shlex
import sys
import time


###############################################################################
# LOGGING
###############################################################################


class ColorFormatter(logging.Formatter):
    """A formatter class that adds colors to logs."""

    COLORS = dict(ERROR=31, WARNING=33, INFO=32, DEBUG=34)

    def __init__(self, format_string, use_color=True):
        """
        Constructs a new ColorFormatter.

        Args:
            format_string: (str) The format string with which
                                 to display a message.
            use_color: (bool) Whether to enable coloring.
        """
        super(ColorFormatter, self).__init__(format_string)
        self.use_color = use_color

    def format(self, record):
        """
        Formats a record.

        A color is chosen suitable for the log level of the record.

        Args:
            record: (logging.Record) The log record to format.

        Returns:
            A formatted log record.
        """
        if self.use_color:
            record.levelname = '\033[{0}m{1}\033[0m'.format(
                ColorFormatter.COLORS[record.levelname],
                record.levelname
            )

        return super(ColorFormatter, self).format(record)


log_handler = logging.StreamHandler()
log_handler.setFormatter(ColorFormatter('%(levelname)s %(message)s'))
log = logging.getLogger(__name__)
log.addHandler(log_handler)

###############################################################################
# UTILITY FUNCTIONS
###############################################################################


def attribute(**kwargs):
    """
    Function decorator to give a function an attribute.

    Args:
        kwargs: Any key=value pairs to add to the wrapped function.

    Returns:
        A new function, whose `__dict__` has the members specified in `kwargs`.
    """
    def inner_attribute(function):
        @functools.wraps(function)
        def function_with_attribute(*args, **kwargs):
            return function(*args, **kwargs)
        function_with_attribute.__dict__.update(kwargs)
        return function_with_attribute
    return inner_attribute


def pattern(regex):
    """
    Adds a compiled regular expression as an attribute to a function.

    Args:
        regex: (str) The regular expression to compile and add to the function.

    Returns:
        A new function, with the given pattern
        compiled and added to the function.
    """
    return attribute(pattern=re.compile(regex))


def time_operation(operation):
    """
    Measures the execution time of an operation.

    Args:
        operation: (function) An operation to execute.

    Returns:
        A pair consisting of the running time of the operation in seconds
        and the return value of the operation.
    """
    start_time = time.time()
    return_value = operation()
    duration = time.time() - start_time

    return duration, return_value


###############################################################################
# PARSER
###############################################################################


@enum.unique
class Kind(enum.Enum):
    """The kind of file this script is processing."""
    CSS = 0
    SASS = 1
    DIRECTORY = 2


class ParseError(Exception):
    """Any error ocurred during parsing of command line arguments."""
    pass


class Parser(argparse.ArgumentParser):
    """Parses and preprocessed command line arguments."""

    Arguments = collections.namedtuple('Arguments', 'input, kind, output')

    def __init__(self):
        """Constructs a new parser for theme2json."""

        super(Parser, self).__init__(description='Theme to JSON converter.')

        self.add_argument(
            'theme',
            metavar='THEME',
            help='The theme folder, SASS or CSS to convert.'
        )

        self.add_argument(
            '-v',
            '--verbose',
            action='count',
            help="Give more information about the program's proceedings"
        )

        self.add_argument(
            '-o',
            '--output',
            metavar='FILE',
            help='Stores the generated JSON to FILE'
        )

        group = self.add_mutually_exclusive_group()
        self._setup_kind_options(group)

    def parse(self, arguments):
        """
        Parses the arguments supplied.

        Args:
            arguments: (list) The command line arguments to parse.

        Returns:
            A Parser.Arguments tuple containing the relevant input arguments.

        Raises:
            Parser.Error for any command line badness.
        """
        arguments = self.parse_args()
        argument_kind = self._determine_argument_kind(arguments)
        self._check_path(arguments.theme, argument_kind)

        if arguments.verbose:
            self._enable_logging(arguments.verbose)

        return Parser.Arguments(
            arguments.theme,
            argument_kind,
            arguments.output
        )

    def _setup_kind_options(self, group):
        """
        Sets up the choice arguments specifying the argument type passed.

        Args:
            group: The mutually exclusive group to add the choices to.
        """
        group.add_argument('-c', '--css', action='store_true',
                           help="THEME is the theme's CSS file.")
        group.add_argument('-s', '--sass', action='store_true',
                           help="THEME is the theme's SASS file.")
        group.add_argument('-d', '--directory', action='store_true',
                           help="THEME is the theme's directory.")

    def _check_path(self, path, argument_kind):
        """
        Check the validity of the input path supplied.

        SASS and CSS kinds must be files, Themes must be directories. Either
        way the path must exist.

        Args:
            path: (str) The input path to check.
            argument_kind: (Kind) The kind of path it is supposed to be.

        Raises:
            ParseError: If the input path is not valid.
        """
        if not os.path.exists(path):
            raise ParseError("Path '{0}' does not exist".format(path))
        if argument_kind is Kind.DIRECTORY and not os.path.isdir(path):
            raise ParseError(
                "Theme path '{0}' is not a directory".format(path)
            )

    def _determine_argument_kind(self, arguments):
        """
        Determines the kind of the input file.

        It is first tried to detect the kind from the input options.
        If no suitable input option (--css, --sass, --theme) was supplied the
        file extension is inspected.

        Args:
            arguments: (dict) The input arguments to inspect.

        Returns:
            A member of the Kind enum.

        Raises:
            ParseError: If the type could not be determined.
        """
        dictionary = vars(arguments)
        for possible_type in ('css', 'sass', 'directory'):
            if dictionary[possible_type]:
                return Kind[possible_type.upper()]
        return self._deduce_argument_kind_from_path(arguments.theme)

    def _deduce_argument_kind_from_path(self, path):
        """
        Attempts to deduce the argument type from the input path.

        Args:
            path: (str) The path to inspect.

        Returns:
            A member of the Kind enum.

        Raises:
            ParseError: If the type could not be determined.
        """
        extension = os.path.splitext(path)[1]
        # Alternatively (but less readable) access Kind.__members__
        if extension == '.css':
            return Kind.CSS
        if extension == '.sass':
            return Kind.SASS
        if extension == '.theme' or os.path.isdir(path):
            return Kind.DIRECTORY

        raise ParseError("Could not deduce file type for '{0}'".format(path))

    def _enable_logging(self, verbosity):
        """
        Enables logging functionality.

        The verbosity level may be in the range [0, 2] (inclusive).

        Args:
            verbosity: (int) The verbosity count supplied to the program.
        """
        log.setLevel(logging.WARNING - min(verbosity, 2) * 10)
        level_name = logging.getLevelName(log.getEffectiveLevel())
        log.info('Enabled logging at level %s', level_name)


###############################################################################
# CONVERTER
###############################################################################


def remove_comments(css):
    """
    Removes any comments from CSS content.

    Args:
        css: (str) The CSS from which to remove comments.

    Returns:
        The resulting CSS.
    """
    return re.sub(
        r'/\*.*\*/|//[^\n]*',
        '',
        css,
        re.MULTILINE
    )


def find_blocks(css):
    """
    Finds all CSS blocks in CSS content.

    A block is string of the form '<selectors> { <properties> }'.

    Args:
        css: (str) The CSS in which to find blocks.

    Returns:
        A list of block strings.
    """
    return re.findall(
        r'(\*|[-.a-zA-Z][-.\w:\s,]+)\s*?\{(.+?)\}',
        css,
        re.MULTILINE | re.DOTALL
    )


def truncate_string(string, length=70):
    """
    Truncates a string to the given length, if necessary.

    If the string must be truncated, an ellipsis is added *after*
    truncating the string to the specified length.

    Args:
        strings: (str) The string to (maybe) truncate.
        length: (int) The maximum length the string should have.

    Returns:
        A new string, possibly truncated.
    """
    string = string.replace('\n', '\\n')
    if len(string) <= length:
        return string
    return '{0} ...'.format(string[:length])


@pattern(r'\*|[-.\w: \t#]+\w')
def find_selectors(root, block_selectors):
    """
    Finds the selectors in a CSS block and returns their dictionaries.

    Given a comma-separated list of selectors such as 'a.b #c:d, a b.c, #d',
    this method will:
        1. Create any necessary nested dictionairies in the root object.
        2. Return a list of the leaves, such that they can be updated
           with properties of the block these selectors belong to.

    Note that CSS IDs are handled just like classes, as JSON has no notion
    of selectivity.

    Args:
        root: (dict) The root dictionary to update.
        block_selectors: (str) The string containing the selectors retrieved
                               from a CSS block.

    Returns:
        A list of dictionaries, corresponding directly to the dictionaries
        of each selector. For example, the selector string 'a.b.c, a .b.c,
        #a b c d:f' with an empty root would produce the following tree:
        { a-b-c: { }, a: { b-c: { }, b: { c: { d-f: { }}}}} and the return
        value would be a list o the innermost dictionaries created here.
    """
    log.debug(
        "Processing selector(s): '%s'",
        truncate_string(block_selectors, 70)
    )
    # A selector group is a sequence of selectors one places before a comma or
    # the opening brace of the property block. In `a.b c, #d.g:f #x` there are
    # two selector groups `a.b c` and `#d.g:f #x`
    for selector_group in find_selectors.pattern.findall(block_selectors):
        if selector_group == '*':
            yield root
            continue
        # Start with the root object and iteratively nest deeper for
        # each selector in the group. With a selector, we here mean
        # one level in the DOM hierarchy (such `a.b`).
        parent = root
        log.debug("Processing selector group: '%s'", selector_group)

        selector = None
        for selector in selector_group.split():
            log.info("Processing selector: '%s'", selector)
            # Get rid of any possible ID selector
            # Note that since JSON only has no concept of selectivity
            # an ID has the same semantics as a class selector
            selector = selector.lstrip('#-.')
            log.debug(
                "Removed leading '[#-.]' characters "
                "from selector to yield: '%s'",
                selector
            )

            # We replace class and pseudo-class specifiers with hyphens
            selector = re.sub(r'[:.]', '-', selector)
            log.debug(
                "Replaced '.' and ':' operators from "
                "selector with hyphens to yield: '%s'",
                selector
            )

            # Only one hash lookup
            child = parent.get(selector)
            if child is None:
                log.debug(
                    "'%s' selector did not exist yet in "
                    "parent selector, creating nested block",
                    selector
                )
                child = {}
                parent[selector] = child
            else:
                log.debug("'%s' selector already existed in parent selector")
                log.debug(
                    "Current state of '%s' is: '%s'",
                    truncate_string(selector, 70),
                    child
                )

            # Go one level deeper
            parent = child

        if selector:
            log.debug("Yielding selector leaf '%s'", selector)
            yield parent


def sanitize_property_value(value):
    """
    Sanitizes the value of a property.

    We do not respect any kind of unit for numbers. For this script (and our
    themes), 10px is the same as 10% and 10rem or 10em. In fact, the unit is
    not even inspected, only any number is extracted.

    Args:
        value: (str) The value to sanitize.

    Returns:
        A new value, whose type depends on the value passed. For example, pixel
        values will be converted to their numeric representation, or 'none' is
        converted to None.
    """
    if value == 'none':
        log.debug("Property value '%s' is 'none', converting to 'None'", value)
        return None

    # JSON itself does not have a distinction between floats
    # and ints (there's only 'number'), but if we convert everything
    # to float, there will be a trailing .0 behind every integer

    float_match = re.match(r'^([+-]?\d*\.\d+)', value)
    if float_match is not None:
        log.debug("Determined property value '%s' to be a float", value)
        return float(float_match.group(1))

    number_match = re.match(r'^([+-]?\d+)', value)
    if number_match is not None:
        log.debug("Determined property value '%s' to be an integer", value)
        return int(number_match.group(1))

    log.debug("Property value '%s' seems to be a string", value)
    return value


@pattern(re.compile(r'([-\w]+):([-+\w \t.#!%]+);'))
def find_properties(block_properties):
    """
    Finds all the properties for a CSS block.

    Args:
        block_properties: (str) The block properties to parse.

    Returns:
        A list of (property, value) pairs with which the selector
        dictionaries returned by find_selectors can be updated.
    """
    properties = []
    for key, value in find_properties.pattern.findall(block_properties):
        values = [sanitize_property_value(v) for v in value.split()]
        assert len(values) > 0

        log.debug("Found %d values for property key '%s'", len(values), key)
        values = values[0] if len(values) == 1 else values

        properties.append((key, values))

    return properties


def css_to_json(css_path, output_file):
    """
    Converts the CSS file at the given path to JSON and outputs it to a file.

    Args:
        css_path: (str) The path at which to load the CSS to convert.
        output_file: (file) A file object which to write the lines
                            of the converted CSS (i.e. the JSON).
    """
    log.info("Processing CSS file at path '%s'", css_path)
    with open(css_path) as source:
        css = source.read()

    log.info("Removing comments from CSS")
    css = remove_comments(css)

    root = {}

    log.info("Looking for CSS blocks")
    duration, blocks = time_operation(lambda: find_blocks(css))
    log.info("Took %f seconds to parse CSS file", duration)
    for block in blocks:
        selector_match, properties_match = block
        selectors = find_selectors(root, selector_match)
        properties = find_properties(properties_match)
        log.debug("Found properties '%s'", truncate_string(repr(properties)))

        for selector in selectors:
            log.debug("Updating properties for selector '%s'", selector)
            selector.update(properties)

    log.info("Finished processing '%s' -- dumping JSON!", css_path)
    output_file.write(json.dumps(root, indent=4))
    output_file.write('\n')


def sass_to_json(sass_path, output_file):
    """
    Converts a SASS file to JSON.

    The SASS file is first converted to CSS using the SASS command line
    utility, then the resulting CSS file is converted as if by passing
    it to the script directly.

    Args:
        sass_path: (str) The path of the SASS file to convert.
        output_file: (file) The file to which to write the converted SASS to.
    """
    log.info("Processing SASS file at path '%s'", sass_path)

    sass_path = shlex.quote(sass_path)  # pylint: disable=E1101
    base_path = os.path.splitext(sass_path)[0]
    css_path = '{0}.css'.format(base_path)
    log.info("Compiling SASS file to CSS at path '%s'", css_path)

    command = 'sass --trace --style=compact '
    command += '{0} {1}'.format(sass_path, css_path)
    log.debug("Executing sass command: '%s'", command)
    try:
        # pylint: disable=E1101
        subprocess.run(shlex.split(command), check=True)
    except subprocess.CalledProcessError:
        log.error("SASS to CSS compilation failed!")
        sys.exit()

    css_to_json(css_path, output_file)


def theme_to_json(theme_path, output_file):
    """
    Converts a theme to JSON.

    The CSS file at `theme_path/theme.css` is processed as if by passing
    it to the script directly.

    Args:
        theme_path: (str) The path at which to find the theme.
        output_file: (file) The file to which to write the converted SASS to.
    """
    log.info("Processing theme at path '%s'", theme_path)
    css_path = os.path.join(theme_path, 'theme.css')
    css_to_json(css_path, output_file)


###############################################################################
# MAIN
###############################################################################


def main():
    arguments = Parser().parse(sys.argv[1:])

    # We write to the specified output file or STDOUT
    if arguments.output is None:
        output = sys.stdout
    else:
        output = open(arguments.output, 'w')

    try:
        if arguments.kind == Kind.CSS:
            css_to_json(arguments.input, output)
        elif arguments.kind == Kind.SASS:
            sass_to_json(arguments.input, output)
        else:
            theme_to_json(arguments.input, output)
    finally:
        if output is not sys.stdout:
            output.close()


if __name__ == '__main__':
    main()
