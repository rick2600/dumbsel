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
- (0x01) mov ra,rb/imm : - mov data to register
- ext ra,rb/imm : extend least significant byte from rb/imm into ra (unsigned)
- exts ra,rb/imm : extend least significant byte from rb/imm into ra (signed)

###### Arithmetic
- add ra,rb,rc/imm : ra = rb + rc/imm
- sub ra,rb,rc/imm : ra = rb - rc/imm
- mul ra,rb,rc/imm : ra = rb * rc/imm
- div ra,rb,rc/imm : ra = rb / rc/imm
- inc ra : ra = ra + 1
- dec ra : ra = ra - 1

###### Logic
- or ra,rb,rc/imm : ra = rb | rc/imm
- and ra,rb,rc/imm : ra = rb & rc/imm
- xor ra,rb,rc/imm : ra = rb ^ rc/imm
- shl ra,rb,rc/imm : ra = rb << rc/imm
- shr ra,rb,rc/imm : ra = rb >> rc/imm
- not ra : ra = ~ra

###### Comparison
- cmp ra,rb/imm : compares ra and rb/imm (unsigned)
- cmps ra,rb/imm : compares ra and rb/imm (signed)

###### Direct memory access
- load ra,rb,rc/imm : ra = mem[rb+rc/imm]
- store ra,rb,rc/imm : mem[rb+rc/imm] = ra

###### Stack operation
- psh ra/imm : pushes ra/imm on stack
- pop ra : pops word on top of stack into ra

###### Branches
- br ra/imm : branches to pc+imm or branches to ra
- bre ra/imm : branches to pc+imm or branches to ra if ZF is set
- brne ra/imm : branches to pc+imm or branches to ra if ZF is clear
- brg ra/imm : branches to pc+imm or branches to ra if GT is set
- brge ra/imm : branches to pc+imm or branches to ra if GT or ZF is set
- brl ra/imm : branches to pc+imm or branches to ra if LT is set
- brle ra/imm : branches to pc+imm or branches to ra if LT or ZF is set
- call ra/imm : save next instruction addr on stack and branches to pc+imm or branches to ra
- back : pops the value on top of stack and branches 

###### Misc
- nop : does nothing
- hlt : halt execution
- ldflg : load flag into r0
- stflg : store r0's content into flag







