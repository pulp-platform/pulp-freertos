#!/usr/bin/env python3

import json
import argparse

parser = argparse.ArgumentParser(prog='count-cycles.py',
                                 description="count cycles")

parser.add_argument('file', type=str, help='log')
parser.add_argument('-v', '--verbose', action='store_true',
                    help='verbose output')

args = parser.parse_args()

with open(args.file) as f:
    stats = json.loads(f.read())
    total_cycles = 0
    for k, v in stats.items():
        print(k + ' contributes ' + str(v['cycles']) + ' cycles')
        total_cycles += int(v['cycles'])

    print('total cycles: ' + str(total_cycles))
