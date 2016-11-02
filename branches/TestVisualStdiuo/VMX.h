#pragma once 
#include "wdm.h"
#include "Definition.h"
extern unsigned __int64 g_u64DR3VAl;
extern unsigned __int64 g_u64DR7VAl;
extern unsigned __int32 TestDR;
#pragma pack (push,1)
struct stHiddenPartSegmentSelector //Intel manual -- 24.4.1  Guest register state 
{
	unsigned __int16	Selector ;
	unsigned __int64	BaseAddress;
	unsigned __int32    Limit;
	union //Format Of access right // 24.4.1 Guest register state
	{
		unsigned __int32 Right;
		struct 
		{
			unsigned __int32 Type :4;
			unsigned __int32 S :1 ;
			unsigned __int32 DPL : 2 ;
			unsigned __int32 P : 1 ;
			unsigned __int32 Reserved : 4 ;
			unsigned __int32 AVL : 1 ;
			unsigned __int32 L : 1 ;
			unsigned __int32 D_B : 1 ;
			unsigned __int32 G : 1 ;
			unsigned __int32 U : 1 ; // segment unusable  ( 0 = usable , 1 = unsable )
		};
	}Right;
};
#pragma pack (pop)
BOOLEAN VMXInit(ULONG_PTR MEM[StackSize]);
	
BOOLEAN SetSegSelector(Selector i_u16SegSelector, unsigned __int32 i_u32SegField);

void GetHiddenPartSegmentSelector(Selector i_u16Selector, struct stHiddenPartSegmentSelector * io_posstHiddenPartSegmentSelector);
