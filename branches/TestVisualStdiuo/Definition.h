#pragma once
#define StackSize               0x4000
#define SEG_Q_LIMIT				0xFFFFFFFFFFFFFFFF
#define SEG_D_LIMIT				SEG_Q_LIMIT
#define ALIGN(a,b)				((ULONG)a | (ULONG)b)

#define BTS(b)					(1 << b)
#define CR4_VMXE				(ULONG)(BTS(13))
#define CR4_DE					(ULONG)(BTS(3))
#define CR0_PG					(ULONG)(BTS(31))
#define CR0_NE					(ULONG)(BTS(5))
#define CR0_PE					(ULONG)(BTS(0))
#pragma pack (push,1)
typedef union stSelector
{
	unsigned __int16 Val;
	struct
	{
		unsigned __int16 RPL : 2;
		unsigned __int16 TI : 1;
		unsigned __int16 Index : 13;
	};
}Selector;
typedef union stGeneralRegister
{
	unsigned __int64 rVal;
	struct
	{
		unsigned __int32 rev1;
		union 
		{
			unsigned __int32 xVal;
			struct
			{
				unsigned __int16 rev2;
				unsigned __int8  hVal;
				unsigned __int8  lVal;
			};

		};
	};
}GeneralRegister;
union RegisterBank
{
		unsigned __int64 Reg[16];
		struct
		{
			unsigned __int64 Rax;
			unsigned __int64 Rcx;
			unsigned __int64 Rdx;
			unsigned __int64 Rbx;
			unsigned __int64 Rsp;
			unsigned __int64 Rbp;
			unsigned __int64 Rsi;
			unsigned __int64 Rdi;
			unsigned __int64 R8;
			unsigned __int64 R9;
			unsigned __int64 R10;
			unsigned __int64 R11;
			unsigned __int64 R12;
			unsigned __int64 R13;
			unsigned __int64 R14;
			unsigned __int64 R15;
		};
};

