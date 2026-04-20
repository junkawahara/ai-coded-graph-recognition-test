#!/usr/bin/env python3
import sys


def main():
    if len(sys.argv) != 4:
        print("usage: check_ptolemaic.py <in> <exp> <out>")
        return 2
    _, exp_path, out_path = sys.argv[1:4]
    with open(exp_path, 'r', encoding='ascii') as f:
        exp = f.read().strip()
    with open(out_path, 'r', encoding='ascii') as f:
        out = f.read().strip()
    if exp not in ('YES', 'NO'):
        print("bad expected file")
        return 2
    if out not in ('YES', 'NO'):
        print("bad output format")
        return 1
    if exp != out:
        print("mismatch: expected", exp, "got", out)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
