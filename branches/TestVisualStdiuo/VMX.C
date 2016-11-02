#include "VMX.h"
#include "VmxConfiguration.h"
#include "constants.h"

BOOLEAN VMXInit(ULONG_PTR MEM[StackSize])
{
	unsigned __int64 u64GuestRsp;
	unsigned __int64 u64GuestRip;
	unsigned __int64 u64Tmp;
	unsigned __int64 u64EntryPoint = EntryPoint;
	unsigned __int64 u64PhysicalEntryPoint = MmGetPhysicalAddress(EntryPoint);
	BOOLEAN bInit = FALSE;
	struct stHiddenPartSegmentSelector osHiddenPartSegmentSelector;
	GetGuestExit(&u64GuestRsp, &u64GuestRip);
	
	if (bInit == TRUE)
	{
		u64GuestRsp = 0;
		return TRUE;
	}
	

	DisableInterupt();
	WriteCr0(g_stVmxConfig.GuestStat.CR0.Cr0);
	WriteCr4(g_stVmxConfig.GuestStat.CR4.Cr4);
	EnableInterupt();

	ASSERT (((unsigned __int64)g_stVmxConfig.VMCS & (!0xFFF) ) == 0  ); // check for Aling address
	*(unsigned __int64 *)g_stVmxConfig.VMCS = ReadMsr(MSR_IA32_VMX_BASIC);

	ASSERT (((unsigned __int64)g_stVmxConfig.VmxOnRegion  & (!0xFFF)  ) == 0 ); // check for Aling address 24.11.5 VMXON region
	*(unsigned __int64 *)g_stVmxConfig.VmxOnRegion = ReadMsr(MSR_IA32_VMX_BASIC);

	if ( __vmx_on(&(g_stVmxConfig.VmxOnRegionPhysicalAddress)) != 0 )		// 24.11.5 VMXON region
	{
		return FALSE ;
	}
	if ( __vmx_vmclear(&(g_stVmxConfig.VMCSPhysicalAddress)) != 0 ) //  24.11.3 initilize a vmcs 
	{
		return FALSE ;
	}
	if (__vmx_vmptrld(&(g_stVmxConfig.VMCSPhysicalAddress)) != 0)
	{
		return FALSE;
	}
	// Geust state area -- intel manual - 24.4.1 Guest Register State -> 26.3.2.1 Loading Guest Control Registers, Debug Registers, and MSRs
	// For check see 26.3.1.1 Checks on Guest Control Registers, Debug Registers, and MSRs
	if ( __vmx_vmwrite(VMCS_GUEST_CR0, g_stVmxConfig.GuestStat.CR0.Cr0) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_CR3, g_stVmxConfig.GuestStat.CR3.Cr3) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_CR4, g_stVmxConfig.GuestStat.CR4.Cr4) != 0 )
	{
		return FALSE ;
	}


	if ( __vmx_vmwrite(VMCS_GUEST_RSP, u64GuestRsp ) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_RIP, u64GuestRip ) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_RFLAGS, g_stVmxConfig.GuestStat.RFLAGS.Rflage ) != 0 )
	{
		return FALSE ;
	}
	
	if ( SetSegSelector(g_stVmxConfig.GuestStat.CS, VMCS_GUEST_CS_SEL ) == FALSE )
	{
		return  FALSE ;
	}
	
	if ( SetSegSelector(g_stVmxConfig.GuestStat.SS , VMCS_GUEST_SS_SEL ) == FALSE )
	{
		return  FALSE ;
	}

	if ( SetSegSelector(g_stVmxConfig.GuestStat.DS , VMCS_GUEST_DS_SEL ) == FALSE )
	{
		return  FALSE ;
	}

	if ( SetSegSelector(g_stVmxConfig.GuestStat.ES , VMCS_GUEST_ES_SEL ) == FALSE )
	{
		return  FALSE ;
	}

	if ( SetSegSelector(g_stVmxConfig.GuestStat.FS , VMCS_GUEST_FS_SEL ) == FALSE )
	{
		return  FALSE ;
	}

	if ( SetSegSelector(g_stVmxConfig.GuestStat.GS , VMCS_GUEST_GS_SEL ) == FALSE )
	{
		return  FALSE ;
	}

	if ( SetSegSelector(g_stVmxConfig.GuestStat.LDR , VMCS_GUEST_LDTR_SEL ) == FALSE )
	{
		return  FALSE ;
	}

	if ( SetSegSelector(g_stVmxConfig.GuestStat.TR , VMCS_GUEST_TR_SEL ) == FALSE )
	{
		return  FALSE ;
	}
	 																
	if ( __vmx_vmwrite(VMCS_GUEST_GDTR_BASE, g_stVmxConfig.GuestStat.GDT.base ) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_GDTR_LIMIT, g_stVmxConfig.GuestStat.GDT.limit ) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_IDTR_BASE, g_stVmxConfig.GuestStat.IDT.base ) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_IDTR_LIMIT, g_stVmxConfig.GuestStat.IDT.limit ) != 0 )
	{
		return FALSE ;
	}

	// see 17.4.1 IA32_DEBUGCTL MSR to read about IA_DEBUGCTL
	if ( __vmx_vmwrite(VMCS_GUEST_IA32_DEBUGCTL, ReadMsr(MSR_IA32_DEBUGCTL) & 0xFFFFFFFF ) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_IA32_DEBUGCTL_HIGH, ReadMsr(MSR_IA32_DEBUGCTL) >> 32 ) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_IA32_SYSENTER_CS, g_stVmxConfig.GuestStat.MSRStar.SyscallCs & 0xFFFFFFFF ) != 0 )
	{
		return FALSE ;
	}

	
	if ( __vmx_vmwrite(VMCS_GUEST_IA32_SYSENTER_EIP,g_stVmxConfig.GuestStat.MSRRIP) != 0 )
	{
		return FALSE ;
	}
	
	if ( __vmx_vmwrite(VMCS_GUEST_IA32_SYSENTER_ESP,g_stVmxConfig.GuestStat.MSRRSP) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_FS_BASE,ReadMsr(MSR_IA32_FS_BASE)) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_GUEST_GS_BASE,ReadMsr(MSR_IA32_GS_BASE)) != 0 )
	{
		return FALSE ;
	}
	
	if ( __vmx_vmwrite(VMCS_VMCS_LINK_POINTER,-1) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_VMCS_LINK_POINTER_HIGH,-1) != 0 )
	{
		return FALSE ;
	}
    // Geust state area -- intel manual - 24.5 HOST-STATE AREA State -> 26.3.2.1 Loading Guest Control Registers, Debug Registers, and MSRs
	// For check see 26.3.1.1 Checks on Guest Control Registers, Debug Registers, and MSRs
	if ( __vmx_vmwrite(VMCS_HOST_CR0, g_stVmxConfig.GuestStat.CR0.Cr0) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_CR3, g_stVmxConfig.GuestStat.CR3.Cr3) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_CR4, g_stVmxConfig.GuestStat.CR4.Cr4) != 0 )
	{
		return FALSE ;
	}
	if (__vmx_vmwrite(VMCS_HOST_RSP, (ULONG_PTR)MEM + StackSize) != 0)
	{
		return FALSE ;
	}
	
	if (__vmx_vmwrite(VMCS_HOST_RIP, u64EntryPoint) != 0)
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_CS_SEL, g_stVmxConfig.GuestStat.CS.Val) != 0 )
	{
		return FALSE ;
	}
	
	if ( __vmx_vmwrite(VMCS_HOST_DS_SEL, g_stVmxConfig.GuestStat.DS.Val) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_HOST_ES_SEL, g_stVmxConfig.GuestStat.ES.Val) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_HOST_SS_SEL, g_stVmxConfig.GuestStat.SS.Val) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_HOST_FS_SEL, g_stVmxConfig.GuestStat.FS.Val) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_HOST_GS_SEL, g_stVmxConfig.GuestStat.GS.Val) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_HOST_TR_SEL, g_stVmxConfig.GuestStat.TR.Val) != 0 )
	{
		return FALSE ;
	}

	if ( __vmx_vmwrite(VMCS_HOST_FS_BASE, ReadMsr(MSR_IA32_FS_BASE)) != 0 )
	{
		return FALSE ;
	}
	
	GetHiddenPartSegmentSelector( g_stVmxConfig.GuestStat.TR, &osHiddenPartSegmentSelector);
	if ( __vmx_vmwrite(VMCS_HOST_TR_BASE, osHiddenPartSegmentSelector.BaseAddress) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_GDTR_BASE, g_stVmxConfig.GuestStat.GDT.base) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_IDTR_BASE, g_stVmxConfig.GuestStat.IDT.base) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_IA32_SYSENTER_CS, g_stVmxConfig.GuestStat.MSRStar.SyscallCs & 0xFFFFFFFF) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_IA32_SYSENTER_EIP, g_stVmxConfig.GuestStat.MSRRIP) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_HOST_IA32_SYSENTER_ESP, g_stVmxConfig.GuestStat.MSRRSP) != 0 )
	{
		return FALSE ;
	}

	//24.6 VM-EXECUTION CONTROL FIELDS
	//24.6.1 Pin-Based VM-Execution Controls
	if ( __vmx_vmwrite(VMCS_VMENTRY_CTL_LOAD_IA32_PAT_BIT, g_stVmxConfig.PinBaseCtrl) != 0 )
	{
		return FALSE ;
	}
	//24.6.2 Processor-Based VM-Execution Controls
	//g_stVmxConfig.ProcessBaseCtrl |= 1 << 23; // Enable MOV-DR exiting
	if ( __vmx_vmwrite(VMCS_PROC_BASED_VMEXEC_CTL, g_stVmxConfig.ProcessBaseCtrl) != 0 )
	{
		return FALSE ;
	}
	//24.6.6 Guest/Host Masks and Read Shadows for CR0 and CR4
	u64Tmp = CR0_PG;
	if ( __vmx_vmwrite(VMCS_CR0_READ_SHADOW, u64Tmp) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_CR4_READ_SHADOW, 0) != 0 )
	{
		return FALSE ;
	}
	//24.6.7 CR3-Target Controls
	if ( __vmx_vmwrite(VMCS_CR3_TARGET_COUNT, 0) != 0 )
	{
		return FALSE ;
	}
	//24.7 VM-EXIT CONTROL FIELDS
	if ( __vmx_vmwrite(VMCS_VMEXIT_CTL, g_stVmxConfig.ExitCtrl) != 0 )
	{
		return FALSE ;
	}
	//24.7.2 VM-Exit Controls for MSRs
	if ( __vmx_vmwrite(VMCS_VMEXIT_MSR_STORE_COUNT, 0) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_VMEXIT_MSR_LOAD_COUNT, 0) != 0 )
	{
		return FALSE ;
	}
	//24.8 VM-ENTRY CONTROL FIELDS
	if ( __vmx_vmwrite(VMCS_VMENTRY_CTL, g_stVmxConfig.GetEntryCtrl) != 0 )
	{
		return FALSE ;
	}
	//24.8.2 VM-Entry Controls for MSRs
	if ( __vmx_vmwrite(VMCS_VMENTRY_MSR_LOAD_COUNT, 0) != 0 )
	{
		return FALSE ;
	}
	if (__vmx_vmwrite(VMCS_EXCEPTION_BMP, 1 << EXCEPTION_DB) != 0)
	{
		return FALSE;
	}
	g_u64DR3VAl = ReadDR3();
	g_u64DR7VAl = ReadDR7();
	{
		union DR7 osDR7 ;
		osDR7.VAl = g_u64DR7VAl;
		osDR7.G3 = 1;
		osDR7.L3 = 1;
		osDR7.RW3 = 3;
		osDR7.LEN3 = 3;
		osDR7.REv1 = 1;
		WriteDR3((unsigned __int64)&TestDR);
		if (__vmx_vmwrite(VMCS_GUEST_DR7, osDR7.VAl) != 0) // about DR7 Format -- intel Manual 17.2.24 Debug control register
		{
			return FALSE;
		}
	}
	bInit = TRUE;
	__vmx_vmlaunch();
	return FALSE;
}
BOOLEAN SetSegSelector( 
	Selector i_u16SegSelector,
	unsigned __int32 i_u32SegField 
	)
{
	
	
	unsigned __int32 u32Index = (i_u32SegField - VMCS_GUEST_ES_SEL);
	struct stHiddenPartSegmentSelector osHiddenPartSegmentSelector;

	if (i_u16SegSelector.Val == 0)
	{
		RtlZeroMemory(&osHiddenPartSegmentSelector, sizeof(struct stHiddenPartSegmentSelector));
		osHiddenPartSegmentSelector.Right.P = 1;
		if (i_u32SegField == VMCS_GUEST_LDTR_SEL)
		{
			osHiddenPartSegmentSelector.Right.Type = 2;
			osHiddenPartSegmentSelector.Right.S = 0;
			osHiddenPartSegmentSelector.Right.U = 1;
		}
		if (__vmx_vmwrite(VMCS_GUEST_ES_SEL + u32Index, i_u16SegSelector.Val) != 0)
		{
			return FALSE;
		}
		if (__vmx_vmwrite(VMCS_GUEST_ES_LIMIT + u32Index, 0) != 0)
		{
			return FALSE;
		}
		if (__vmx_vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS + u32Index, osHiddenPartSegmentSelector.Right.Right) != 0)
		{
			return FALSE;
		}
		if (__vmx_vmwrite(VMCS_GUEST_ES_BASE + u32Index, 0) != 0)
		{
			return FALSE;
		}
		return TRUE;
	}
	GetHiddenPartSegmentSelector( i_u16SegSelector, &osHiddenPartSegmentSelector);
	/*if (i_u32SegField == VMCS_GUEST_GS_SEL)
	{
		osHiddenPartSegmentSelector.BaseAddress = 0xfffff80001179000;
	}*/
	if ( __vmx_vmwrite(VMCS_GUEST_ES_LIMIT + u32Index, osHiddenPartSegmentSelector.Limit ) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_GUEST_ES_ACCESS_RIGHTS + u32Index, osHiddenPartSegmentSelector.Right.Right ) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_GUEST_ES_SEL + u32Index, osHiddenPartSegmentSelector.Selector ) != 0 )
	{
		return FALSE ;
	}
	if ( __vmx_vmwrite(VMCS_GUEST_ES_BASE + u32Index, osHiddenPartSegmentSelector.BaseAddress ) != 0 )
	{
		return FALSE ;
	}
	return TRUE ;
}

