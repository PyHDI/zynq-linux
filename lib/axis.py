from __future__ import absolute_import
from __future__ import print_function

import mmap
import struct


class AXIS(object):

    def __init__(self, uio_prefix='/dev/uio',
                 num_axis=8, axis_size=0x1000):
        self.num_axis = num_axis
        self._fds = [open(''.join([uio_prefix, str(i)]), 'r+b')
                     for i in range(self.num_axis)]
        self.axis = [mmap.mmap(f.fileno(), axis_size) for f in self._fds]

    def __del__(self):
        for a in self.axis:
            a.close()
        for f in self._fds:
            f.close()

    def write(self, index, offset, value):
        self.axis[index][offset:offset + 4] = struct.pack('i', value)

    def read(self, index, offset):
        return struct.unpack('i', self.axis[index][offset:offset + 4])[0]
