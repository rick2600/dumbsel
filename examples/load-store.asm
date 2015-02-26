mov r0,0
mov r1,0x80
mov r8,0x1122
mov r9,0x3344
mov r10,0x4455
store r8,r0,r1
store r9,r0,0x82
storeb r10,r0,0x84
load r2,r0,r1
loadb r3,r0,0x84
hlt
