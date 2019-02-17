/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM arm version 1.4.0
** DO NOT EDIT! The original file is in "call_arm.dasc".
*/

/* vim: ts=4 sw=4 sts=4 et tw=78
 * Portions copyright (c) 2015-present, Facebook, Inc. All rights reserved.
 * Portions copyright (c) 2011 James R. McKaskill.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
 
//The generate code is for arm not for thumb
#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif

static const unsigned int build_actionlist[716] = {
0xed0d0a00,
0x000f8100,
0x00000000,
0xed4d0a00,
0x000f8100,
0x00000000,
0xed0d1a00,
0x000f8100,
0x00000000,
0xed4d1a00,
0x000f8100,
0x00000000,
0xed0d2a00,
0x000f8100,
0x00000000,
0xed4d2a00,
0x000f8100,
0x00000000,
0xed0d3a00,
0x000f8100,
0x00000000,
0xed4d3a00,
0x000f8100,
0x00000000,
0xed0d4a00,
0x000f8100,
0x00000000,
0xed4d4a00,
0x000f8100,
0x00000000,
0xed0d5a00,
0x000f8100,
0x00000000,
0xed4d5a00,
0x000f8100,
0x00000000,
0xed0d6a00,
0x000f8100,
0x00000000,
0xed4d6a00,
0x000f8100,
0x00000000,
0xed0d7a00,
0x000f8100,
0x00000000,
0xed4d7a00,
0x000f8100,
0x00000000,
0xed0d0b00,
0x000f8100,
0x00000000,
0xed0d1b00,
0x000f8100,
0x00000000,
0xed0d2b00,
0x000f8100,
0x00000000,
0xed0d3b00,
0x000f8100,
0x00000000,
0xed0d4b00,
0x000f8100,
0x00000000,
0xed0d5b00,
0x000f8100,
0x00000000,
0xed0d6b00,
0x000f8100,
0x00000000,
0xed0d7b00,
0x000f8100,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c100e,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c0006,
0x00000000,
0xe51d1000,
0x000e8180,
0x00000000,
0xe2866000,
0x000b0000,
0x00000000,
0xe8b6100e,
0x00000000,
0xe8b60006,
0x00000000,
0xe4961004,
0x00000000,
0xed1d0a00,
0x000f8100,
0x00000000,
0xed1d0b00,
0x000f8100,
0x00000000,
0xed160a00,
0xe2866004,
0x00000000,
0xe2866000,
0x000b0000,
0x00000000,
0xed160b00,
0xe2866008,
0x00000000,
0xe1a0c00d,
0xe92d000f,
0x00000000,
0xe24dd040,
0x00000000,
0xe92d5050,
0xe1a0600c,
0xe3004000,
0x000c0200,
0xe3404000,
0x000c0200,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030000,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030000,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe5801000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe880100e,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe8800006,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c000c,
0x00000000,
0xe2866004,
0x00000000,
0xe8b6000c,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030003,
0x00000000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe8800006,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xe5801000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030002,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030004,
0x00000000,
0xe6ef1071,
0x00000000,
0xe6af1071,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030005,
0x00000000,
0xe6ff1071,
0x00000000,
0xe6bf1071,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030005,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030005,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030006,
0x00000000,
0xe28dc000,
0x000b0000,
0xe89c000c,
0x00000000,
0xe2866004,
0x00000000,
0xe8b6000c,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030007,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3a01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030008,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3003000,
0x000c0200,
0xe3403000,
0x000c0200,
0xe3e02000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030009,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x00030000,
0xe3003000,
0x000c0200,
0xe3403000,
0x000c0200,
0xe3e02000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x0003000a,
0x00000000,
0xe3e01001,
0xe1a00004,
0xeb000000,
0x0003000b,
0x00000000,
0xe3e01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000c,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000d,
0x00000000,
0xe3e01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000e,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000f,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030010,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030011,
0x00000000,
0xee106a10,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0xee006a10,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030012,
0x00000000,
0xec564b10,
0xe1a00004,
0xe3e01002,
0xeb000000,
0x0003000b,
0xec464b10,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030013,
0xed2d0b04,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0xecbd0b04,
0x00000000,
0xe3e01000,
0xe1a00004,
0xeb000000,
0x00030014,
0xed2d0a02,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0xecbd0a02,
0x00000000,
0xe1a06000,
0xe3e01002,
0xe1a00004,
0xeb000000,
0x0003000b,
0xe1a00006,
0x00000000,
0xe1a06000,
0xe1a00004,
0xe1a04001,
0xe3e01002,
0xeb000000,
0x0003000b,
0xe1a00006,
0xe1a01004,
0x00000000,
0xe89da050,
0x00000000,
0xe2866004,
0x00000000,
0xe8a60003,
0x00000000,
0xe28dc000,
0x000b0000,
0xe88c0003,
0x00000000,
0xe4860004,
0x00000000,
0xe50d0000,
0x000e8180,
0x00000000,
0xed0d1b00,
0x000f8100,
0x00000000,
0xed0d0b00,
0x000f8100,
0x00000000,
0xed0d0a00,
0x000f8100,
0x00000000,
0xe1a0c00d,
0xe92d5870,
0xe1a0b00c,
0xe1a04000,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030015,
0xe1a05000,
0xe3550000,
0x000b0000,
0xaa000000,
0x00050001,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030016,
0x0006000b,
0x00000000,
0xe04dd205,
0xe28d6010,
0xe24dd040,
0x00000000,
0xe24dd000,
0x000b0000,
0xe28d6000,
0x000b0000,
0x00000000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0xe24dd008,
0xe58d0000,
0x00000000,
0xe3003000,
0x000c0200,
0xe3403000,
0x000c0200,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0x000b0000,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030009,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030017,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000a,
0x00000000,
0xe3a01000,
0x000b0000,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000d,
0xe3500000,
0x13a00001,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000d,
0x00000000,
0xe6ef0070,
0x00000000,
0xe6af0070,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000d,
0x00000000,
0xe6ff0070,
0x00000000,
0xe6bf0070,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000c,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000d,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030010,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000e,
0x00000000,
0xe1a00004,
0xeb000000,
0x0003000f,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030012,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030011,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030013,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030014,
0x00000000,
0xe28d3000,
0x000b0000,
0xe1a02005,
0xe3a01000,
0x000b0000,
0xe1a00004,
0xeb000000,
0x00030018,
0x00000000,
0xec9d0b10,
0x00000000,
0xec9d0b0e,
0x00000000,
0xec9d0b0c,
0x00000000,
0xec9d0b0a,
0x00000000,
0xec9d0b08,
0x00000000,
0xec9d0b06,
0x00000000,
0xec9d0b04,
0x00000000,
0xed1d0b00,
0x00000000,
0xe28dd040,
0x00000000,
0xe8bd000f,
0x00000000,
0xe8bd0003,
0x00000000,
0xe300c000,
0x000c0200,
0xe340c000,
0x000c0200,
0xe12fff3c,
0x00000000,
0xe1a06000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030001,
0xe5806000,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe1a03001,
0xe1a02000,
0x00000000,
0xe1a02001,
0xe1a01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030003,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe1a06000,
0xe1a05001,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0xe5806000,
0xe5805004,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe1a06000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3a01000,
0xe1a00004,
0xeb000000,
0x00030001,
0xe5806000,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe3a00000,
0xe91ba870,
0x00000000,
0xe1a01000,
0xe1a00004,
0xeb000000,
0x00030004,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe1a01000,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030019,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030005,
0x00000000,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030006,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe1a00004,
0xeb000000,
0x00030007,
0xe3a00001,
0xe91ba870,
0x00000000,
0xe1a06000,
0xe3002000,
0x000c0200,
0xe3402000,
0x000c0200,
0xe3001000,
0x000c0200,
0xe3401000,
0x000c0200,
0xe1a00004,
0xeb000000,
0x00030001,
0x00000000,
0xed000b00,
0xed801b02,
0x00000000,
0xed000b00,
0x00000000,
0xe3a00001,
0xe91ba870,
0x00000000
};

static const char *const globnames[] = {
  (const char *)0
};
static const char *const extnames[] = {
  "rawgeti",
  "push_cdata",
  "lua_remove",
  "lua_pushinteger",
  "lua_pushboolean",
  "push_int",
  "push_float",
  "lua_pushnumber",
  "lua_call",
  "check_typed_pointer",
  "check_enum",
  "lua_settop",
  "check_uint32",
  "check_int32",
  "check_uint64",
  "check_int64",
  "check_uintptr",
  "check_float",
  "check_double",
  "check_complex_double",
  "check_complex_float",
  "lua_gettop",
  "luaL_error",
  "check_typed_cfunction",
  "unpack_varargs_stack",
  "push_uint",
  (const char *)0
};

#define JUMP_SIZE 8

#define MIN_BRANCH ((INT32_MIN) >> 8)
#define MAX_BRANCH ((INT32_MAX) >> 8)
//arm pc offset 8 so comparing with next instruction is 4,
//unlike x86 which pass in the current instruction address+1 rather than the next instruction 
#define BRANCH_OFF 4	


#define ROUND_UP(x, align) (((int) (x) + (align - 1)) & ~(align - 1))
#ifdef TARGET_OS_IPHONE
#define  CK_ALGIN  0
#else
#define  CK_ALGIN  1
#endif
#define ALIGNED(x, align) (!CK_ALGIN||((int)(x) & (align - 1)) == 0)
#if defined(__ARM_PCS_VFP) || (GCC_VERSION==40500||defined(__clang__))&&!defined(__ARM_PCS) && !defined(__SOFTFP__) && !defined(__SOFTFP) && \
    defined(__VFP_FP__)
#define ARM_HF 1
#else
#define ARM_HF 0
#endif	
static void compile_extern_jump(struct jit* jit, lua_State* L, cfunction func, uint8_t* code)
{
    /* The jump code is the function pointer followed by a stub to call the
     * function pointer. The stub exists so we can jump to functions with an
     * offset greater than 32MB.
     *
     * Note we have to manually set this up since there are commands buffered
     * in the jit state.
     */
	
    *(cfunction*) code = func;
     //ldr pc, [pc - 12]
    *(uint32_t*) &code[4] = 0xE51FF00CU; 
	
}




