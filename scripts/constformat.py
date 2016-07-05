import common
import re

def handler(text, file):
    text = re.sub(r'\b(k[A-Z]\w*)\b', lambda x: common.transformCamelCaseUpperCase(x.group(1), '_')[2:], text)
    common.fwrite(file, text)

def main():
    common.fileBatchMain(handler)

if __name__ == '__main__':
    main()
