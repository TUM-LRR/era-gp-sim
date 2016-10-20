#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import os
import sys
import yaml


def convert(path):
    print('Processing "{0}" ...'.format(path))
    with open(path, 'r') as source:
        intermediate = yaml.load(source)
    output_path = '{0}.json'.format(os.path.splitext(path)[0])
    with open(output_path, 'w') as destination:
        destination.write(json.dumps(intermediate, indent=4))
    print('Wrote "{0}" ...'.format(output_path))


def main():
    if len(sys.argv) < 2:
        print('usage: yaml2json <files...>')
        sys.exit(-1)

    for path in sys.argv[1:]:
        convert(path)

    print('Done \033[91m<3\033[0m')


if __name__ == '__main__':
    main()
