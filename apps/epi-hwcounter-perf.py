#!/usr/bin/env python3
# author: balasr
# compute effective values from pcmr readings

#   I  - Timer interrupt
#
#   P             - Start of periodic task
#   (R)          - End of telemetry / start Reading
#   (r)           - end of reading temp
#   T             - end of power computation and dispatching
#   a             - end of alpha reduction
#   c              - end of PIDs
#   C             - end of Frequency computation
#   p             - end of periodic task
#
#
#   O            - start of taskOS
#   U            - end of initial readings
#   A             - end of parameter adaptation
#   o             - end of writing and taskOS

import argparse
from statistics import mean

parser = argparse.ArgumentParser(prog='epi-measure.py',
                                 description="""Compute effective values
                                 from pcer/pcmr readings""")

parser.add_argument('file', type=str, help='text log of measurments')
parser.add_argument('-v', '--verbose', action='store_true',
                    help='verbose output')

args = parser.parse_args()


def log(s):
    if args.verbose:
        print(s)


with open(args.file, "r") as f:
    # parsing
    timers = []
    wheres = []
    timestamps = []
    for line in f:
        # drop everything until Timer
        stripped_prefix = line[line.find('Timer'):]
        units = stripped_prefix.split(',')
        if units[0] == '\n' or units[0] == '':
            continue

        timer = units[0]
        where = units[1]
        value = units[2]

        timer_val = int(timer.split(':')[1])
        where_val = where.split(':')[1].strip()
        timestamp = int(value.split(':')[1])
        timers.append(timer_val)
        wheres.append(where_val)
        timestamps.append(timestamp)
        log("Parsed line with -> Timer: %d where: %s timestamp: %d"
              % (timer_val, where_val, timestamp))

    # evaluating
    if len(wheres) != len(timestamps):
        raise ValueError("number of entries mismatch")
        exit(1)

    # record pairing of start end, sanity check
    pairs = {'notification': False, 'period': False, 'power': False,
             'pid': False, 'alpha': False, 'freq': False, 'taskos': False,
             'parameter': False}

    def record_start(name):
        if pairs[name]:
            raise ValueError('warning: start recording %s, but already started!' % name)
        pairs[name] = True

    def record_end(name):
        if not(pairs[name]):
            raise ValueError('warning: stop recording %s, but already stopped!' % name)
        pairs[name] = False

    # all intervals
    interrupt_start = 0
    notification_start = 0
    period_start = 0
    power_dispatch_start = 0
    pid_start = 0
    alpha_start = 0
    freq_start = 0
    taskos_start = 0

    parameter_start = 0

    notifications = [] # form interrupt to start of periodic task
    periodic_tasks = [] # duration of the periodic task
    power_dispatch = [] # duration of power dispatch calculation
    pids = []           # duration of pid calculation
    alphas = []         # duration of alpha calculation
    freqs = []          # duration of freq calculation

    taskos_tasks = []
    parameter_adaptions = []

    for where, timestamp in zip(wheres, timestamps):
        #   I  - Timer interrupt
        #
        #   P    - Start of periodic task
        #   (R)  - End of telemetry / start Reading
        #   (r)  - end of reading temp
        #   T    - end of power computation and dispatching
        #   a    - end of alpha reduction
        #   c    - end of PIDs
        #   C    - end of Frequency computation
        #   p    - end of periodic task
        if where == 'I':
            log('Found interrupt at %d' % (timestamp))
            log('Found notificaiton start at %d' % (timestamp))
            record_start('notification')
            interrupt_start = timestamp
            notification_start = timestamp
        elif where == 'P':
            log('Found notification end at %d' % (timestamp))
            log('Found period start at %d' % (timestamp))
            log('Found power start at %d' % (timestamp))
            record_end('notification')
            record_start('period')
            record_start('power')
            notifications.append(timestamp - notification_start)
            period_start = timestamp
            power_dispatch_start = timestamp
        elif where == 'T':
            log('Found power end at %d' % (timestamp))
            log('Found alpha start at %d' % (timestamp))
            record_end('power')
            record_start('alpha')
            power_dispatch.append(timestamp - power_dispatch_start)
            alpha_start = timestamp
        elif where == 'a':
            log('Found alpha end at %d' % (timestamp))
            log('Found pid start at %d' % (timestamp))
            record_end('alpha')
            record_start('pid')
            alphas.append(timestamp - alpha_start)
            pid_start = timestamp
        elif where == 'c':
            log('Found pid end at %d' % (timestamp))
            log('Found frequency start at %d' % (timestamp))
            record_end('pid')
            record_start('freq')
            pids.append(timestamp - pid_start)
            freq_start = timestamp
        elif where == 'C':
            log ('Found frequency end at %d' % (timestamp))
            record_end('freq')
            freqs.append(timestamp - freq_start)
        elif where == 'p':
            log('Found period end at %d' % (timestamp))
            record_end('period')
            periodic_tasks.append(timestamp - period_start)
        #   O            - start of taskOS
        #   U            - end of initial readings
        #   A            - end of parameter adaptation
        #   o            - end of writing and taskOS
        elif where == 'O':
            log('Found taskos start at %d' % (timestamp))
            log('Found parameter adaption start at %d' % (timestamp))
            record_start('taskos')
            record_start('parameter')
            taskos_start = timestamp
            parameter_start = timestamp
        elif where == 'U':
            log("UNIMPLEMENTED 'U'")
            exit(1)
        elif where == 'A':
            log('Found parameter end at %d' % (timestamp))
            record_end('parameter')
            parameter_adaptions.append(timestamp - parameter_start)
        elif where == 'o':
            log('Found taskos end at %d' % (timestamp))
            record_end('taskos')
            taskos_tasks.append(timestamp - taskos_start)

    # sanity checks (positive) values, non-empty lists, sum test

    # drop last samples because they contain time from printfs and other instrumentation
    periodic_tasks = periodic_tasks[:-1]
    power_dispatch = power_dispatch[:-1]
    pids = pids[:-1]
    alphas = alphas[:-1]
    freqs = freqs[:-1]
    taskos_tasks = taskos_tasks[:-1]
    parameter_adaptions = parameter_adaptions[:-1]

    # show results
    print('notification duration: ' + str(notifications))
    print('perodic tasks duration: ' + str(periodic_tasks))
    print('power dispatch duration: ' + str(power_dispatch))
    print('pid duration: ' + str(pids))
    print('alpha duration: ' + str(alphas))
    print('freq compuation duration: ' + str(freqs))
    print('parameter adapation duration: ' + str(parameter_adaptions))
    print('taskos duration' + str(taskos_tasks))

    print('section name        | average number of cycles')
    print('-' * 50)
    print('notification        | %d' % int(mean(notifications)))
    print('power dispatch      | %d' % int(mean(power_dispatch)))
    print('PID compuation      | %d' % int(mean(pids)))
    print('alpha computation   | %d' % int(mean(alphas)))
    print('freq compuation     | %d' % int(mean(freqs)))
    print('parameter adapation | %d' % int(mean(parameter_adaptions)))
    print('-' * 50)
    print('taskos task         | %d' % int(mean(taskos_tasks)))
    print('perodic task        | %d' % int(mean(periodic_tasks)))
