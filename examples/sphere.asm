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
	push	65536
	ja		.DRAW_LOOP
	ret

ON_SPHERE:
	push	256
	push	ax
	div

	push	128
	sub
	pop		bx

	push	bx
	push	bx
	mul

	push	256
	push	ax
	div
	push	256
	mul
	push	ax
	sub

	push	128
	sub
	pop		bx

	push	bx
	push	bx
	mul

	add

	push	16384
	jb		.ON_SPHERE_FALSE

	push	1
	ret

.ON_SPHERE_FALSE:
	push	0
	ret