void GetHiddenPartSegmentSelector(Selector i_u16Selector,
								 struct  stHiddenPartSegmentSelector * io_posHiddenPartSegmentSelector)
{
	struct stSegmentDescriptor *posSegmentDescriptor;
	RtlZeroMemory(io_posHiddenPartSegmentSelector, sizeof(struct stHiddenPartSegmentSelector));
	if (i_u16Selector.TI == 1)
	{
		unsigned __int64	u64LDTBaseAddress;
		struct stSystemDescriptor *posSystemDescriptor;
		if (g_stVmxConfig.GuestStat.LDR.Index == 0)
		{
			return;
		}
		posSystemDescriptor = (struct stSystemDescriptor *)((unsigned char*)g_stVmxConfig.GuestStat.GDT.base +
			(g_stVmxConfig.GuestStat.LDR.Index) * 8);
		u64LDTBaseAddress = posSystemDescriptor->Base32_63; 
		u64LDTBaseAddress = posSystemDescriptor->Base0_15 | (posSystemDescriptor->Base23_16 << 16)
			| (posSystemDescriptor->Base31_24 << 24) | (u64LDTBaseAddress << 32);

		posSystemDescriptor = (struct stSegmentDescriptor *)((unsigned char*)u64LDTBaseAddress +(i_u16Selector.Index) *8);
	}
	else
	{
		posSegmentDescriptor = (struct stSegmentDescriptor *)((unsigned char*)g_stVmxConfig.GuestStat.GDT.base +
			(i_u16Selector.Index) * 8);
	}
	
