.global _ZN3TCB13contextSwitchEPNS_7ContextES1_
.type _ZN3TCB13contextSwitchEPNS_7ContextES1_ , @function
_ZN3TCB13contextSwitchEPNS_7ContextES1_:
    BEQ  x0, a0, nw #if a0==0 jump to nw because there is no need for storing old context
    sd ra, 0 * 8(a0)
    sd sp, 1 * 8(a0)

nw: ld ra, 0 * 8(a1)
    ld sp, 1 * 8(a1)

    ret