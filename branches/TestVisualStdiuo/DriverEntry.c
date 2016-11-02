#include "wdm.h"
#include "constants.h"
#include "Definition.h"
#include "VmxConfiguration.h"
#include "VirtualMachine.h"
#include "VMX.h"

BOOLEAN IsVirtualizationEnabled();
BOOLEAN IsVirtualizationLocked();
BOOLEAN CanVmxON();
unsigned __int32 TestDR = 0;
unsigned __int32 TestDR2 = 0;
NTSTATUS 
DriverEntry(
    IN PDRIVER_OBJECT DriverObject, 
    IN PUNICODE_STRING RegistryPath 
    )
{
	ULONG_PTR * hvStack ;
	LARGE_INTEGER HighestAcceptableAddress;
	ULONG tmp = sizeof(ULONG);
	__outbyte(0xe9, 'c');
	if (CanVmxON() == FALSE )
	{
		return STATUS_FAIL_CHECK;
	}
	HighestAcceptableAddress.QuadPart = 0x0000000000FFFFFF;
	hvStack = (ULONG_PTR*)(MmAllocateContiguousMemory(StackSize, HighestAcceptableAddress));
	VmInit(hvStack);
	SetConfig();
	VMXInit(hvStack);
	{
		union DR7 osDR7 = { 0 };
		osDR7.G2 = 1;
		osDR7.L2 = 1;
		osDR7.RW2 = 3;
		osDR7.LEN2 = 3;
		osDR7.REv1 = 1;
		WriteDR3((unsigned __int64)&TestDR2);
		WriteDR7(osDR7.VAl);
		TestDR2 = 0xffffffff;
		TestDR2 = 0xffffffff;
	}
	
	return  STATUS_SUCCESS ;
}
BOOLEAN CanVmxON()
{
	if (!IsVirtualizationEnabled())
	{
		if ( IsVirtualizationLocked() ) 
		{
			DbgPrint("Virtualizing not enable and lock");
		}
		else
		{
			DbgPrint("Virtualizing not enable and not locked");
		}
		return FALSE ;
	}
	if (IsVirtualizationLocked())
	{
		__writemsr(MSR_IA32_FEATURE_CONTROL, (ReadMsr(MSR_IA32_FEATURE_CONTROL) | MSR_IA32_FEATURE_CONTROL_LOCK_BIT));
	}
	return TRUE ;
}
BOOLEAN IsVirtualizationEnabled()
{
	return (0 != (ReadMsr(MSR_IA32_FEATURE_CONTROL) & MSR_IA32_FEATURE_CONTROL_VMXON_BIT));
}
BOOLEAN IsVirtualizationLocked()
{
	return ( 0 == (ReadMsr(MSR_IA32_FEATURE_CONTROL) & MSR_IA32_FEATURE_CONTROL_LOCK_BIT));
}
