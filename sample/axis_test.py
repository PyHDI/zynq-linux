from __future__ import absolute_import
from __future__ import print_function

import os
import sys

sys.path.insert(0, os.path.abspath('../lib/'))

import axis


def main():
    myaxis = axis.AXIS()
    port = 0

    wvalue = int(sys.argv[1])
    woffset = 0
    print('write: {}'.format(wvalue))
    myaxis.write(port, woffset, wvalue)

    roffset = 8
    rvalue = myaxis.read(port, roffset)
    print('read: {}'.format(rvalue))

if __name__ == '__main__':
    main()
