[BITS 32]

global invalidateDirectory
invalidateDirectory:
	mov eax, cr3
	mov cr3, eax
	ret