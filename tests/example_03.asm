mov r0,0
mov r1,0x3344
mov r2,0x5566
mov r3,0x7788
mov r4,6

load r5,r0,4
load r6,r0,r4
loadb r7,r0,4

mov r4, 18
store r1,r0,16
store r2,r0,r4
storeb r3,r0,20
hlt
