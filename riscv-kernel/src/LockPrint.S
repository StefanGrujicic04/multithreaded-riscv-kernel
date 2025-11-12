.global copyAndSwap
copyAndSwap:
    lr.w t0, (a0) # gives reservation
    bne a1 , t0 , fail #if not match than go fail
    sc.w  t0, a2, (a0) #checks if what is going to be written is same as reservation
    bnez t0, copyAndSwap #if t0 not 0 try again
    li a0,0
    jr ra
fail:
    li a0, 1
    jr ra