from __future__ import absolute_import
from __future__ import print_function

import os
import sys

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
    wvalue = int(sys.argv[1])

    gpio = GPIO()
    gpio.write(wvalue)
    print('write: {}'.format(wvalue))

    rvalue = gpio.read()
    print('read: {}'.format(rvalue))

if __name__ == '__main__':
    main()
