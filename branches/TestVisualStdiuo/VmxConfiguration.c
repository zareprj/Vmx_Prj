#include "VmxConfiguration.h"
#include "constants.h"

struct VmxConfig g_stVmxConfig ;
void SetConfig()
{
	RtlZeroMemory (&g_stVmxConfig , sizeof(struct VmxConfig));
	g_stVmxConfig.PinBaseCtrl = GetPinBaseCtrl() ;
	g_stVmxConfig.ProcessBaseCtrl = GetProcessBaseCtrl();
	g_stVmxConfig.ExitCtrl = GetExitCtrl();
	g_stVmxConfig.GetEntryCtrl = GetEntryCtrl();
	g_stVmxConfig.VmxOnRegion = MmAllocateNonCachedMemory(PAGE_SIZE);
	RtlZeroMemory(g_stVmxConfig.VmxOnRegion, PAGE_SIZE);
	g_stVmxConfig.VmxOnRegionPhysicalAddress = MmGetPhysicalAddress(g_stVmxConfig.VmxOnRegion); 
	g_stVmxConfig.VMCS = MmAllocateNonCachedMemory(PAGE_SIZE);
	RtlZeroMemory(g_stVmxConfig.VMCS, PAGE_SIZE);
	g_stVmxConfig.VMCSPhysicalAddress = MmGetPhysicalAddress(g_stVmxConfig.VMCS);
	GetGustState ();
}

void GetGustState ()
{
	g_stVmxConfig.GuestStat.CR0.Cr0 = ReadCr0();
	g_stVmxConfig.GuestStat.CR0.Cr0 = g_stVmxConfig.GuestStat.CR0.Cr0 & ReadMsr(MSR_IA32_VMX_CR0_FIXED1) | ReadMsr(MSR_IA32_VMX_CR0_FIXED0) | CR0_PE_BIT | CR0_NE_BIT | CR0_PG_BIT;
	g_stVmxConfig.GuestStat.CR4.Cr4 = ReadCr4();
	g_stVmxConfig.GuestStat.CR4.Cr4 = g_stVmxConfig.GuestStat.CR4.Cr4 & ReadMsr(MSR_IA32_VMX_CR4_FIXED1) | ReadMsr(MSR_IA32_VMX_CR4_FIXED0) | CR4_VMXE_BIT | CR4_DE_BIT;
	g_stVmxConfig.GuestStat.CR3.Cr3 = ReadCr3();
	g_stVmxConfig.GuestStat.RFLAGS.Rflage = ReadFlag();
	g_stVmxConfig.GuestStat.CS.Val = ReadCS();
	g_stVmxConfig.GuestStat.DS.Val = ReadDS();
	g_stVmxConfig.GuestStat.SS.Val = ReadSS();
	g_stVmxConfig.GuestStat.ES.Val = ReadES();
	g_stVmxConfig.GuestStat.FS.Val = ReadFS();
	g_stVmxConfig.GuestStat.GS.Val = ReadGS();
	g_stVmxConfig.GuestStat.MSRRIP = ReadMsr(MSR_IA32_SYSENTER_EIP) & 0xFFFFFFFF ;
	g_stVmxConfig.GuestStat.MSRRSP = ReadMsr(MSR_IA32_SYSENTER_ESP) & 0xFFFFFFFF ;
	g_stVmxConfig.GuestStat.MSRStar.StarMsr = ReadMsr(MSR_IA32_STAR);
	ReadGdt(&(g_stVmxConfig.GuestStat.GDT));
	ReadIdt(&(g_stVmxConfig.GuestStat.IDT));
	ReadTr(&(g_stVmxConfig.GuestStat.TR.Val));
	ReadLdt(&(g_stVmxConfig.GuestStat.LDR.Val));
}
ULONG GetPinBaseCtrl()
{
	return ReadMsr(MSR_IA32_VMX_PINBASED_CTLS) & SEG_D_LIMIT ;
}
ULONG GetProcessBaseCtrl()
{
	return ReadMsr(MSR_IA32_VMX_PROCBASED_CTLS) & SEG_D_LIMIT | VMCS_PROC_BASED_VMEXEC_CTL_RDTSCEXIT_BIT ;
}
ULONG GetExitCtrl()
{
	return ((ReadMsr(MSR_IA32_VMX_EXIT_CTLS) & SEG_D_LIMIT | (1 << 15)) | VMCS_VMEXIT_CTL_HOST_ADDRESS_SPACE_SIZE_BIT | VMCS_VMEXIT_CTL_ACK_ITR_ON_EXIT);
}
ULONG GetEntryCtrl()
{
	return ((ReadMsr(MSR_IA32_VMX_ENTRY_CTLS)& SEG_D_LIMIT) | VMCS_VMENTRY_CTL_64_GUEST_BIT);
}

