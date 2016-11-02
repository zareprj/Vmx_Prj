#include "VirtualMachine.h"
#include "constants.h"
#include "Definition.h"
BOOLEAN ThrowPassEvent();
void HandleCrxAccess(union RegisterBank *i_posRegisterBank);
unsigned __int64 g_u64DR3VAl;
unsigned __int64 g_u64DR7VAl;
void VmInit(unsigned __int64 *MEM)
{
	unsigned int index ;
	MEM[0] = kStackMark;
	for (index = 0 ;index < MAX_CALLBACK ; index ++)
	{
		if ((index > EXIT_REASON_VMCALL) && (index <= EXIT_REASON_VMXON))
		{
			MEM[index + 1] = (unsigned __int64)EM_dummy;
		}
		else
		{
			switch(index)
				{
				case EXIT_REASON_VMCALL:
					MEM[index + 1] = (unsigned __int64)EM_vmcall;
					break;
				case EXIT_REASON_RDMSR:
					MEM[index + 1] = (unsigned __int64)EM_rdmsr;
					break;
				case EXIT_REASON_WRMSR:
					MEM[index + 1] = (unsigned __int64)EM_wrmsr;
					break;			
				case EXIT_REASON_INVD:
					MEM[index + 1] = (unsigned __int64)EM_invd;
					break;
				case EXIT_REASON_CPUID:
					MEM[index + 1] = (unsigned __int64)EM_cpuid;
					break;
				case EXIT_REASON_RDTSC:
					MEM[index + 1] = (unsigned __int64)EM_rdtsc;
					break;
				default:
					MEM[index + 1] = (unsigned __int64)EM_dummy;
				}
		}
	}
}
__forceinline ULONG_PTR* GetTopOfStack(  const ULONG_PTR* stack	)
{
	/*ULONG_PTR* Tmpstack = stack;
	while (kStackMark != *stack && Tmpstack - stack < StackSize + 4)
		stack =  (ULONG_PTR*)(ALIGN(stack - 1, PAGE_SIZE));*/

	return (ULONG_PTR*)((unsigned __int8 *)stack - StackSize);
}
void PrintToBochse(char *i_strMsg)
{
	int iLen = strlen(i_strMsg);
	int iIndex = 0;
	for (; iIndex < iLen; iIndex++)
	{
		__outbyte(0xe9, i_strMsg[iIndex]);
	}
}