void compile_globals(struct jit* jit, lua_State* L)
{
    (void) jit;
}

typedef struct stack_info{
	int extra;
	int int_off;
	int stack_off;
	int float_size;
#if ARM_HF
	int float_off; 
#endif	
} stack_info;
//vfp use back-filling rule for registers until a float value on stack
typedef struct reg_info{
	uint16_t exs;	
	union{
		uint8_t ints;
		uint8_t regs;
	};
#if ARM_HF
	uint8_t float_sealed;
	short floats;//each bit is a float: s0-s15 or v0-v7 or q0-q3
	uint8_t left_single;
	uint8_t left_dual;
	uint8_t highest_bit;
#endif
} reg_info;

#define MAX_REGS 4
#define MAX_FLOAT_REGS 16
#ifndef bool
#define bool uint8_t
#endif

#define has_bit(x,b) (((x)&(1<<(b)))!=0)
#define set_bit(x,b) (x=((x)|(1<<(b)))) 
#define FIX_ALIGN(x,al) \
	if(!ALIGNED((x),al)){\
		x=ROUND_UP(x,al);\
	}
static ALWAYS_INLINE bool is_float_sealed(reg_info* regs){
#if ARM_HF
return regs->float_sealed;
#else
return regs->regs>=MAX_REGS;
#endif
}
static ALWAYS_INLINE void add_int_reg(reg_info* regs){
	if(regs->regs<MAX_REGS)
		regs->regs++;
}
static ALWAYS_INLINE void add_int64_reg(reg_info* regs){
	if(regs->regs<MAX_REGS-1)
		regs->regs=ROUND_UP(regs->regs,2)+2;
	else if(regs->regs==MAX_REGS-1)
		regs->regs=MAX_REGS;
}
#if ARM_HF
static void save_float_reg(struct jit* Dst,int reg,int size,stack_info* st){
	if(reg==-1) return;
	switch(size){
	case 1:
		switch(reg){
			case 0:
				dasm_put(Dst, 0, st->float_size);
				break;
			case 1:
				dasm_put(Dst, 3, st->float_size);
				break;
			case 2:
				dasm_put(Dst, 6, st->float_size);
				break;
			case 3:
				dasm_put(Dst, 9, st->float_size);
				break;
			case 4:
				dasm_put(Dst, 12, st->float_size);
				break;
			case 5:
				dasm_put(Dst, 15, st->float_size);
				break;
			case 6:
				dasm_put(Dst, 18, st->float_size);
				break;
			case 7:
				dasm_put(Dst, 21, st->float_size);
				break;
			case 8:
				dasm_put(Dst, 24, st->float_size);
				break;
			case 9:
				dasm_put(Dst, 27, st->float_size);
				break;
			case 10:
				dasm_put(Dst, 30, st->float_size);
				break;
			case 11:
				dasm_put(Dst, 33, st->float_size);
				break;
			case 12:
				dasm_put(Dst, 36, st->float_size);
				break;
			case 13:
				dasm_put(Dst, 39, st->float_size);
				break;
			case 14:
				dasm_put(Dst, 42, st->float_size);
				break;
			case 15:
				dasm_put(Dst, 45, st->float_size);
				break;
		}
		break;
	case 2:
		switch(reg>>1){
			case 0:
				dasm_put(Dst, 48, st->float_size);
				break;
			case 1:
				dasm_put(Dst, 51, st->float_size);
				break;
			case 2:
				dasm_put(Dst, 54, st->float_size);
				break;
			case 3:
				dasm_put(Dst, 57, st->float_size);
				break;
			case 4:
				dasm_put(Dst, 60, st->float_size);
				break;
			case 5:
				dasm_put(Dst, 63, st->float_size);
				break;
			case 6:
				dasm_put(Dst, 66, st->float_size);
				break;
			case 7:
				dasm_put(Dst, 69, st->float_size);
				break;
		}
		break;
	}
    st->float_size+=size;
}

