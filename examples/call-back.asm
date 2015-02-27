main:
  mov ts, 0x80
  psh 3
  psh 2
  call sum
  hlt

sum:
  psh bs
  mov bs,ts
  load r0,bs,4
  load r1,bs,6
  add r0,r0,r1
  pop bs
  back  