unsigned __int64 VMEntryPoint(ULONG_PTR *MEM, unsigned __int64 * stack_top)
{
	unsigned __int16 u16ds;
	unsigned __int32 u32ExitReason;
	unsigned __int32 u32ExitInstructionLength;
	unsigned __int64 u64GuestEIP;
	unsigned __int64 u64RFlage;
	unsigned __int64 u64Ret ;
	unsigned __int16 u16LdtrSel;
	unsigned __int64 u64Tmp;
	unsigned __int64 eaxTemp;
	/*char debugMessage[150];
	sprintf(debugMessage, "Status rax = %llu rcx = %llu", ((union RegisterBank *)MEM)->Rax, ((union RegisterBank *)MEM)->Rcx);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " rdx = %llu rbx = %llu", ((union RegisterBank *)MEM)->Rdx, ((union RegisterBank *)MEM)->Rbx);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " rsp = %llu rbp = %llu", ((union RegisterBank *)MEM)->Rsp, ((union RegisterBank *)MEM)->Rbp);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " rsp = %llu rbp = %llu", ((union RegisterBank *)MEM)->Rsi, ((union RegisterBank *)MEM)->Rdi);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r8 = %llu r9 = %llu", ((union RegisterBank *)MEM)->R8, ((union RegisterBank *)MEM)->R9);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r10 = %llu r11 = %llu", ((union RegisterBank *)MEM)->R10, ((union RegisterBank *)MEM)->R11);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r12 = %llu r13 = %llu", ((union RegisterBank *)MEM)->R12, ((union RegisterBank *)MEM)->R13);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r14 = %llu r15 = %llu \n", ((union RegisterBank *)MEM)->R14, ((union RegisterBank *)MEM)->R15);
	PrintToBochse(debugMessage);*/
	
	stack_top = ((char *)stack_top - StackSize);
	if ( __vmx_vmread(VMCS_GUEST_DS_SEL, &u16ds) != 0 )
	{
		return 0;
	}
	xchgds(&u16ds);
	if ( __vmx_vmread(VMCS_EXIT_REASON,&u32ExitReason) != 0 )
	{
		writeds(u16ds);
		return 0;
	}
	/*sprintf(debugMessage, "Exit Reason = %u \n", u32ExitReason);
	PrintToBochse(debugMessage);*/

	if (u32ExitReason == EXIT_REASON_EXCEPTION_OR_NMI)
	{
		union DR6 osDR6 = { 0 };
		union DebugTrap osDebugTrap = { 0 };
		union ExitQuelDebugExeption osExitQuelDebugExeption = { 0 };
		if (__vmx_vmread(VMCS_EXIT_QUALIFICATION, &osExitQuelDebugExeption.VAL) != 0)
		{
			return;
		}
		if (osExitQuelDebugExeption.BreakPointIndicatot == 3)
		{
			osDR6.Rev1 = 0xff;
			osDR6.Rev2 = 0xffff;
			WriteDR6(osDR6.VAl);
			u64Ret = EM_dummy;
			return u64Ret;
		}
		osDR6.BD = osExitQuelDebugExeption.BD;
		osDR6.BS = osExitQuelDebugExeption.BS;
		osDR6.BreakPointIndicatot = osExitQuelDebugExeption.BreakPointIndicatot;
		osDR6.Rev1 = 0xff;
		osDR6.Rev2 = 0xffff;
		WriteDR6(osDR6.VAl);
		ThrowPassEvent();
		u64Ret = EM_dummy;
		return u64Ret;
	}
	
	if ( __vmx_vmread(VMCS_VMEXIT_INSTRUCTION_LEN,&u32ExitInstructionLength) != 0 )
	{
		writeds(u16ds);
		return 0;
	}
	if( __vmx_vmread(VMCS_GUEST_RIP, &u64GuestEIP) != 0)
	{
		writeds(u16ds);
		return 0;
	}
	__vmx_vmwrite(VMCS_GUEST_RIP,u64GuestEIP+u32ExitInstructionLength);
	
	
	if((u32ExitReason > EXIT_REASON_VMCALL) && (u32ExitReason <= EXIT_REASON_VMXON))
	{
		if (__vmx_vmread(VMCS_GUEST_RFLAGS,&u64RFlage) != 0) 
		{
			writeds(u16ds);
			return 0;
		}
		__vmx_vmwrite(VMCS_GUEST_RFLAGS,u64RFlage & (~0x8d5) | 0x1);
	}else if (u32ExitReason == EXIT_REASON_MOV_CR) 
	{
		
		/*sprintf(debugMessage, "Rip = %llu \n", u64GuestEIP);
		PrintToBochse(debugMessage);*/
		HandleCrxAccess((union RegisterBank *)MEM);
		u64Ret = EM_dummy;
	}
	else if (u32ExitReason == EXIT_REASON_MOV_DR)
	{
		union ExitQuelDebugRegisterAccess osExitQuelDebugRegisterAccess = { 0 };
		if (__vmx_vmread(VMCS_EXIT_QUALIFICATION, &osExitQuelDebugRegisterAccess.VAL) != 0)
		{
			return;
		}
		if (osExitQuelDebugRegisterAccess.NumberOfDebugRegister == 3)
		{
			if (osExitQuelDebugRegisterAccess.Direction == 0) // Mov to DR
			{
				g_u64DR3VAl = ((union RegisterBank *)MEM)->Reg[osExitQuelDebugRegisterAccess.Register];
				WriteDR3(g_u64DR3VAl);
			}
			else
			{
				g_u64DR3VAl = ReadDR3();
				((union RegisterBank *)MEM)->Reg[osExitQuelDebugRegisterAccess.Register] = g_u64DR3VAl;
			}
		}
		else if (osExitQuelDebugRegisterAccess.NumberOfDebugRegister == 7)
		{
			if (osExitQuelDebugRegisterAccess.Direction == 0)
			{
				union DR7 osDR7 ;
				g_u64DR7VAl = osDR7.VAl = ((union RegisterBank *)MEM)->Reg[osExitQuelDebugRegisterAccess.Register];
				/*osDR7.G3 = 1;
				osDR7.L3 = 1;
				osDR7.RW3 = 3;
				osDR7.LEN3 = 3;
				osDR7.REv1 = 1;*/
				WriteDR7(osDR7.VAl);
			}
			else
			{
				g_u64DR7VAl = ReadDR7();
				((union RegisterBank *)MEM)->Reg[osExitQuelDebugRegisterAccess.Register] = g_u64DR7VAl;
			}
		}
		u64Ret = EM_dummy;
	}
	else if (u32ExitReason >= MAX_CALLBACK)
	{
		u64Ret = EM_dummy;
	}
	else
		u64Ret = stack_top[u32ExitReason + 1];
	/*sprintf(debugMessage, "Status rax = %llu rcx = %llu", ((union RegisterBank *)MEM)->Rax, ((union RegisterBank *)MEM)->Rcx);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " rdx = %llu rbx = %llu", ((union RegisterBank *)MEM)->Rdx, ((union RegisterBank *)MEM)->Rbx);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " rsp = %llu rbp = %llu", ((union RegisterBank *)MEM)->Rsp, ((union RegisterBank *)MEM)->Rbp);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " rsp = %llu rbp = %llu", ((union RegisterBank *)MEM)->Rsi, ((union RegisterBank *)MEM)->Rdi);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r8 = %llu r9 = %llu", ((union RegisterBank *)MEM)->R8, ((union RegisterBank *)MEM)->R9);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r10 = %llu r11 = %llu", ((union RegisterBank *)MEM)->R10, ((union RegisterBank *)MEM)->R11);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r12 = %llu r13 = %llu", ((union RegisterBank *)MEM)->R12, ((union RegisterBank *)MEM)->R13);
	PrintToBochse(debugMessage);
	sprintf(debugMessage, " r14 = %llu r15 = %llu \n", ((union RegisterBank *)MEM)->R14, ((union RegisterBank *)MEM)->R15);
	PrintToBochse(debugMessage);*/
	writeds(u16ds);
////	__vmx_vmread(VMCS_GUEST_LDTR_SEL, &u16LdtrSel);
//	if (u16LdtrSel == 0)
//	{
//		u64Tmp = 0;
//		__vmx_vmwrite(VMCS_GUEST_LDTR_BASE, u64Tmp);
//		u32ExitInstructionLength = 0;
//		__vmx_vmwrite(VMCS_GUEST_LDTR_LIMIT, u32ExitInstructionLength);
//		__vmx_vmwrite(VMCS_GUEST_LDTR_ACCESS_RIGHTS, u32ExitInstructionLength);
//	}
	return u64Ret;

}
void HandleCrxAccess(union RegisterBank *i_posRegisterBank)
{
	unsigned __int64 u64Tmp;
	union
	{
		struct exit_qual_cr s;
		unsigned __int64 v;
	} eqc;
	char debugMessage[100];
	unsigned __int32 ui32RegNum;
	unsigned __int64 ui64Ctlr;
	if (__vmx_vmread(VMCS_EXIT_QUALIFICATION,&eqc.v) != 0) 
	{
			return ;
	}
	switch (eqc.s.type)
	{
	case EXIT_QUAL_CR_TYPE_MOV_TO_CR:
		
		if (eqc.s.num == 0)
			ui32RegNum = VMCS_GUEST_CR0;
		else if (eqc.s.num == 3)
			ui32RegNum = VMCS_GUEST_CR3;
		else if (eqc.s.num == 4)
			ui32RegNum = VMCS_GUEST_CR4;
		u64Tmp = i_posRegisterBank->Reg[eqc.s.reg];
		/*sprintf(debugMessage, "Des %d reg val  %llu \n", eqc.s.reg, u64Tmp);
		PrintToBochse(debugMessage);*/
		__vmx_vmwrite(ui32RegNum, u64Tmp);
		
		break;
	case EXIT_QUAL_CR_TYPE_MOV_FROM_CR:
		if (eqc.s.num == 0)
			ui32RegNum = VMCS_GUEST_CR0;
		else if (eqc.s.num == 3)
			ui32RegNum = VMCS_GUEST_CR3;
		else if (eqc.s.num == 4)
			ui32RegNum = VMCS_GUEST_CR4;
		__vmx_vmread(ui32RegNum, &ui64Ctlr);
			u64Tmp = i_posRegisterBank->Reg[eqc.s.reg];
			i_posRegisterBank->Reg[eqc.s.reg] = ui64Ctlr;
		/*	sprintf(debugMessage, "Src %d reg oldval %llu new val \n", eqc.s.reg, u64Tmp, ui64Ctlr);
			PrintToBochse(debugMessage);*/
		break;
	case EXIT_QUAL_CR_TYPE_CLTS:
		{
			union stCR0 cr0;
			__vmx_vmread(VMCS_GUEST_CR0,&cr0.Cr0);
			cr0.TS = 0;
			__vmx_vmwrite(VMCS_GUEST_CR0 ,cr0.Cr0);
		}
		break;
	case EXIT_QUAL_CR_TYPE_LMSW:
		{
			union stCR0 cr0;
			__vmx_vmread(VMCS_GUEST_CR0,&cr0.Cr0);
			cr0.Cr0 &= ~0xFFFF;
			cr0.Cr0	|= eqc.s.lmsw_src;
			__vmx_vmwrite(VMCS_GUEST_CR0 ,cr0.Cr0);
		}
		break;
	default:
		;
	}
}
BOOLEAN ThrowPassEvent()
{
	union vmExitIntruptInfo osExitIntruptInfo;
	unsigned __int32 u32ExitInstructionLength;
	unsigned __int32 u32ErrorCode;
	if (__vmx_vmread(VMCS_VMEXIT_INTR_INFO, &osExitIntruptInfo.Val) != 0)
	{	
		return FALSE;
	}
	if (__vmx_vmread(VMCS_VMEXIT_INTR_ERRCODE, &u32ErrorCode) != 0)
	{
		return FALSE;
	}
	if (__vmx_vmread(VMCS_VMEXIT_INSTRUCTION_LEN, &u32ExitInstructionLength) != 0)
	{
		return FALSE;
	}
	osExitIntruptInfo.Reservd = 0;
	__vmx_vmwrite(VMCS_VMENTRY_INTR_INFO_FIELD, osExitIntruptInfo.Val);
	__vmx_vmwrite(VMCS_VMENTRY_EXCEPTION_ERRCODE, u32ErrorCode);
	__vmx_vmwrite(VMCS_VMENTRY_INSTRUCTION_LEN, u32ExitInstructionLength);
	return TRUE; 
}