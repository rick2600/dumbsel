mov r0, 0xffff
mov r1, 0
mov r2, 0xfffe
mov r3, 0xffff
mov r4, 4
cmps r0,r1
cmps r0,r2
cmps r0,r3
cmps r0,r4
cmpsb r0,r2
cmpsb r0,r4
hlt
