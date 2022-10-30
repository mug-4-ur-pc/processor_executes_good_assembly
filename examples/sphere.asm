	call	DRAW_SPHERE
	drw
	hit

DRAW_SPHERE:
	push	0
	pop		ax

.DRAW_LOOP:
	call	ON_SPHERE
	pop		[ax]
	push	ax
	push	1
	add
	pop		ax
	push	ax
	push	256
	ja		.DRAW_LOOP
	ret

ON_SPHERE:
	push	16
	push	ax
	div

	push	8
	sub
	pop		bx

	push	bx
	push	bx
	mul

	push	16
	push	ax
	div
	push	16
	mul
	push	ax
	sub

	push	8
	sub
	pop		bx

	push	bx
	push	bx
	mul

	add

	push	49
	jb		.ON_SPHERE_FALSE

	push	1
	ret

.ON_SPHERE_FALSE:
	push	0
	ret
