#!/usr/bin/env python

import mach
import sys

for arg in sys.argv[1:]:
    print arg, mach.task_for_pid(int(arg))
