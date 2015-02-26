mov r0, 0x1122
mov r1, 0x3344
mov ts, 0x80
psh r1
psh r0
pop r8
pop r7
psh 0x5566
pop r6
hlt
