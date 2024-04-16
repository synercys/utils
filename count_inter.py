'''
Python script to print info. about interrupts in a duration 't' on -cpulist on a Linux-based OS
Use it in case you need to find interrupts that are very frequent, waking the CPU often

Uses /proc interface, tested on Ubuntu 22.04 and Raspbian Linux v5.5

Usage:
    python3 count_inter.py --cpulist 0-6 --time 30 captures interrupts on cpus 0-6 in a duration of 30 seconds

Contact:
    Ashish Kashinath <ashishk3@illinois.edu> in case you find issues or if it doesn't work
'''


import argparse
import time
import csv
import re
import sys
import os
import json
from shutil import which
from subprocess import Popen, PIPE, STDOUT
from enum import Enum, unique

class Interrupt:
    def __init__(self, vec, name, counter_for_all_cpus) :
        self.counter = list()
        self.vector = vec
        self.irqname = name
        for count in counter_for_all_cpus :
            self.counter.append(int(count))
        self.diff_counter = list()

    def get_sum(self) :
        return sum(self.counter)

    def update_and_diff(self, counter_for_all_cpus) :
        self.diff_counter = list()
        for i, count in enumerate(counter_for_all_cpus) :
            self.diff_counter.append(int(count) - self.counter[i])
            self.counter[i] = int(count)

    def get_irq_count_all(self) :
        if self.diff_counter :
            return sum(self.diff_counter)
        else :
            return 0

    def get_irq_count_cpulist(self, cpu_list) :
        if not self.diff_counter :
            return 0
        cpus = cpu_list.split(',')
        cpu_ids = set()
        for cpu in cpus :
            if '-' in cpu :
                a, b = cpu.split('-')
                for i in range(int(a), int(b)+1) :
                    cpu_ids.add(i)
            else :
                cpu_ids.add(int(cpu))
        return sum([diff for i, diff in enumerate(self.diff_counter) if i in cpu_ids])


def main() :
    parser = argparse.ArgumentParser(description='Tool to log interrupts happening during certain amount of time')
    parser.add_argument('-cpulist', '--cpulist', required=False, dest='cpulist', help='CPUs list, if ignored, default ALL')
    parser.add_argument('-t', '--time', required=True, dest='time', help='Sampling time for interrupts')
    args = parser.parse_args()
    if args.time == '0' :
        return

    irq_list = dict()
    with open('/proc/interrupts', 'r') as irqfile:
        header = irqfile.readline().split()
        for line in irqfile :
            irq_data = line.split()
            vec = irq_data[0].strip(':')
            if vec in ('ERR', 'MIS') :
                continue
            name = ' '.join(irq_data[1 + len(header):])
            counters = irq_data[1:len(header)+1]
            irq_list[vec] = Interrupt(vec, name, counters)

    time.sleep(int(args.time))

    with open('/proc/interrupts', 'r') as irqfile:
        header = irqfile.readline().split()
        for line in irqfile :
            irq_data = line.split()
            vec = irq_data[0].strip(':')
            if vec in ('ERR', 'MIS') :
                continue
            name = ' '.join(irq_data[1 + len(header):])
            counters = irq_data[1:len(header)+1]
            irq_list[vec].update_and_diff(counters)
            if args.cpulist :
                print(name, irq_list[vec].get_irq_count_cpulist(args.cpulist))
            else :
                print(name, irq_list[vec].get_irq_count_all())

main()
