#pragma once






namespace registers
{

	

	enum : int
	{
		rax = 0,
		rbx, rcx, rdx,
		rdi, rsi, rbp, rsp,
		r8, r9, r10, r11,
		r12, r13, r14, r15,
		rip, rflags, cs,
		orig_rax, fs_base,
		gs_base,
		fs, gs, ss, ds, es,

		xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7,
		xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15,

		REGISTER_COUNT

	};

	const static char *registerNames[REGISTER_COUNT]{
		"rax",
		"rbx", "rcx", "rdx",
		"rdi", "rsi", "rbp", "rsp",
		"r8", "r9", "r10", "r11",
		"r12", "r13", "r14", "r15",
		"rip", "rflags", "cs",
		"orig_rax", "fs_base",
		"gs_base",
		"fs", "gs", "ss", "ds", "es",

		"xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",
		"xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15",

	};


	//page 58
	//https://www.uclibc.org/docs/psABI-x86_64.pdf
	const static int registerCode[REGISTER_COUNT]
	{
		0, 3, 2, 1, 
		5, 4, 6, 7,

		//r8 - r15
		8, 9, 10, 11, 12, 13, 14, 15,

		-1, 49, 51, -1, 58, 59, 54, 55, 52, 53, 50,

		//xmm
		17, 18, 19, 20, 21, 22, 23, 24,
		25, 26, 27, 28, 29, 30, 31, 32,

		//todo(vlod) YMM registers
	};




}