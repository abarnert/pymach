#!/usr/bin/env python

import mach
import sys
def ByteToHex( byteStr ):
    """
    Convert a byte string to it's hex string representation e.g. for output.
    """
    
    # Uses list comprehension which is a fractionally faster implementation than
    # the alternative, more readable, implementation below
    #   
    #    hex = []
    #    for aChar in byteStr:
    #        hex.append( "%02X " % ord( aChar ) )
    #
    #    return ''.join( hex ).strip()        

    return ''.join( [ "%02X " % ord( x ) for x in byteStr ] ).strip()

for arg in sys.argv[1:]:
    pid  = arg 
    addr = 0x186a0
    hp_addr = 0x1EB58778
    sz   = 0x10
    task = mach.task_for_pid(int(pid))
    data = mach.vm_read(task,hp_addr,sz)
    print pid," | ", ByteToHex(data), " | ", data
