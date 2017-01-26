#!/usr/bin/env python
# -*- coding: utf-8 -*-

import common
import re
import os

def getPath(fn):
    npath = os.path.abspath(fn)
    arr = []
    rest = npath
    curr = ''
    while curr != 'erasim':
        rest, curr = os.path.split(rest)
        arr = [curr] + arr
    return '-'.join(arr[2:]).replace('-', '_').replace('.', '_').upper()

def refine(text, file):
    return re.sub(
        r'ERASIM_\S*',
        lambda x: 'ERASIM_' + getPath(file),
        text)

def handler(text, file):
    text = refine(text, file)
    common.fwrite(file, text)

def main():
    common.fileBatchMain(handler)

if __name__ == '__main__':
    main()