union stCR0
{
	unsigned __int64 Cr0;
	struct 
	{
		unsigned int PE:1 ; //Protected Mode Enable 	If 1, system is in protected mode, else system is in real mode
		unsigned int MP:1 ; //Monitor co-processor 	Controls interaction of WAIT/FWAIT instructions with TS flag in CR0
		unsigned int EM:1 ; //If set, no x87 floating point unit present, if clear, x87 FPU present
		unsigned int TS:1 ; //Task switched 	Allows saving x87 task context upon a task switch only after x87 instruction used
		unsigned int ET:1 ; //Extension type 	On the 386, it allowed to specify whether the external math coprocessor was an 80287 or 80387
		unsigned int NE:1 ; //Numeric error 	Enable internal x87 floating point error reporting when set, else enables PC style x87 error detection
		unsigned int D1:10 ; //DonCare number 1
		unsigned int WP:1 ; //Write protect 	Determines whether the CPU can write to pages marked read-only
		unsigned int D2:1 ; //DonCare number 2
		unsigned int AM:1 ; //Alignment mask 	Alignment check enabled if AM set, AC flag (in EFLAGS register) set, and privilege level is 3
		unsigned int D3:10; //DonCare number 3
		unsigned int NW:1 ; //Not-write through  	Globally enables/disable write-back caching
		unsigned int CD:1 ; //Cache disable 	Globally enables/disable the memory cache
		unsigned int PG:1 ; //Paging 	If 1, enable paging and use the CR3 register, else disable paging
	};
};
union stRFLAGS
{
	unsigned __int64 Rflage;
	struct 
	{
		unsigned int CF :1 ; // Carry Flag
		unsigned int R1 :1 ; // Reserved
		unsigned int PF :1 ; // Parity flag
		unsigned int R2 :1 ; 
		unsigned int AF :1 ; // adjust flag
		unsigned int R3 :1 ;
		unsigned int ZF :1 ; // Zero flag
		unsigned int SF :1 ; // Sign flag
		unsigned int TF :1 ; // Trap flag
		unsigned int IF :1 ; // intrupt flage Enable
		unsigned int DF :1 ; // Direction flag
		unsigned int OF :1 ; // overflow flag
		unsigned int IOPL : 2 ;//  IO Pervilage flag
		unsigned int NT :1 ; //nested Task 
		unsigned int R4 :1 ;
		unsigned int RF :1 ; //resume flag
		unsigned int vm :1 ; //virtual mode 8086	
		unsigned int AC :1 ; //Alignment Check
		unsigned int VIF:1 ; //virtual interrput
		unsigned int VIP:1 ; //virtual interrrupt pending
		unsigned int ID :1 ; // able to use CPUID instruction
	};
};
union stCR4
{
	unsigned __int64 Cr4;
	struct 
	{
		unsigned int VME:1 ; //	Virtual 8086 Mode Extensions 	If set, enables support for the virtual interrupt flag (VIF) in virtual-8086 mode.
		unsigned int PVI:1 ; //Protected-mode Virtual Interrupts 	If set, enables support for the virtual interrupt flag (VIF) in protected mode.
		unsigned int TSD:1 ; //Time Stamp Disable 	If set, RDTSC instruction can only be executed when in ring 0, otherwise RDTSC can be used at any privilege level.
		unsigned int DE:1  ; //Debugging Extensions 	If set, enables debug register based breaks on I/O space access
		unsigned int PSE:1 ; //Page Size Extension 	If unset, page size is 4 KiB, else page size is increased to 4 MiB (or 2 MiB with PAE set).
		unsigned int PAE:1 ; // 	Physical Address Extension 	If set, changes page table layout to translate 32-bit virtual addresses into extended 36-bit physical addresses.
		unsigned int MCE:1 ; //Machine Check Exception 	If set, enables machine check interrupts to occur.
		unsigned int PGE:1 ; //Page Global Enabled 	If set, address translations (PDE or PTE records) may be shared between address spaces.
		unsigned int PCE:1 ; //Performance-Monitoring Counter enable 	If set, RDPMC can be executed at any privilege level, else RDPMC can only be used in ring 0.
		unsigned int OSFXSR:1 ; //Operating system support for FXSAVE and FXRSTOR instructions 	If set, enables SSE instructions and fast FPU save & restore
		unsigned int OSXMMEXCPT:1; //Operating System Support for Unmasked SIMD Floating-Point Exceptions 	If set, enables unmasked SSE exceptions.
		unsigned int D1:2 ;//DonCare number 1
		unsigned int VMXE:1 ; //Virtual Machine Extensions Enable 	see Intel VT-x
		unsigned int SMXE:1 ; //Safer Mode Extensions Enable 	see Trusted Execution Technology (TXT)
		unsigned int D2:2 ;//DonCare number 2
		unsigned int PCIDE :1 ;//PCID Enable 	If set, enables process-context identifiers (PCIDs).
		unsigned int OSXSAVE:1 ;//XSAVE and Processor Extended States Enable
		unsigned int D3:1 ;//DonCare number 3
		unsigned int SMEP:1 ;//Supervisor Mode Execution Protection Enable 	If set, execution of code in a higher ring generates a fault
		unsigned int SMAP:1 ;//Supervisor Mode Access Protection Enable 	If set, access of data in a higher ring generates a fault[1]
	};
};
typedef enum enSegmentType
{
	UpTo16 = 0 ,Reserved1=1,LDT=2,Reserved2=3,Reserved3=4,Reserved4=5,Reserved5=6,Reserved6=7,Reserved7=8,TSSA=9,Reserved8=10,
	TSSB=11,CallGate=12,Reserved9=13,InterruptGate=14,TrapGate=15

}SegmentType;
struct stSegmentDescriptor // from intel Manual - 3.4.5 Segment Decriptor
{

	unsigned __int32  SegLimit0_15 :16;
	unsigned __int32  Base0_15 : 16;
	unsigned __int32  Base23_16 : 8;
	
	unsigned __int32 Type : 4;
	unsigned __int32  S : 1; //Descriptor Type (0= system , 1= code or data)
	unsigned __int32  DPL : 2; //Descriptor Privileg level
	unsigned __int32  P : 1; //segment present
	unsigned __int32  SegLimit19_16 : 4;
	unsigned __int32  AVL : 1; // Availible for use by system software
	unsigned __int32  L : 1;//64 bit code Segment
	unsigned __int32  D_B : 1;// defualt operation size;
	unsigned __int32  G : 1; //Granularity
	unsigned __int32  Base31_24 : 8;
};

struct stSystemDescriptor // from intel Manual - 3.4.5 Segment Decriptor
{