static int add_float_reg(reg_info* regs,int sz){
	if(is_float_sealed(regs)) return 0;
	int ret=-1;
	if(sz==1){
		if(regs->left_single||regs->left_dual){
			int i=0,n=regs->highest_bit;
			for(;i<n;++i){
				if(!has_bit(regs->floats,i)){
					if(i+1==n||!has_bit(regs->floats,i+1)){
						regs->left_single--;
					}else{
						regs->left_dual--;
						regs->left_single++;
					}
					set_bit(regs->floats,i);
					ret=i;
				}
			}
		}else{
			ret=regs->highest_bit;
			set_bit(regs->floats,regs->highest_bit);
			++regs->highest_bit;
		}
	}else if(sz==2){
		if(regs->left_dual){
			int i=regs->highest_bit;
			for(;i--!=1;){
				if(!has_bit(regs->floats,i)&&!has_bit(regs->floats,i-1)){
					regs->left_dual--;
					ret=i-1;
					set_bit(regs->floats,i);
					set_bit(regs->floats,i-1);
				}
			}
		}else if(regs->highest_bit==MAX_FLOAT_REGS-1){
			regs->highest_bit=MAX_FLOAT_REGS;
		}else{
			assert(regs->highest_bit<MAX_FLOAT_REGS-1);
			if(!ALIGNED(regs->highest_bit, 2)){
				regs->highest_bit++;
				regs->left_single++;
			}
			set_bit(regs->floats,regs->highest_bit);
			set_bit(regs->floats,regs->highest_bit+1);
			ret=regs->highest_bit;
			regs->highest_bit+=2;
		}
	}else if(sz==4){
		if(regs->highest_bit>MAX_FLOAT_REGS-4){
			regs->highest_bit=MAX_FLOAT_REGS;
		}else{
			if(!ALIGNED(regs->highest_bit,4)){
				int origin=regs->highest_bit;
				regs->highest_bit=ROUND_UP(regs->highest_bit,4);
				switch(regs->highest_bit-origin){
					case 3:
						regs->left_single++;
					case 2:
						regs->left_dual++;
						break;
					case 1:
						regs->left_single++;
						break;
				}
			}
			ret=regs->highest_bit;
			set_bit(regs->floats,regs->highest_bit);
			set_bit(regs->floats,regs->highest_bit+1);
			set_bit(regs->floats,regs->highest_bit+2);
			set_bit(regs->floats,regs->highest_bit+3);
			regs->highest_bit+=4;
		}
	}
	if(regs->highest_bit==MAX_FLOAT_REGS){
		regs->float_sealed=true;		
	}
	return ret;

	
}
#endif
static void load_reg(struct jit* Dst,int off,int size){
	if(size==16){
		dasm_put(Dst, 72, off);
	}else if(size==8){
		dasm_put(Dst, 76, off);
	}else{
		dasm_put(Dst, 80, off);
	}
}
// arm store/load range for immediate value is only -256-255
static void load_stack(struct jit* Dst,stack_info* st,int size,int align){
	int off=st->stack_off;
	FIX_ALIGN(st->stack_off,align);
	if((off=st->stack_off-off)){
		dasm_put(Dst, 83, off);
	}
	if(size==16){
		dasm_put(Dst, 86);
	}else if(size==8){
		dasm_put(Dst, 88);
	}else{
		dasm_put(Dst, 90);
	}
	st->stack_off+=size;
}

