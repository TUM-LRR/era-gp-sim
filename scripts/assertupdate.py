import common
import re
import os

def refine(text):
    return re.sub(
        r'#include\s*<(?:cassert|assert\.h)>',
        lambda x: r'#include "common/assert.hpp"',
        re.sub(
            r'\bassert\b(?=\s*\()',
            'assert::that',
            text
        ))

def handler(text, file):
    text = refine(text)
    common.fwrite(file, text)

def main():
    common.fileBatchMain(handler)

if __name__ == '__main__':
    main()
    