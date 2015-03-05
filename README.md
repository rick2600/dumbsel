# Dumbsel

Dumbsel is a dumb and weird architecture for learning purporse.

##### Playing
```
git clone https://github.com/rick2600/dumbsel.git
cd dumbsel
make
./dumbsel examples/<file>.bin
```

## Documentation


##### Registers
Registers are 16 bits
- r0 (special register, may be used implicitly in some instructions, return value)
- r1-r5 (params)
- r6-13 (general purpose)
- bs (base stack)
- ts (top stack)
- pc (program conter, not user accessible)
- flags (execution status, see: ldflg/stflg)

##### Cache
There is an instruction cache that caches up to 16 recently executed instruction


##### Instructions


###### Data movement
- (01) mov ra,rb/imm : - mov data to register
- (02) ext ra,rb/imm : extend least significant byte from rb/imm into ra (unsigned)
- (03) exts ra,rb/imm : extend least significant byte from rb/imm into ra (signed)

###### Arithmetic
- (04) add ra,rb,rc/imm : ra = rb + rc/imm
- (05) sub ra,rb,rc/imm : ra = rb - rc/imm
- (06) mul ra,rb,rc/imm : ra = rb * rc/imm
- (07) div ra,rb,rc/imm : ra = rb / rc/imm
- (08) inc ra : ra = ra + 1
- (09) dec ra : ra = ra - 1

###### Logic
- (10) or ra,rb,rc/imm : ra = rb | rc/imm
- (11) and ra,rb,rc/imm : ra = rb & rc/imm
- (12) xor ra,rb,rc/imm : ra = rb ^ rc/imm
- (13) shl ra,rb,rc/imm : ra = rb << rc/imm
- (14) shr ra,rb,rc/imm : ra = rb >> rc/imm
- (15) not ra : ra = ~ra

###### Comparison
- (16) cmp ra,rb/imm : compares ra and rb/imm (unsigned)
- (17) cmps ra,rb/imm : compares ra and rb/imm (signed)

###### Direct memory access
- (18) load ra,rb,rc/imm : ra = mem[rb+rc/imm]
- (19) store ra,rb,rc/imm : mem[rb+rc/imm] = ra

###### Stack operation
- (20) psh ra/imm : pushes ra/imm on stack
- (21) pop ra : pops word on top of stack into ra
- (47) psha : pushes r0-13, bs, ts on stack
- (48) popa : pops from stack, values for r0-13, bs

###### Branches
- (22) br ra/imm : branches to pc+imm or branches to ra
- (23) bre ra/imm : branches to pc+imm or branches to ra if ZF is set
- (24) brne ra/imm : branches to pc+imm or branches to ra if ZF is clear
- (25) brg ra/imm : branches to pc+imm or branches to ra if GT is set
- (26) brge ra/imm : branches to pc+imm or branches to ra if GT or ZF is set
- (27) brl ra/imm : branches to pc+imm or branches to ra if LT is set
- (28) brle ra/imm : branches to pc+imm or branches to ra if LT or ZF is set
- (32) call ra/imm : save next instruction addr on stack and branches to pc+imm or branches to ra
- (33) back : pops the value on top of stack and branches 

###### Misc
- (00) nop : does nothing
- (35) hlt : halt execution
- (36) ldflg : load flag into r0
- (37) stflg : store r0's content into flag
- (38) ldccr : load ccr into r0
- (39) stccr : store r0's content into ccr
- (40) ldicr : load icr into r0
- (41) sticr : store r0's content into icr
- (42) ldtcr : load tcr into r0
- (43) sttcr : store r0's content into tcr
- (44) di : disable interruption
- (45) ei : enable interruption




