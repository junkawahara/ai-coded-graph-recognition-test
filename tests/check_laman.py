#!/usr/bin/env python3
import sys


def main():
    if len(sys.argv) != 4:
        print("usage: check_TYPE.py <in> <exp> <out>")
        return 2

    in_path, exp_path, out_path = sys.argv[1:4]

    with open(exp_path, "r", encoding="ascii") as f:
        expected = f.read().strip()

    with open(out_path, "r", encoding="ascii") as f:
        actual = f.read().strip()

    if actual != expected:
        print("mismatch: expected", repr(expected), "got", repr(actual))
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