	io_posHiddenPartSegmentSelector->Selector = i_u16Selector.Val ;
	io_posHiddenPartSegmentSelector->Limit  = (posSegmentDescriptor->SegLimit0_15 | (posSegmentDescriptor->SegLimit19_16 << 16 ));
	
	
	// see Intel manual  3.5.2 Segment Descriptor Table in IA-32e
	//and see 7.2.3 TSS Descriptor in 64-bit mode
	if (posSegmentDescriptor->S == 0)
	{
		struct stSystemDescriptor *posSystemDescriptor = (struct stSystemDescriptor *) posSegmentDescriptor;
		io_posHiddenPartSegmentSelector->BaseAddress = posSystemDescriptor->Base32_63 ; 
		io_posHiddenPartSegmentSelector->BaseAddress = posSegmentDescriptor->Base0_15 | (posSegmentDescriptor->Base23_16 << 16)
			| (posSegmentDescriptor->Base31_24 << 24) | (io_posHiddenPartSegmentSelector->BaseAddress << 32);
	}
	else
	{
		io_posHiddenPartSegmentSelector->BaseAddress = posSegmentDescriptor->Base0_15 | (posSegmentDescriptor->Base23_16 << 16)
			| (posSegmentDescriptor->Base31_24 << 24);
	}

