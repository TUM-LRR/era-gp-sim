#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import sys


def read(path):
  with open(path, 'r') as source:
    return source.read()


def getify(text):
  return re.sub(
    r'\b(?!has|get|set|is)([a-z]\w+)\(',
    lambda m: 'get{0}('.format(m.group(1).capitalize()),
    text
  )


def write(path, text):
  with open(path, 'w') as destination:
    destination.write(text)


def main():
  if len(sys.argv) < 2:
    raise RuntimeError('Usage: getify <path/to/file>')
  text = read(sys.argv[1])
  text = getify(text)
  write(sys.argv[1], text)

if __name__ == '__main__':
  main()
