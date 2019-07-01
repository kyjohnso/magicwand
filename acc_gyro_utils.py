#!/usr/bin/env python

import numpy as np

def read_file(filename,i_command=0,n_command=1):
    with open(filename) as f:
        command_list = []
        for l in f.readlines():
            if l[:5] == 'start':
                seq = []
            elif l[:4] == 'stop':
                seq_len = len(seq)
                oh = np.zeros(n_command)
                oh[i_command] = 1
                command_list.append({'sequence':seq,
                                     'command':i_command,
                                     'command_oh': oh,
                                     'seq_len':seq_len})
            elif l[:3] == 'AcX':
                seq_el = []
                for el in l.split('|'):
                    e = el.split('=')
                    seq_el.append(float(e[-1]))
                seq.append(seq_el) 
    return command_list
