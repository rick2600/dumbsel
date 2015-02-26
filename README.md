# Dumbsel

Dumbsel is a dumb and weird architecture for learning purporse.

##### Playing
```
git clone https://github.com/rick2600/dumbsel.git
cd dumbsel
make
./dumbsel tests/example_00.bin
```

# Documentation

##### Registers
Registers are 16 bits
- r0 - r13
- bs (base stack)
- ts (top stack)

##### Instructions

###### Data movement
- mov ra,rb/imm : - mov data to register
- ext ra,rb/imm : extend byte to unsigned short
- exts ra,rb/imm : extend byte to signed short

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
- not ra : ra = -ra

###### Comparison
- cmp ra,rb/imm : compares ra and rb/imm (unsigned)
- cmps ra,rb/imm : compares ra and rb/imm (signed)

###### Direct memory access
- load ra,rb,rc/imm : ra = mem[rb+rc/immm]
- store ra,rb,rc/imm : mem[rb+rc/imm] = ra

###### Stack operation
- psh ra/imm : pushes ra/imm on stack
- pop ra : pops word on top of stack into ra

###### Branches
- br ra/imm : branches to pc+imm or branches to ra
- bre ra/imm : branches to pc+imm or branches to ra if ZF is set
- brne ra/imm : branches to pc+imm or branches to ra if ZF is clear

###### Misc
- nop : does nothing
- hlt : halt execution