	unsigned __int32  SegLimit0_15 : 16;
	unsigned __int32  Base0_15 : 16;
	unsigned __int32  Base23_16 : 8;
	SegmentType Type : 4;
	unsigned __int32  S : 1; //Descriptor Type (0= system , 1= code or data)
	unsigned __int32  DPL : 2; //Descriptor Privileg level
	unsigned __int32  P : 1; //segment present
	unsigned __int32  SegLimit19_16 : 4;
	unsigned __int32  AVL : 1; // Availible for use by system software
	unsigned __int32  L : 1;//64 bit code Segment
	unsigned __int32  D_B : 1;// defualt operation size;
	unsigned __int32  G : 1; //Granularity
	unsigned __int32  Base31_24 : 8;
	unsigned __int32  Base32_63;
	unsigned __int32  IGN1 : 8;
	unsigned __int32  Zero : 5;
	unsigned __int32  IGN2 : 19;
};


union stCR3
{
	unsigned __int64 Cr3;
	struct
	{
		unsigned int Ignore1 :3;
		unsigned int PWT :1 ;//Page-Level  Write-Through
		unsigned int PCD :1 ;//Page Level cache disable
		unsigned int Ignore2:7;
		unsigned __int64 PhysicalAddress :40;
		unsigned int Ignore3:12;
	};
};

struct DiscriptorPointer
{
	unsigned __int16 limit;
	unsigned __int64 base;
};

union DR7
{
	unsigned __int64 VAl;
	struct
	{
		unsigned __int64 L0 : 1; //Enables (when set) the breakpoint condition for the associated breakpoint for the current task
		unsigned __int64 G0 : 1;
		unsigned __int64 L1 : 1;
		unsigned __int64 G1 : 1;
		unsigned __int64 L2 : 1;
		unsigned __int64 G2 : 1;
		unsigned __int64 L3 : 1;
		unsigned __int64 G3 : 1;
		unsigned __int64 LE : 1;
		unsigned __int64 GE : 1;
		unsigned __int64 REv1 : 3;
		unsigned __int64 GD : 1;
		unsigned __int64 REv2 : 2;
		unsigned __int64 RW0 : 2;
		unsigned __int64 LEN0 : 2;
		unsigned __int64 RW1 : 2;
		unsigned __int64 LEN1 : 2;
		unsigned __int64 RW2 : 2;
		unsigned __int64 LEN2 : 2;
		unsigned __int64 RW3 : 2;
		unsigned __int64 LEN3 : 2;
		unsigned __int64 REv3 : 32;
	};
};
union DR6
{
	unsigned __int64 VAl;
	struct
	{
		unsigned __int64 BreakPointIndicatot : 4;
		unsigned __int64 Rev1 : 9;
		unsigned __int64 BD : 1;
		unsigned __int64 BS : 1;
		unsigned __int64 BT : 1;
		unsigned __int64 Rev2 : 16;
		unsigned __int64 Rev3 : 32;
	};
};
union DebugTrap
{
	unsigned __int64 VAl;
	struct
	{
		unsigned __int64 BreakPointIndicator : 4;
		unsigned __int64 REv1 : 8;
		unsigned __int64 BreakPointEnable : 1;
		unsigned __int64 REv2 : 1;
		unsigned __int64 BS : 1;
		unsigned __int64 Rev3 : 49;
	};
};
#pragma pack (pop)


unsigned __int64 ReadMsr(ULONG i_ulMsr);
unsigned __int64 ReadCr0 ();
unsigned __int64 ReadCr4 ();
unsigned __int64 ReadCr3 ();
unsigned __int64 ReadFlag();
unsigned __int16 ReadCS();
unsigned __int16 ReadDS();
unsigned __int16 ReadSS();
unsigned __int16 ReadFS();
unsigned __int16 ReadGS();
unsigned __int16 ReadES();
void ReadGdt(void *);
void ReadIdt(void *);
void ReadTr(void *);
void ReadLdt(void *); 
void GetGuestExit(unsigned __int64 * o_u64Rsp ,unsigned __int64 * o_u64Rip );
void DisableInterupt();
void EnableInterupt();
void WriteCr0(unsigned __int64);
void WriteCr4(unsigned __int64);
unsigned __int64 VmxON(void *);
unsigned __int64 VmClear(void *);
unsigned __int64 VmPtrLd(void *);
void EM_rdmsr();
void EM_wrmsr(void);
void EM_cpuid(void);
void EM_invd(void);
void EM_dummy(void);
void EM_vmcall(void);
void EM_rdtsc(void);
void EntryPoint(void);
void xchgds(unsigned __int16 *);
void writeds(unsigned __int16);
void WriteDR7(unsigned __int64);
void WriteDR3(unsigned __int64);
void WriteDR6(unsigned __int64);
unsigned __int64 ReadDR6();
unsigned __int64 ReadDR3();
unsigned __int64 ReadDR7();