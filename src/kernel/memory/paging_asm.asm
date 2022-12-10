[BITS 64]

global invalidateDirectory
invalidateDirectory:
	mov rax, cr3
	mov cr3, rax
	ret