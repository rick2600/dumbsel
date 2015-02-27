mov r0, 0
increment:
  inc r0
  cmp r0, 10
  brne increment
hlt
