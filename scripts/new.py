#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import re
import sys
import argparse


def create_parser():
    parser = argparse.ArgumentParser(description='Create a new class.')
    parser.add_argument('path', metavar='P',
                        help='path/to/class')
    parser.add_argument('-o', '--header-only', action='store_true',
                        help='Whether to only create a header')
    return parser


def parse(argument):
    root = re.match(r'.*/era-gp-sim', os.getcwd()).group()
    relative_path = os.path.dirname(argument)
    name = os.path.basename(argument)

    return root, relative_path, name


def get_license(root):
    path = os.path.join(root, 'scripts', 'short-license.txt')
    with open(path, 'r') as short_license:
        return short_license.read() + '\n'


def create_header(argument, root, relative_path, name):
    directory = os.path.join(root, "include", relative_path)
    header_path = os.path.join(directory, '{0}.hpp'.format(name))
    guard_name = re.sub(r'[-/]', '_', argument).upper()
    guard = 'ERAGPSIM_{0}_HPP'.format(guard_name)
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
    source_path = os.path.join(directory, name + ".cpp")
    include_path = os.path.join(relative_path, "{0}.hpp".format(name))
    print('Creating source file ...')
    with open(source_path, 'w') as source:
        source.write(get_license(root))
        source.write('#include "{0}"\n'.format(include_path))


def main():
    parser = create_parser()
    args = parser.parse_args(sys.argv[1:])

    root, relative_path, name = parse(args.path)
    create_header(args.path, root, relative_path, name)
    if not args.header_only:
        create_source(root, relative_path, name)

    print('Done \033[91m<3\033[0m')

if __name__ == "__main__":
    main()