static void load_int(struct jit* Dst,stack_info* st,int size,int align){
	FIX_ALIGN(st->int_off,align);
	if(st->int_off<MAX_REGS*4){
		load_reg(Dst,st->int_off+st->extra,size);
		st->int_off+=size;
	}else{
		load_stack(Dst,st,size,align);
	}
	
}

static void load_float(struct jit* Dst,stack_info* st,int size,int vfp,int align){
	#if ARM_HF
	if(st->float_off<st->float_size){
		if(vfp){
			if(size==4){//float
				dasm_put(Dst, 92, st->float_off+st->extra);
			}else if(size==8){//double
				dasm_put(Dst, 95, st->float_off+st->extra);
			}
		}else load_reg(Dst,st->float_off+st->extra,size);
		st->float_off+=size;
	}else if(vfp){
		if(size==4){//float
			dasm_put(Dst, 98);
		}else if(size==8){//double
			int off=st->stack_off;
			FIX_ALIGN(st->stack_off,align);
			if((off=st->stack_off-off)){
				dasm_put(Dst, 101, off);
			}
			dasm_put(Dst, 104);
		}
	}else{
		load_stack(Dst,st,size,align);
	}
	#else	
		load_int(Dst,st,size,align);
	#endif
}
#if ARM_HF
static void push_regs(lua_State* L,struct jit* Dst,int ct_usr,int nargs,stack_info* st){
	const struct ctype* mt;
	reg_info regs;int i;
	memset(&regs,0,sizeof(reg_info));
    for (i = 1; i <= nargs&&!is_float_sealed(&regs); ++i){
		lua_rawgeti(L, ct_usr, i);
		mt = (const struct ctype*) lua_touserdata(L, -1);
		if (mt->pointers || mt->is_reference) {
			add_int_reg(&regs);
		}else{
			switch(mt->type){
				case COMPLEX_DOUBLE_TYPE:
					save_float_reg(Dst,add_float_reg(&regs,2),2,st);
				case DOUBLE_TYPE:
				case COMPLEX_FLOAT_TYPE:// Though complex alignment is 4, but vfp requires a sequence of regsiters
					save_float_reg(Dst,add_float_reg(&regs,2),2,st);
					break;
				case FLOAT_TYPE:
					save_float_reg(Dst,add_float_reg(&regs,1),1,st);
					break;
				case INT64_TYPE:
					add_int64_reg(&regs);
					break;
				default:
					add_int_reg(&regs);//no need to check type support here
			}
		}
	}
	st->float_off+=st->int_off;
	st->int_off+=0x40;
}
#endif
cfunction compile_callback(lua_State* L, int fidx, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals, ref;
    const struct ctype* mt;
	stack_info st;

    int top = lua_gettop(L);

    ct_usr = lua_absindex(L, ct_usr);
    fidx = lua_absindex(L, fidx);
    nargs = (int) lua_rawlen(L, ct_usr);

    dasm_setup(Dst, build_actionlist);

    lua_newtable(L);
    lua_pushvalue(L, -1);
    ref = luaL_ref(L, LUA_REGISTRYINDEX);
    num_upvals = 0;

    if (ct->has_var_arg) {
        luaL_error(L, "can't create callbacks with varargs");
    }
	memset(&st,0,sizeof(stack_info));
	st.extra=0x10;
    /* prolog and get the upval table */
	dasm_put(Dst, 107);
#if ARM_HF
	dasm_put(Dst, 110);
	push_regs(L,Dst,ct_usr,nargs,&st);
#endif	
    
    dasm_put(Dst, 112, (unsigned short)(L), (((unsigned int)(L))>>16), (unsigned short)(ref), (((unsigned int)(ref))>>16), (unsigned short)(LUA_REGISTRYINDEX), (((unsigned int)(LUA_REGISTRYINDEX))>>16));
	
    /* get the lua function */
    lua_pushvalue(L, fidx);
    lua_rawseti(L, -2, ++num_upvals);
	
	
    dasm_put(Dst, 130, num_upvals);
#if !ARM_HF 
	// Complex type is return in the address stored in r0 for softfp
	lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);
	if(!mt->pointers && !mt->is_reference&&(mt->type==COMPLEX_DOUBLE_TYPE||mt->type==COMPLEX_FLOAT_TYPE)){
		st.int_off+=4;
	} 
	lua_pop(L,1);
