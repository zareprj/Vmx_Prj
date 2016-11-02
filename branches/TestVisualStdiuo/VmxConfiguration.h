#pragma once
#include "Ntddk.h"
#include "wdm.h"
#include "Definition.h"


#pragma pack (push,1)
union stStarMsr
{
	unsigned __int64 StarMsr;
	struct
	{
		unsigned __int64 Reserved:0x20;
		unsigned __int64 SyscallCs:0x10;
		unsigned __int64 SysretCs:0x10;
	};
};

struct VmxConfig
{
	ULONG  PinBaseCtrl;
	ULONG  ProcessBaseCtrl;
	ULONG  ExitCtrl;
	ULONG  GetEntryCtrl;
	PHYSICAL_ADDRESS  VmxOnRegionPhysicalAddress;
	PVOID  VmxOnRegion;
	PHYSICAL_ADDRESS  VMCSPhysicalAddress;
	PVOID  VMCS;
	struct stGuestStat 
	{
		union stCR0  CR0; 
		union stCR4  CR4;
		union stCR3  CR3;
		union stRFLAGS RFLAGS;
		Selector CS;
		Selector DS;
		Selector SS;
		Selector ES;
		Selector FS;
		Selector GS;
		unsigned __int64 MSRRIP;
		unsigned __int64 MSRRSP;
		union stStarMsr        MSRStar; 
		struct DiscriptorPointer GDT; 
		struct DiscriptorPointer IDT;
		Selector TR;
		Selector LDR;
	}GuestStat;
};
#pragma pack (pop)
extern struct VmxConfig g_stVmxConfig ;
void SetConfig();
void GetGustState ();
ULONG GetPinBaseCtrl();
ULONG GetProcessBaseCtrl();
ULONG GetExitCtrl();
ULONG GetEntryCtrl();