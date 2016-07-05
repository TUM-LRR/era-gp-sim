import common
import re
import os

def adjustedFilename(fn, separatorJoiner):
    root, file = os.path.split(fn)
    name, ext = os.path.splitext(file)
    newname = common.transformCamelCaseLowerCase(name, '-') + ext
    if root == '':
        return separatorJoiner(newname)
    else:
        return separatorJoiner(root, newname)

def refineIncludes(text):
    return re.sub(
        r'#include\s*"(include/)?(.*)"',
        lambda x: r'#include "' + adjustedFilename(x.group(2), lambda *x: '/'.join([*x])) + r'"',
        text)

def handler(text, file):
    text = refineIncludes(text)
    common.fwrite(file, text)
    os.rename(file, adjustedFilename(file, os.path.join))

def main():
    common.fileBatchMain(handler)

if __name__ == '__main__':
    main()