#endif
	//whether 64 bit type requires 8 bytes alignment in stack is defined by compiler.android compiler reqiures only 4 byte alignment;
	//actually the stack it self may reqiures 8 bytes alignment
    for (i = 1; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference) {
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */
			
            dasm_put(Dst, 137, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16));
            load_int(Dst,&st,4,4);
            dasm_put(Dst, 153);
        } else {
            switch (mt->type) {
			case COMPLEX_DOUBLE_TYPE:
				
				lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
                dasm_put(Dst, 159, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16));
                load_float(Dst,&st,16,0,8);
                dasm_put(Dst, 175);
				break;
			case COMPLEX_FLOAT_TYPE:
                lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
                dasm_put(Dst, 181, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16));
				load_float(Dst,&st,8,0,4);
                dasm_put(Dst, 197);
				break;
            case INT64_TYPE:
				
			#if LUA_VERSION_NUM>=503
                lua_pop(L, 1);
				
            #if CK_ALGIN
				FIX_ALIGN(st.int_off,8);
				if(st.int_off<16){
					dasm_put(Dst, 203, st.int_off+st.extra);
					st.int_off+=8;
				}else{
					if(!ALIGNED(st.stack_off,8)){
						st.stack_off+=4;
						dasm_put(Dst, 207);
					}
					dasm_put(Dst, 209);
					st.stack_off+=8;
				}
			#else
				load_int(Dst,st,8,8);
			#endif
                dasm_put(Dst, 211);
			#else
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 215, (unsigned short)(mt), (((unsigned int)(mt))>>16));
                load_int(Dst,&st,8,8);
                dasm_put(Dst, 224);
			#endif
                break;

            case INTPTR_TYPE:
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 230, (unsigned short)(mt), (((unsigned int)(mt))>>16));
                load_int(Dst,&st,4,4);
                dasm_put(Dst, 239);
				
                break;

            case BOOL_TYPE:
                lua_pop(L, 1);
				
                load_int(Dst,&st,4,4);
                dasm_put(Dst, 245);
                break;

            case INT8_TYPE:
                lua_pop(L, 1);
				load_int(Dst,&st,4,4);
                if (mt->is_unsigned) {
					dasm_put(Dst, 249);
                }else{
					dasm_put(Dst, 251);
				}
                dasm_put(Dst, 253);
                break;

            case INT16_TYPE:
                lua_pop(L, 1);
				load_int(Dst,&st,4,4);
                if (mt->is_unsigned) {
					dasm_put(Dst, 257);
                }else{
					dasm_put(Dst, 259);
				}
                dasm_put(Dst, 261);
                break;

            case ENUM_TYPE:
            case INT32_TYPE:
                lua_pop(L, 1);
				
                load_int(Dst,&st,4,4);
                dasm_put(Dst, 265);
                break;

            case FLOAT_TYPE:
                lua_pop(L, 1);
                
                load_float(Dst,&st,4,ARM_HF,4);
                dasm_put(Dst, 269);
                break;

            case DOUBLE_TYPE:
                lua_pop(L, 1);
				
				#if ARM_HF
				load_float(Dst,&st,8,ARM_HF,8);
				#elif CK_ALGIN
				FIX_ALIGN(st.int_off,8);
				if(st.int_off<16){
					dasm_put(Dst, 273, st.int_off+st.extra);
					st.int_off+=8;
				}else{
					if(!ALIGNED(st.stack_off,8)){
						st.stack_off+=4;
						dasm_put(Dst, 277);
					}
					dasm_put(Dst, 279);
					st.stack_off+=8;
				}
				#else
				load_float(Dst,&st,8,ARM_HF,8);
				#endif	
                dasm_put(Dst, 281);
                break;
				
            default:
                luaL_error(L, "NYI: callback arg type");
            }
        }
    }

    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    dasm_put(Dst, 285, ((mt->pointers || mt->is_reference || mt->type != VOID_TYPE) ? 1 : 0), nargs);
    

    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        lua_rawseti(L, -3, ++num_upvals); /* usr value */
        lua_rawseti(L, -2, ++num_upvals); /* mt */

        dasm_put(Dst, 293, num_upvals-1, (unsigned short)(mt), (((unsigned int)(mt))>>16));
        goto single_no_pop;
    } else {
        switch (mt->type) {
        case ENUM_TYPE:
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */

            dasm_put(Dst, 309, num_upvals-1, (unsigned short)(mt), (((unsigned int)(mt))>>16));

            goto single_no_pop;

        case VOID_TYPE:
            dasm_put(Dst, 325);
            lua_pop(L, 1);
            break;

        case BOOL_TYPE:
        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
		    dasm_put(Dst, 330);
            if (mt->is_unsigned) {
                dasm_put(Dst, 332);
            } else {
                dasm_put(Dst, 336);
            }
            goto single;

        case INT64_TYPE:
            dasm_put(Dst, 340);
            if (mt->is_unsigned) {
                dasm_put(Dst, 342);
            } else {
                dasm_put(Dst, 346);
            }
            goto dual;

        case INTPTR_TYPE:
            dasm_put(Dst, 350);
            goto single;

        case FLOAT_TYPE:
            dasm_put(Dst, 355);
			#if ARM_HF
			dasm_put(Dst, 360);
			#else
            goto single;
			#endif

        case DOUBLE_TYPE:
            dasm_put(Dst, 367);
			#if ARM_HF
			dasm_put(Dst, 372);
            #else
			goto dual;
			#endif
			break;
		case COMPLEX_DOUBLE_TYPE:
            lua_pop(L, 1);
			dasm_put(Dst, 379);
			break;
		case COMPLEX_FLOAT_TYPE:
            lua_pop(L, 1);
			dasm_put(Dst, 390);
			break;
			
        single:
            lua_pop(L, 1);
		single_no_pop:	
            dasm_put(Dst, 401);
			
            break;
		dual:
			dasm_put(Dst, 408);
			
            lua_pop(L, 1);
			break;
       

        
        default:
            luaL_error(L, "NYI: callback return type");
        }
    }
	
    dasm_put(Dst, 417);
	
    lua_pop(L, 1); /* upval table - already in registry */
    assert(lua_gettop(L) == top);

    {
        void* p;
        struct ctype ft;
        cfunction func;

        func = compile(Dst, L, NULL, ref);

        ft = *ct;
        ft.is_jitted = 1;
        p = push_cdata(L, ct_usr, &ft);
        *(cfunction*) p = func;

        assert(lua_gettop(L) == top + 1);

        return func;
    }
}

