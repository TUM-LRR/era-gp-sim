#!/usr/bin/env python
# -*- coding: utf-8 -*-

import common

def handler(text, file):
    common.fwrite(file, text)

def main():
    common.fileBatchMain(handler)

if __name__ == '__main__':
    main()
    