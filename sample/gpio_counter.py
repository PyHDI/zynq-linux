from __future__ import absolute_import
from __future__ import print_function

import os
import sys
import time

sys.path.insert(0, os.path.abspath('../lib/'))

import axis


class GPIO(object):

    def __init__(self, port=0, read_offset=8, write_offset=0):
        self.axis = axis.AXIS()
        self.port = port
        self.read_offset = read_offset
        self.write_offset = write_offset

    def write(self, data):
        self.axis.write(self.port, self.write_offset, data)

    def read(self):
        return self.axis.read(self.port, self.read_offset)


def main():
    interval = 0.1
    if len(sys.argv) > 1:
        try:
            interval = float(sys.argv[1])
        except:
            pass
    
    gpio = GPIO()
    rep_num = gpio.read()

    for i in range(rep_num):
        time.sleep(interval)
        gpio.write(i)

if __name__ == '__main__':
    main()
