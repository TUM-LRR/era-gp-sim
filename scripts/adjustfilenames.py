import common
import re
import os

def adjustedFilename(fn):
    root, file = os.path.split(fn)
    name, ext = os.path.splitext(file)
    newname = common.transformCamelCaseLowerCase(name, '-') + ext
    if root == '':
        return os.path.join(newname)
    else:
        return os.path.join(root, newname)
    
def rewriteFilename(fn, loc):
    fn = adjustedFilename(fn)
    dir, ofile = os.path.split(os.path.abspath(loc))
    npath = os.path.normpath(os.path.join(dir, fn))
    if not os.path.exists(npath):
        npath = os.path.join('era-gp-sim', 'include', fn)
    arr = []
    rest = npath
    curr = ''
    while curr != 'era-gp-sim':
        rest, curr = os.path.split(rest)
        arr = [curr] + arr
    return '/'.join(arr[2:])

def refineIncludes(file, text):
    return re.sub(
        r'#include\s*"(.*?)"',
        lambda x: r'#include "' + rewriteFilename(x.group(1), file) + r'"',
        text)

def handler(text, file):
    text = refineIncludes(file, text)
    common.fwrite(file, text)
    os.rename(file, adjustedFilename(file))

def main():
    common.fileBatchMain(handler)

if __name__ == '__main__':
    main()