static ALWAYS_INLINE void save_int64_stack_align(struct jit* Dst,reg_info* regs,int align){
	if(align&&!ALIGNED(regs->exs,2)){
		regs->exs++;
		dasm_put(Dst, 419);
	}
	dasm_put(Dst, 421);
	regs->exs+=2;
}
static ALWAYS_INLINE void save_int64_align(struct jit* Dst,reg_info* regs,int align){
	if(align&&!ALIGNED(regs->ints,2)){
		regs->ints++;
	}
	if(regs->ints<MAX_REGS){
		dasm_put(Dst, 423, ((regs->ints<<2)+0x40*1));
		regs->ints+=2;
	}else{
		save_int64_stack_align(Dst,regs,align);
	}
	
}
static ALWAYS_INLINE  void save_int64(struct jit* Dst,reg_info* regs){
	save_int64_align(Dst,regs,1);
}
static ALWAYS_INLINE void save_int_stack_align(struct jit* Dst,reg_info* regs){
	dasm_put(Dst, 427);
	regs->exs++;
}
static ALWAYS_INLINE void save_int(struct jit* Dst,reg_info* regs){
	if(regs->ints<MAX_REGS){
		dasm_put(Dst, 429, ((regs->ints++<<2)+0x40*1));
	}else{
		save_int_stack_align(Dst,regs);
	}
}
static void save_float(struct jit* Dst,reg_info* regs,int size,int align){
#if ARM_HF
	if(regs->float_sealed){
		int reg=add_float_reg(regs,size);
		if(reg<0) goto SAVE_STACK;
		switch(size){
		case 4:
			dasm_put(Dst, 432, (regs->regs<<2)+8);
		case 2:
			dasm_put(Dst, 435, (regs->regs<<2));
			break;
		case 1:
			dasm_put(Dst, 438, (regs->regs<<2));
			break;
		}
	}
	SAVE_STACK:
	if(size==1){
		save_int_stack_align(Dst,regs);
	}else if(size==2){
		save_int64_stack_align(Dst,regs,align);
	}else if(size==4){
		save_int64_stack_align(Dst,regs,1);
		save_int64_stack_align(Dst,regs,1);
	}
#else
	if(size==1){
		save_int(Dst,regs);
	}else if(size==2){
		save_int64_align(Dst,regs,align);
	}
#endif
}
void compile_function(lua_State* L, cfunction func, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals,retByAddr;
    const struct ctype* mt;
    void* p; reg_info regs;

    int top = lua_gettop(L);

    ct_usr = lua_absindex(L, ct_usr);
    nargs = (int) lua_rawlen(L, ct_usr);

    p = push_cdata(L, ct_usr, ct);
    *(cfunction*) p = func;
    num_upvals = 1;

    dasm_setup(Dst, build_actionlist);

    dasm_put(Dst, 441);
	
    if (ct->has_var_arg) {
        dasm_put(Dst, 446, nargs, (unsigned short)("too few arguments"), (((unsigned int)("too few arguments"))>>16));
    } 

    /* reserve enough stack space for all of the arguments (16 bytes per
     * argument for complex double and maintains alignment). For Hard floating point,
	 * leave extra 64 bytes
	 */
	if (ct->has_var_arg){
		dasm_put(Dst, 463);
	}else if(nargs>0){
		dasm_put(Dst, 467, (0x40*1+(nargs<<4)), 0x40*1+0x10);
		
	} 
	memset(&regs,0,sizeof(reg_info));// Complex type is return in the address stored in r0 for softfp 
	#if !ARM_HF
	lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);
	retByAddr=!mt->pointers && !mt->is_reference && (mt->type==COMPLEX_DOUBLE_TYPE||mt->type==COMPLEX_FLOAT_TYPE);
    if (retByAddr) {	
		regs.ints++;
		dasm_put(Dst, 472, (unsigned short)(mt), (((unsigned int)(mt))>>16));
	}
	lua_pop(L,1);
	#endif
    for (i = 1; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);
        if (mt->pointers || mt->is_reference || mt->type == FUNCTION_PTR_TYPE || mt->type == ENUM_TYPE) {
            lua_getuservalue(L, -1);
            num_upvals += 2;

            dasm_put(Dst, 483, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16), i);
			
            if (mt->pointers || mt->is_reference) {
                dasm_put(Dst, 494);
            } else if (mt->type == FUNCTION_PTR_TYPE) {
                dasm_put(Dst, 498);
            } else if (mt->type == ENUM_TYPE) {
                dasm_put(Dst, 502);
            }

            save_int(Dst,&regs);
        } else {
            lua_pop(L, 1);
            dasm_put(Dst, 506, i);

            switch (mt->type) {
			case BOOL_TYPE:
				dasm_put(Dst, 509);
                save_int(Dst,&regs);
                break;
            case INT8_TYPE:
                dasm_put(Dst, 515);
                if (mt->is_unsigned) {
                     dasm_put(Dst, 519);
                } else {
                     dasm_put(Dst, 521);
                }
                save_int(Dst,&regs);
                break;

            case INT16_TYPE:
                dasm_put(Dst, 523);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 527);
                } else {
                    dasm_put(Dst, 529);
                }
                save_int(Dst,&regs);
                break;

            case INT32_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 531);
                } else {
                    dasm_put(Dst, 535);
                }
                save_int(Dst,&regs);
                break;
            case INTPTR_TYPE:
                dasm_put(Dst, 539);
                save_int(Dst,&regs);
				break;

            case INT64_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 543);
                } else {
                    dasm_put(Dst, 547);
                }
				save_int64(Dst,&regs);
                break;

            case DOUBLE_TYPE:
				dasm_put(Dst, 551);
				save_float(Dst,&regs,2,1);
                break;

            case FLOAT_TYPE:
				dasm_put(Dst, 555);
                save_float(Dst,&regs,1,1);
                break;
			case COMPLEX_DOUBLE_TYPE:
				#if ARM_HF
				dasm_put(Dst, 559);
				save_float(Dst,&regs,4,1);
				#else
				FIX_ALIGN(regs.ints,2);
				if(regs.ints<4){
				}else{
					if(!ALIGNED(regs.exs,2)){
						++regs.exs;
					}
				}
				regs.ints+=4;
				goto FIX_REG;
				#endif
				break;
			case COMPLEX_FLOAT_TYPE:
				#if ARM_HF
				dasm_put(Dst, 563);
				save_float(Dst,&regs,2,0);
				#else
				if(regs.ints<4){
				}else{
				}
				regs.ints+=2;
				FIX_REG:
				if(regs.ints>4){
					regs.exs+=regs.ints-4;
					regs.ints=4;
				}
				#endif
                break;
            default:
                luaL_error(L, "NYI: call arg type");
            }
        }
    }

    if (ct->has_var_arg) {
        dasm_put(Dst, 567, (regs.ints<<2), nargs+1);
		regs.ints=4;
    } 
	
	#if ARM_HF
	switch(ROUND_UP(regs.highest_bit,4)>>1){
		case 8 :
			dasm_put(Dst, 576);
			break;
		case 7 :
			dasm_put(Dst, 578);
			break;
		case 6 :
			dasm_put(Dst, 580);
			break;
		case 5:
			dasm_put(Dst, 582);
			break;
		case 4 :
			dasm_put(Dst, 584);
			break;
		case 3 :
			dasm_put(Dst, 586);
			break;
		case 2 :
			dasm_put(Dst, 588);
			break;
		case 1 :
			dasm_put(Dst, 590);
			break;
	}
	dasm_put(Dst, 592);
	#endif
	
	//pop registers from stack,align 8 for some compiler
	
	switch(regs.ints){
	case 4:
	case 3:
		dasm_put(Dst, 594);
		break;
	case 2:
	case 1:
		dasm_put(Dst, 596);
		break;
	default:
		abort();
	}
	
	dasm_put(Dst, 598, (unsigned short)(func), (((unsigned int)(func))>>16));


    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    if (mt->pointers || mt->is_reference || mt->type==FUNCTION_PTR_TYPE) {
        lua_getuservalue(L, -1);
        num_upvals += 2;
        dasm_put(Dst, 604, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));

    } else {
        switch (mt->type) {
        case INT64_TYPE:
        #if LUA_VERSION_NUM>=503
             lua_pop(L, 1);
	    #if CK_ALGIN
            dasm_put(Dst, 620);
		#else
            dasm_put(Dst, 623);
		#endif		
            dasm_put(Dst, 626);
            break;
		#else
			num_upvals++;
            dasm_put(Dst, 632, (unsigned short)(mt), (((unsigned int)(mt))>>16));
            break;
        #endif
		
        case INTPTR_TYPE:
            num_upvals++;
            dasm_put(Dst, 647, (unsigned short)(mt), (((unsigned int)(mt))>>16));
            break;

        case VOID_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 660);
            break;

        case BOOL_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 663);
            break;

        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
        case ENUM_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 670);
            if (mt->is_unsigned) {
                dasm_put(Dst, 672);
            } else {
                dasm_put(Dst, 676);
            }
            dasm_put(Dst, 680);
            break;

        case FLOAT_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 683);
            break;

        case DOUBLE_TYPE:
            lua_pop(L, 1);
        #if CK_ALGIN
		#else
		#endif	
            dasm_put(Dst, 689);
            break;
		case COMPLEX_DOUBLE_TYPE:
		case COMPLEX_FLOAT_TYPE:
			lua_getuservalue(L,-1);
            num_upvals += 2;
			#if ARM_HF
			dasm_put(Dst, 695, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));
			if(mt->type==COMPLEX_DOUBLE_TYPE){
				dasm_put(Dst, 708);
			}else{
				dasm_put(Dst, 711);
			}
			#else
            // value are stored in return storage in r0 for softfp, set usr value here
			if(!lua_isnil(L,-1)){
			}
			#endif
            dasm_put(Dst, 713);
			break;
        default:
            luaL_error(L, "NYI: call return type");
        }
    }

    assert(lua_gettop(L) == top + num_upvals);
    {
        cfunction f = compile(Dst, L, NULL, LUA_NOREF);
        /* add a callback as an upval so that the jitted code gets cleaned up when
         * the function gets gc'd */
        push_callback(L, f, func);
        lua_pushcclosure(L, (lua_CFunction) f, num_upvals+1);
    }
}

