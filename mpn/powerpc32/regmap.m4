divert(-1)

dnl Map register names r0, r1, etc, to just `0', `1', etc.
dnl This is needed on all systems but NeXT, Rhapsody, and MacOS-X
forloop(i,0,31,
`define(`r'i,i)'
)

divert
