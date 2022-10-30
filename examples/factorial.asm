	in		ax
	push	ax
	call	FACTORIAL
	out
	hit

FACTORIAL:
	pop		ax
	push	1
	push	ax
	jna		.END_FACTORIAL

	push	ax
	push	1
	push	ax
	sub
	call	FACTORIAL
	mul
	ret

.END_FACTORIAL:
	push	1
	ret
