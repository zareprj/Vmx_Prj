#pragma once
#include <wdm.h>
#define	MAX_CALLBACK			EXIT_REASON_XSETBV+2
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                  \
	((ULONG)(char)(ch0) | ((ULONG)(char)(ch1) << 8) |   \
	((ULONG)(char)(ch2) << 16) | ((ULONG)(char)(ch3) << 24 ))

#define kStackMark	MAKEFOURCC('E', 'B', 'I', 'Z')

unsigned __int64 VMEntryPoint(ULONG_PTR *MEM);
void VmInit(ULONG_PTR *MEM);
#pragma pack (push,1)

enum control_reg {
	CONTROL_REG_CR0 = 0,
	CONTROL_REG_CR2 = 2,
	CONTROL_REG_CR3 = 3,
	CONTROL_REG_CR4 = 4,
	CONTROL_REG_CR8 = 8,
};

enum general_reg {
	GENERAL_REG_RAX = 0,
	GENERAL_REG_RCX = 1,
	GENERAL_REG_RDX = 2,
	GENERAL_REG_RBX = 3,
	GENERAL_REG_RSP = 4,
	GENERAL_REG_RBP = 5,
	GENERAL_REG_RSI = 6,
	GENERAL_REG_RDI = 7,
	GENERAL_REG_R8 = 8,
	GENERAL_REG_R9 = 9,
	GENERAL_REG_R10 = 10,
	GENERAL_REG_R11 = 11,
	GENERAL_REG_R12 = 12,
	GENERAL_REG_R13 = 13,
	GENERAL_REG_R14 = 14,
	GENERAL_REG_R15 = 15,
};

enum exit_qual_cr_lmsw {
	EXIT_QUAL_CR_LMSW_REGISTER = 0,
	EXIT_QUAL_CR_LMSW_MEMORY = 1,
};

enum exit_qual_cr_type {
	EXIT_QUAL_CR_TYPE_MOV_TO_CR = 0,
	EXIT_QUAL_CR_TYPE_MOV_FROM_CR = 1,
	EXIT_QUAL_CR_TYPE_CLTS = 2,
	EXIT_QUAL_CR_TYPE_LMSW = 3,
};



struct exit_qual_cr {
	enum control_reg num : 4;
	enum exit_qual_cr_type type : 2;
	enum exit_qual_cr_lmsw lmsw : 1;
	unsigned int reserved1 : 1;
	enum general_reg reg : 4;
	unsigned int reserved2 : 4;
	unsigned int lmsw_src : 16;
} ;

union ExitQuelDebugExeption
{
	unsigned __int64 VAL;
	struct
	{
		unsigned __int64 BreakPointIndicatot : 4;
		unsigned __int64 REV1 : 9;
		unsigned __int64 BD : 1;
		unsigned __int64 BS : 1;
		unsigned __int64 REV2 : 49;
	};
};

union ExitQuelDebugRegisterAccess
{
	unsigned __int64 VAL;
	struct
	{
		unsigned __int64 NumberOfDebugRegister : 3;
		unsigned __int64 REV1 : 1;
		unsigned __int64 Direction : 1;
		unsigned __int64 REV2 : 3;
		unsigned __int64 Register : 4;
		unsigned __int64 REV3 : 52;
	};
};


union vmExitIntruptInfo
{
	unsigned __int32 Val;
	struct
	{
		unsigned __int32 Vector : 8;
		unsigned __int32 Type : 3;
		unsigned __int32 Error_Code_Valid : 1;
		unsigned __int32 NMI : 1;
		unsigned __int32 Reservd : 18;
		unsigned __int32 Valid : 1;
	};
};

#pragma pack (pop)