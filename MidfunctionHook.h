#pragma once

#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <xmmintrin.h>


struct registers
{
	int64_t rax;
	int64_t rcx;
	int64_t rdx;
	int64_t rbx;
	int64_t rbp;
	int64_t rsi;
	int64_t rdi;
	int64_t r8;
	int64_t r9;
	int64_t r10;
	int64_t r11;
	int64_t r12;
	int64_t r13;
	int64_t r14;
	int64_t r15;

	__m128 xmm0;
	__m128 xmm1;
	__m128 xmm2;
	__m128 xmm3;
	__m128 xmm4;
	__m128 xmm5;
	__m128 xmm6;
	__m128 xmm7;
	__m128 xmm8;
	__m128 xmm9;
	__m128 xmm10;
	__m128 xmm11;
	__m128 xmm12;
	__m128 xmm13;
	__m128 xmm14;
	__m128 xmm15;

	void print()
	{
		printf("rax = %llx\nrcx = %llx\nrdx = %llx\nrbx = %llx\nrbp = %llx\nrsi = %llx\nrdi = %llx\nr8 = %llx\nr9 = %llx\nr10 = %llx\nr11 = %llx\nr12 = %llx\nr13 = %llx\nr14 = %llx\nr15 = %llx\n",
			rax,
			rcx,
			rdx,
			rbx,
			rbp,
			rsi,
			rdi,
			r8,
			r9,
			r10,
			r11,
			r12,
			r13,
			r14,
			r15);
	}
};


class MidfunctionHook
{
	//bytes overwritten by placing the detour
	BYTE* originalBytes;

	//location where hook is placed
	BYTE* sourceAddress;

	//runs overwritten instructions
	BYTE* trampoline;

	//number of bytes to overwrite (don't cut instructions in half)
	int hookLength;

public:
	void Hook(BYTE* sourceAddress, BYTE* targetAddress, const int length);
	void Unhook();
};

