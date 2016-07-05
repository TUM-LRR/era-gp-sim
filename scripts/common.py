import sys
import re

def fread(fname):
    with open(fname, 'r') as file:
        return file.read()

def fwrite(fname, content):
    with open(fname, 'w') as file:
        return file.write(content)

def transformCamelCase(text, transform, joinstr):
    #Just to explain the regex: We check for all occurences of lower strings, capitalized strings, numbers, and uppercase strings,
    #but so, that the next string is capitalized if possible. Or so. I do not trust it fully yet. But it seems to work. --@DXIS
    return joinstr.join(transform(x) for x in re.findall(r'([A-Z]?[a-z]+|[A-Z]+(?=[^a-z]|\b)|[0-9]+)', text))

def transformCamelCaseUpperCase(text, sep):
    return transformCamelCase(text, lambda x: x.upper(), sep)

def transformCamelCaseLowerCase(text, sep):
    return transformCamelCase(text, lambda x: x.lower(), sep)

def fileBatchMain(handler):
    if len(sys.argv) < 2:
        print('You should specify at least one file.')
        return
    
    for i in range(1, len(sys.argv)):
        file = sys.argv[i]
        text = fread(file)
        text = handler(text, file)

    print('Done!')
