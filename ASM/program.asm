
mov ax, 1
tag loop
push ax
print
add ax, 1
mov cx, 10
cmp ax, cx
jne loop