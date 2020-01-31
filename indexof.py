#!/usr/bin/env python3
from sys import argv

if __name__ == "__main__":
    if len(argv) != 2:
        exit("usage: %s <string>")
    output = []
    for c in argv[1]:
        c = ord(c)
        if c >= 0x41 and c <= 0x5A: # uppercase
            output.append(c - 0x41)
        elif c >= 0x61 and c <= 0x7A: # lowercase
            output.append(c - 0x47) # 0x61 - 26 = 0x47 | 'a' starts at index 26
        elif c >= 0x30 and c <= 0x39: # number
            output.append(c + 0x4) # 0x30 - 52 = -4 | '0' starts at index 52
        elif c == 0x2e: # '.'
            output.append(62)
    # To get the correct C syntax
    output = str(output).replace("[", "{").replace("]", "}")
    print(" %s\nSize: %d" % (output, len(argv[1])))