	if ( posSegmentDescriptor->G == 1 ) 
	{
		io_posHiddenPartSegmentSelector->Limit = (io_posHiddenPartSegmentSelector->Limit << 12 ) | 0xFFFF ;
	}
	if ( i_u16Selector.Val == 0 ) 
	{
		io_posHiddenPartSegmentSelector->Right.U = 1 ;
	}
	else
	{
		io_posHiddenPartSegmentSelector->Right.DPL = posSegmentDescriptor->DPL ; 
		io_posHiddenPartSegmentSelector->Right.AVL = posSegmentDescriptor->AVL ;
		io_posHiddenPartSegmentSelector->Right.D_B = posSegmentDescriptor->D_B ; 
		io_posHiddenPartSegmentSelector->Right.G = posSegmentDescriptor->G ;
		io_posHiddenPartSegmentSelector->Right.L = posSegmentDescriptor->L;
		io_posHiddenPartSegmentSelector->Right.S = posSegmentDescriptor->S;
		io_posHiddenPartSegmentSelector->Right.P = posSegmentDescriptor->P ;
		io_posHiddenPartSegmentSelector->Right.Type = posSegmentDescriptor->Type ;
	}
	return ;
}
void VMXEntryPoint(ULONG_PTR MEM[StackSize])
{
	//foundTopStack;
	//FoundCallBacks
	//CAllCallBack
	//finish
}