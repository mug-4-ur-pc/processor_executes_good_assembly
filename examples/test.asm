	in   ax
	call a
	push ax
	out
	hit
a:
	push ax
	push ax
	mul
	pop	 ax
	ret
