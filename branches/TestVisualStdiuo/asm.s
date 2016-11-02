extrn VMEntryPoint:proc
.code
ReadMsr proc
	xor rax , rax 
	xor rdx , rdx 
	rdmsr
	mov cl , 32
	shl rdx,cl
	or  rdx,rax
	mov rax ,rdx
	ret
ReadMsr endp

WriteDR7 proc
	mov DR7 , rcx
	ret 
WriteDR7 endp
ReadDR7 proc
	mov rax , DR7
	ret 
ReadDR7 endp
WriteDR3 proc
	mov DR3 , rcx
	ret 
WriteDR3 endp
ReadDR3 proc
	mov rax , DR3
	ret 
ReadDR3 endp

ReadDR6 proc
	mov rax , DR6
	ret
ReadDR6 endp
WriteDR6 proc
	mov DR6 , rcx
	ret
WriteDR6 endp
ReadCr0 proc
	mov rax , cr0
	ret
ReadCr0 endp

ReadCr4 proc
	mov rax , cr4 
	ret
ReadCr4 endp

ReadCr3 proc
	mov rax , cr3 
	ret
ReadCr3 endp

ReadFlag proc
	PUSHFQ
	pop rax
	ret
ReadFlag endp

ReadCS proc
	xor rax , rax
	mov ax , cs
	ret
ReadCS endp

ReadDS proc
	xor rax , rax
	mov ax , ds
	ret
ReadDS endp

ReadSS proc
	xor rax , rax
	mov ax , ss
	ret
ReadSS endp

ReadES proc
	xor rax , rax
	mov ax , es
	ret
ReadES endp

ReadGS proc
	xor rax ,rax 
	mov ax , gs
	ret
ReadGS endp 

ReadFS proc
	xor rax ,rax 
	mov ax , fs
	ret
ReadFS endp 

ReadGdt proc
	sgdt  [rcx]
	ret
ReadGdt endp

ReadIdt proc
	sidt  [rcx]
	ret
ReadIdt endp

ReadTr proc
	str word ptr [rcx]
	ret
ReadTr endp

ReadLdt proc
	sldt word ptr [rcx]
	ret
ReadLdt endp

GetGuestExit proc
	lea rax, [rsp + sizeof(QWORD)]
	mov [rcx], rax
	
	mov rax, [rsp]
	mov [rdx], rax
		
	xor rax, rax	
	ret
GetGuestExit endp

DisableInterupt proc
	cli
	ret
DisableInterupt endp

EnableInterupt proc
	sti
	ret
EnableInterupt endp

WriteCr0 proc
	mov cr0 , rcx
	ret
WriteCr0 endp

WriteCr4 proc 
	mov cr4 , rcx
	ret
WriteCr4 endp


EM_rdmsr proc
	rdmsr
	ret
EM_rdmsr endp

EM_wrmsr proc 
	wrmsr
	ret
EM_wrmsr endp

EM_cpuid proc
	cpuid
	ret
EM_cpuid endp

EM_invd proc
	invd
	ret
EM_invd endp

EM_dummy proc
	ret
EM_dummy endp

EM_vmcall proc
	ret
EM_vmcall endp

EM_rdtsc proc
	push rcx
	mov rcx, 010h
	rdmsr;IA32_TIME_STAMP_COUNTER
	pop rcx
	ret
EM_rdtsc endp 

xchgds proc
	mov rax, ds
	mov ds, [rcx]
	mov [rcx], rax
	ret
xchgds endp

writeds proc
	mov ds, rcx
	ret
writeds endp

EntryPoint proc
	cli
	push r15
	mov r15, rsp
	add r15, sizeof(qword)
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rdi
	push rsi
	push rbp
	push r15
	push rbx
	push rdx
	push rcx
	push rax

	mov rcx, rsp		
	mov rdx,r15
	push rdx 
	push rcx 
	push rcx
	call VMEntryPoint
	pop rdx
	pop rdx 
	pop rdx
	sub rdx,4000h
	mov [rdx] , rax
	pop rax
	pop rcx
	pop rdx
	pop rbx
	add rsp, sizeof(qword) ;SKIP POP RSP 
	pop rbp
	pop rsi
	pop rdi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
	push r15 
	mov r15 , rsp
	add r15 , 8
	sub r15 , 4000h
	mov r15 , [r15]
	call r15 
	pop r15 
	sti
	vmresume
	ret
EntryPoint endp

end