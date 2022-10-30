	in		ax
	in		bx
	in		cx
	
	push	ax
	push	0
	je		LINEAR

	push	bx
	push	bx
	mul
	pop		dx
	push	4
	push	ax
	mul
	push	cx
	mul
	push	dx
	sub
	pop		dx
	push	dx
	push	0
	ja		INF_OR_NO

	push	2
	push	ax
	mul
	pop		ex
	push	dx
	sqrt
	push	0
	je		DISCR_IS_ZERO

	push	bx
	push	0
	sub
	pop		bx

	push	ex
	push	dx
	push	bx
	add
	div
	out

	push	ex
	push	dx
	push	bx
	sub
	div
	out
	hit

DISCR_IS_ZERO:
	push	ex
	push	bx
	push	0
	sub
	div
	out
	hit

LINEAR:
	push	bx
	push	0
	je		INF_OR_NO

	push	0
	push	cx
	sub
	pop		cx
	push	bx
	push	cx
	div
	out
	hit

INF_OR_NO:
	hit
