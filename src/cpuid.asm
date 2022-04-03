EFLAGS_ID_BIT EQU 0x00200000

SECTION .text

GLOBAL is_cpuid_supported

; https://wiki.osdev.org/CPUID
;
; When returning, eax is 0 if CPUID is not supported and 1 if CPUID is
; supported.
;
; Assuming that EFLAGS is 0x00200000 and ID bit can be modified.
; <pushfd>				-> EFLAGS == 0x00200000
; <pushfd>				-> EFLAGS == 0x00200000
; <Invert value on top of stack>	-> EFLAGS == 0x00200000
; <popfd>				-> EFLAGS == 0x00000000
; <pushfd>				-> EFLAGS == 0x00000000
; <pop eax>				-> EFLAGS == 0x00000000, eax == 0x00000000
; <xor eax with top of stack>		-> EFLAGS == 0x00000000, eax == 0x00200000
;	(Top of stack is the original value of EFLAGS)
; <popfd>				-> EFLAGS == 0x00200000, eax == 0x00200000
; <and eax with constant EFLAGS_ID_BIT>	-> EFLAGS == 0x00200000, eax == 0x00200000
osdev_is_cpuid_supported:
	pushfq				; Save original value of EFLAGS

	pushfq				; Save EFLAGS
	xor dword [esp],EFLAGS_ID_BIT	; Invert ID bit in saved EFLAGS
	popfq				; Load saved EFLAGS (with ID bit inverted)

	pushfq				; Save EFLAGS again (ID bit may or may not be inverted)
	pop rax				; eax = modified EFLAGS (ID bit may or may not be inverted)
	xor eax,[esp]			; eax = whichever bits were changed

	popfq				; Restore original value of EFLAGS
	and eax, EFLAGS_ID_BIT		; eax = zero if ID bit can't be changed, non-zero otherwise
	ret

; When returning, eax is 0 if CPUID is not supported and 1 if CPUID is
; supported.
is_cpuid_supported:
	pushfq				; Store original EFLAGS

	pushfq
	or dword [rsp],EFLAGS_ID_BIT	; Set bit to EFLAGS value on stack
	popfq				; Try to write modified value back to
					; register.

	pushfq
	pop rax				; Pop EFLAGS for checking if was able
					; to set ID bit.
	and eax, EFLAGS_ID_BIT
	cmp eax, EFLAGS_ID_BIT
	jne .cpuid_not_supported

	pushfq
	xor dword [rsp],EFLAGS_ID_BIT
	popfq				; ID bit should be cleared now, if its
					; modification is possible.

	pushfq
	pop rax
	and eax, EFLAGS_ID_BIT
	cmp eax, EFLAGS_ID_BIT
	je .cpuid_not_supported

.cpuid_supported:
	popfq				; Restore original EFLAGS
	mov rax, 1
	ret

.cpuid_not_supported:
	popfq				; Restore original EFLAGS
	mov rax, 0
	ret
