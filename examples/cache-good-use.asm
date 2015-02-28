mov r0, 0
mov r1, 8
mov r2, 1
mov r3, 256

for:  
  cmp r0, 8
  bre exit
  inc r0
  dec r1
  shl r2,r2,1
  shr r3,r3,1
  br for

exit:  
hlt
