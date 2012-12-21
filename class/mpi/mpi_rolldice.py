#!/usr/bin/env python
"""
use mpi to roll lots of dice...

usage::

  $ mpiexec -n <nprocs> python rolldice.py
"""

from mpi4py import MPI
from random import *

##############################################################

"""
simulate rolling five 6-sided dice; keep going until 
we get 5-of-a-kind. display/return how many rolls it took.

J. Knerr
Spring 2012
"""

def dosims(nsims):
  """do nsims simulations, accumulate total rolls needed to get 5oak"""

  total = 0
  for j in range(nsims):
    numrolls = 0
    dice = range(5)
    while True:
      # roll the 5 dice
      for i in range(5):
        dice[i] = randrange(1,7)
      numrolls = numrolls + 1
      # see if they are all equal
      got5 = True
      first = dice[0]
      for d in dice:
        if d != first:
          got5 = False
      if got5:
        #print "Got 5-of-a-kind: %s -- took %d tries\n" % (dice, numrolls)
        break
    total = total + numrolls

  return total

##############################################################

comm = MPI.COMM_WORLD
nprocs = comm.Get_size()
myrank = comm.Get_rank()
hname = MPI.Get_processor_name()

Nper = 20

if myrank == 0:
  print "using %d tasks. each does %d simulations..." % (nprocs, Nper)
else:
  print "---> %20s is task %2d <--" % (hname, myrank)

mytotal = dosims(Nper)
# just for debugging
print "(%15s) rank %2d total: %d" % (hname, myrank, mytotal)

total = comm.reduce(mytotal, op=MPI.SUM, root=0)

if myrank == 0:
  result = float(total)/(Nper * nprocs)
  print "final result: %0.1f is ave rolls to get 5-of-a-kind (%d sims)" % (result, Nper*nprocs)

##############################################################

