#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import re
import sys


def parse(argument):
    root = re.match(r'.*/era-gp-sim', os.getcwd()).group()
    relative_path = os.path.dirname(argument)
    name = os.path.basename(argument)

    return root, relative_path, name

def get_license(root):
    path = os.path.join(root, 'scripts', 'short-license.txt')
    with open(path, 'r') as short_license:
        return short_license.read() + '\n'


def create_header(root, relative_path, name):
    directory = os.path.join(root, "include", relative_path)
    header_path = os.path.join(directory, '{0}.hpp'.format(name))
    guard = '{0}_HPP'.format(name.replace('-', '_').upper())
    class_name = ''.join([i.capitalize() for i in name.split('-')])
    print("Creating header file for class '{0}' ...".format(class_name))
    with open(header_path, 'w') as header:
        header.write(get_license(root))
        header.write('#ifndef {0}\n'.format(guard))
        header.write('#define {0}\n\n'.format(guard))
        header.write('class {0} {{\n\n}};\n\n'.format(class_name))
        header.write('#endif /* {0} */\n'.format(guard))


def create_source(root, relative_path, name):
    directory = os.path.join(root, "source", relative_path)
    source_path = os.path.join(directory, name + ".hpp")
    include_path = os.path.join(relative_path, "{0}.hpp".format(name))
    print('Creating source file ...')
    with open(source_path, 'w') as source:
        source.write('#include "{0}"\n'.format(include_path))


def main():
    if len(sys.argv) < 2:
        raise RuntimeError("Usage: new.py <relative/path/to/file>")

    root, relative_path, name = parse(sys.argv[1])
    create_header(root, relative_path, name)
    create_source(root, relative_path, name)
    print('Done \033[91m<3\033[0m')

if __name__ == "__main__":
    main()
