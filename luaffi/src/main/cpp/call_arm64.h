/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM arm version 1.4.0
** DO NOT EDIT! The original file is in "call_arm64.dasc".
*/

/* vim: ts=4 sw=4 sts=4 et tw=78
 * Portions copyright (c) 2015-present, Facebook, Inc. All rights reserved.
 * Portions copyright (c) 2011 James R. McKaskill.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#if DASM_VERSION != 10400
#error "Version mismatch between DynASM and included encoding engine"
#endif

static const unsigned int build_actionlist[765] = {
0xf84003e1,
0x000f0000,
0x00000000,
0xf8408681,
0x00000000,
0xfc4003e0,
0x000f0000,
0x00000000,
0xbc408681,
0x00000000,
0xfc408681,
0x00000000,
0x6d4007e0,
0x000a8cef,
0x00000000,
0x2cc10680,
0x00000000,
0x6cc10680,
0x00000000,
0xbc408680,
0x00000000,
0xfc408680,
0x00000000,
0xd10283ff,
0x00000000,
0xd10083ff,
0x00000000,
0xa9091fe6,
0x00000000,
0xa90817e4,
0x00000000,
0xa9070fe2,
0x00000000,
0xa90607e0,
0x910003ea,
0x00000000,
0x6d051fe6,
0x00000000,
0x6d0417e4,
0x00000000,
0x6d030fe2,
0x00000000,
0x6d0207e0,
0x910003eb,
0x00000000,
0xa9017bfd,
0xa90057f4,
0x00000000,
0x910283f4,
0x00000000,
0x52800015,
0x000a0205,
0xf2a00015,
0x000a0205,
0xf2c00015,
0x000a0205,
0xf2e00015,
0x000a0205,
0x52800002,
0x000a0205,
0x72a00002,
0x000a0205,
0x52800001,
0x000a0205,
0x72a00001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030000,
0x00000000,
0x52800002,
0x000a0205,
0x92800001,
0xaa1503e0,
0x94000000,
0x00030000,
0x00000000,
0x52800002,
0x000a0205,
0x92800001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030000,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030001,
0x00000000,
0xf9000001,
0x12800021,
0xaa1503e0,
0x94000000,
0x00030002,
0x00000000,
0x52800002,
0x000a0205,
0x92800001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030000,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030001,
0x00000000,
0x6d000400,
0x12800021,
0xaa1503e0,
0x94000000,
0x00030002,
0x00000000,
0x52800002,
0x000a0205,
0x92800001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030000,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030001,
0x00000000,
0x2d000400,
0x12800021,
0xaa1503e0,
0x94000000,
0x00030002,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030003,
0x00000000,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x2a1f03e1,
0xaa1503e0,
0x94000000,
0x00030001,
0x00000000,
0xf9000001,
0x12800021,
0xaa1503e0,
0x94000000,
0x00030002,
0x00000000,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x2a1f03e1,
0xaa1503e0,
0x94000000,
0x00030001,
0x00000000,
0xf9000001,
0x12800021,
0xaa1503e0,
0x94000000,
0x00030002,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030004,
0x00000000,
0x384003e1,
0x000f0000,
0x00000000,
0x38c003e1,
0x000f0000,
0x00000000,
0x38408681,
0x00000000,
0x38c08681,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030005,
0x00000000,
0x784003e1,
0x000f0000,
0x00000000,
0x78c003e1,
0x000f0000,
0x00000000,
0x78408681,
0x00000000,
0x78c08681,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030005,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030005,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030006,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030007,
0x00000000,
0x52800002,
0x000a0205,
0x52800001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030008,
0x00000000,
0x52800002,
0x000a0205,
0x92800021,
0xaa1503e0,
0x94000000,
0x00030000,
0x52800003,
0x000a0205,
0xf2a00003,
0x000a0205,
0xf2c00003,
0x000a0205,
0xf2e00003,
0x000a0205,
0x92800002,
0x92800021,
0xaa1503e0,
0x94000000,
0x00030009,
0x00000000,
0x52800002,
0x000a0205,
0x92800021,
0xaa1503e0,
0x94000000,
0x00030000,
0x52800003,
0x000a0205,
0xf2a00003,
0x000a0205,
0xf2c00003,
0x000a0205,
0xf2e00003,
0x000a0205,
0x92800002,
0x92800021,
0xaa1503e0,
0x94000000,
0x0003000a,
0x00000000,
0x12800021,
0xaa1503e0,
0x94000000,
0x0003000b,
0x00000000,
0x12800001,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000c,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000d,
0x00000000,
0x12800001,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000e,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000f,
0x00000000,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030010,
0x00000000,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030011,
0x00000000,
0x9e660014,
0x12800041,
0xaa1503e0,
0x94000000,
0x0003000b,
0x9e670280,
0x00000000,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030012,
0x00000000,
0x9e660014,
0x12800041,
0xaa1503e0,
0x94000000,
0x0003000b,
0x9e670280,
0x00000000,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030013,
0x00000000,
0x12800001,
0xaa1503e0,
0x94000000,
0x00030014,
0x00000000,
0xaa1503e0,
0x12800041,
0x9e660014,
0x9e660035,
0x94000000,
0x0003000b,
0x9e670280,
0x9e6702a1,
0x00000000,
0xaa0003f4,
0x12800041,
0xaa1503e0,
0x94000000,
0x0003000b,
0xaa1403e0,
0x00000000,
0xa94057f4,
0xa9417bfd,
0x910003ff,
0x000c0000,
0xd65f03c0,
0x00000000,
0xfc0003e0,
0x000f0000,
0x00000000,
0xbc008681,
0x00000000,
0xfc008681,
0x00000000,
0x6d0007e0,
0x000a8cef,
0x00000000,
0x2c810680,
0x00000000,
0x6c810680,
0x00000000,
0xbc008680,
0x00000000,
0xfc008680,
0x00000000,
0xd100c3ff,
0xa9027bfd,
0xa90153f3,
0xf90003f5,
0x910003fd,
0xaa0003f5,
0x00000000,
0x94000000,
0x00030015,
0xaa0003f3,
0xf100027f,
0x000c0000,
0x5400000a,
0x00050801,
0x52800001,
0x000a0205,
0xf2a00001,
0x000a0205,
0xf2c00001,
0x000a0205,
0xf2e00001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030016,
0x0006000b,
0xcb3373ff,
0x910003f4,
0xd10203ff,
0x00000000,
0xd10003ff,
0x000c0000,
0x910203f4,
0x00000000,
0x52800003,
0x000a0205,
0xf2a00003,
0x000a0205,
0xf2c00003,
0x000a0205,
0xf2e00003,
0x000a0205,
0x52800002,
0x000a0205,
0x72a00002,
0x000a0205,
0x52800001,
0x000a0205,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030009,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030017,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000a,
0x00000000,
0x52800001,
0x000a0205,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000d,
0x6b1f001f,
0x1a9f07e0,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000d,
0x00000000,
0x53001c00,
0x00000000,
0x13001c00,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000d,
0x00000000,
0x53003c00,
0x00000000,
0x13003c00,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000c,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000d,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000e,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003000f,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030010,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030012,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030011,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030013,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030014,
0x00000000,
0xb80003e0,
0x000f0000,
0x00000000,
0xb8008680,
0x00000000,
0xf80003e0,
0x000f0000,
0x00000000,
0xf8008680,
0x00000000,
0x910003e4,
0x000c0000,
0x52800003,
0x000a0205,
0xaa1303e2,
0x52800001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030018,
0x00000000,
0x910003e4,
0x000c0000,
0x52800003,
0x000a0205,
0xaa1303e2,
0x52800001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030019,
0x00000000,
0xf100027f,
0x000c0000,
0x5400000d,
0x00050801,
0xaa1403e5,
0x52800003,
0x000a0205,
0x52800003,
0x000a0205,
0xaa1303e2,
0x52800001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x0003001a,
0x0006000b,
0x00000000,
0xa9471fe6,
0x00000000,
0xa94617e4,
0x00000000,
0xa9450fe2,
0x00000000,
0xa94407e0,
0x00000000,
0x6d431fe6,
0x00000000,
0x6d4217e4,
0x00000000,
0x6d410fe2,
0x00000000,
0x6d4007e0,
0x00000000,
0x910203ff,
0x00000000,
0x52800009,
0x000a0205,
0xf2a00009,
0x000a0205,
0xf2c00009,
0x000a0205,
0xf2e00009,
0x000a0205,
0xd63f0120,
0x00000000,
0xaa0003f4,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x52800001,
0x000a0205,
0x72a00001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030001,
0xf9000014,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0xaa0003e1,
0xaa1503e0,
0x94000000,
0x00030003,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0xaa0003f4,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x2a1f03e1,
0xaa1503e0,
0x94000000,
0x00030001,
0xf9000014,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0x2a1f03e0,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0xaa0003e1,
0xaa1503e0,
0x94000000,
0x00030004,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0xaa0003e1,
0x00000000,
0xaa1503e0,
0x94000000,
0x0003001b,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030005,
0x00000000,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030006,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0xaa1503e0,
0x94000000,
0x00030007,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0x1e260000,
0x1e260021,
0xaa018000,
0xaa0003f4,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x52800001,
0x000a0205,
0x72a00001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030001,
0xf9000014,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
0x00000000,
0x9e660014,
0x9e660033,
0x52800002,
0x000a0205,
0xf2a00002,
0x000a0205,
0xf2c00002,
0x000a0205,
0xf2e00002,
0x000a0205,
0x52800001,
0x000a0205,
0x72a00001,
0x000a0205,
0xaa1503e0,
0x94000000,
0x00030001,
0xa9005013,
0x52800020,
0x910003bf,
0xf94003f5,
0xa94153f3,
0xa9427bfd,
0x9100c3ff,
0xd65f03c0,
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
  "unpack_varargs_float",
  "unpack_varargs_int",
  "unpack_varargs_stack_skip",
  "push_uint",
  (const char *)0
};

#define JUMP_SIZE 16

//in aarch64 the pc is indicated the current 
#define MIN_BRANCH ((INT32_MIN) >> 6)
#define MAX_BRANCH ((INT32_MAX) >> 6)
//arm64 pc has no offset so comparing with next instruction is -4 
#define BRANCH_OFF -4

static void compile_extern_jump(struct jit* jit, lua_State* L, cfunction func, uint8_t* code)
{
    /* The jump code is the function pointer followed by a stub to call the
     * function pointer. The stub exists so we can jump to functions with an
     * offset greater than 128MB.
     *
     * Note we have to manually set this up since there are commands buffered
     * in the jit state.
     */
	 
	 //l: ptr
	 *(cfunction*) code = func;
	 // ldr x9,#-8
	  *(uint32_t*) &code[8] = 0x58FFFFC9;
	 //br x9
	 *(uint32_t*) &code[12] = 0xD61F0120;
	
}

//| ldr reg, >5
//| b >6
//|5:
//|.long64 val
//|6:



void compile_globals(struct jit* jit, lua_State* L)
{
    (void) jit;
}
typedef struct reg_info{
	uint8_t ints;
	uint8_t floats;
} reg_info;

static ALWAYS_INLINE reg_info caculateRegs(lua_State* L,int ct_usr,int nargs){
    int i;reg_info regs;
    const struct ctype* mt;
    for (i = 1,regs.ints=0,regs.floats=0; i <= nargs&&(regs.floats<8||regs.ints<8); ++i){
		lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);
		if (mt->pointers || mt->is_reference) {
			if(regs.ints<8)regs.ints++;
		}else{
			switch(mt->type){
				case COMPLEX_DOUBLE_TYPE:
				case COMPLEX_FLOAT_TYPE:
					if(regs.floats<7)
						regs.floats+=2;
					else if(regs.floats==7)
						regs.floats=8;
					break;
				case FLOAT_TYPE:
				case DOUBLE_TYPE:
					if(regs.floats<8) ++regs.floats;
					break;
				default:
					if(regs.ints<8)++regs.ints;//no need to check type support here
			}
		}
		lua_pop(L,1);
	}
	
	return regs;
}
// arm store/load range for immediate value is only -256-255
static ALWAYS_INLINE void load_int(struct jit* Dst,reg_info* regs){
	if(regs->ints<8)
		dasm_put(Dst, 0, 0x60+(regs->ints++<<3));
	else
		dasm_put(Dst, 3);
}
static ALWAYS_INLINE void load_float(struct jit* Dst,reg_info* regs,int isFloat,int isComplex){
	if(regs->floats<8){
		if(!isComplex||regs->floats==7){
			dasm_put(Dst, 5, 0x20+(regs->floats++<<3));
			if(isComplex){
				if(isFloat){
					dasm_put(Dst, 8);
				}else{
					dasm_put(Dst, 10);
				}
			}
		}else{
			dasm_put(Dst, 12, 0x20+(regs->floats<<3));
			regs->floats+=2;
		}
	}else if(isComplex){
		if(isFloat){
			//complex float is packed as one double on stack
			dasm_put(Dst, 15);
		}else{
			dasm_put(Dst, 17);
		}
	}else {
		if(isFloat){
			dasm_put(Dst, 19);
		}else {
			dasm_put(Dst, 21);
		}
	}
			
	
}

cfunction compile_callback(lua_State* L, int fidx, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals, ref;
    const struct ctype* mt;

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

	reg_info regs=caculateRegs(L,ct_usr,nargs);
	
	if(regs.ints||regs.floats){
		dasm_put(Dst, 23);
	}else{
		dasm_put(Dst, 25);
	}
	//8 integer reigsters and 8 floating registers
	switch(regs.ints){
		case 8:
		case 7:
			dasm_put(Dst, 27);
		case 6:
		case 5:
			dasm_put(Dst, 29);
		case 4:
		case 3:
			dasm_put(Dst, 31);
		case 2:
		case 1:
			dasm_put(Dst, 33);
	}	

	switch(regs.floats){
		case 8:
		case 7:
			dasm_put(Dst, 36);
		case 6:
		case 5:
			dasm_put(Dst, 38);
		case 4:
		case 3:
			dasm_put(Dst, 40);
		case 2:
		case 1:
			dasm_put(Dst, 42);
	} 
	dasm_put(Dst, 45);
	
	if(regs.ints==8||regs.floats==8){ // may be overflowed if it's full
		dasm_put(Dst, 48);
	}
	
    /* get the lua function */
    lua_pushvalue(L, fidx);
    lua_rawseti(L, -2, ++num_upvals);
	
	dasm_put(Dst, 50, (unsigned short)(L), (((unsigned int)(L))>>16), (unsigned short)((unsigned long)(L)>>32), (unsigned short)((unsigned long)(L)>>48), (unsigned short)(ref), (((unsigned int)(ref))>>16), (unsigned short)(LUA_REGISTRYINDEX), (((unsigned int)(LUA_REGISTRYINDEX))>>16));
	
    dasm_put(Dst, 70, num_upvals);
	

    for (i = 1,regs.ints=0,regs.floats=0; i <= nargs; ++i) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference) {
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */
			
            dasm_put(Dst, 77, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));
			load_int(Dst,&regs);
            dasm_put(Dst, 97);

        } else {
            switch (mt->type) {
			case COMPLEX_DOUBLE_TYPE:
				lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 103, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));
				load_float(Dst,&regs,0,1);
                dasm_put(Dst, 123);
				
				break;
			case COMPLEX_FLOAT_TYPE:
				lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 129, num_upvals-1, i, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));
				load_float(Dst,&regs,1,1);
                dasm_put(Dst, 149);
				
				break;
            case INT64_TYPE:
			    #if LUA_VERSION_NUM>=503
                lua_pop(L, 1);
				load_int(Dst,&regs);
                dasm_put(Dst, 155);
				
				#else
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 159, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));
                load_int(Dst,&regs);
                dasm_put(Dst, 172);
				
				#endif
                break;

            case INTPTR_TYPE:
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 178, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));
                load_int(Dst,&regs);
                dasm_put(Dst, 191);
				
                break;

            case BOOL_TYPE:
                lua_pop(L, 1);
				
				load_int(Dst,&regs);
				dasm_put(Dst, 197);
				
                break;

            case INT8_TYPE:
				lua_pop(L, 1);
				if(regs.ints<8){
					if (mt->is_unsigned) {
						dasm_put(Dst, 201, 0x60+(regs.ints++<<3));
					} else {
						dasm_put(Dst, 204, 0x60+(regs.ints++<<3));
					}
				}else {
					if (mt->is_unsigned) {
						dasm_put(Dst, 207);
					} else {
						dasm_put(Dst, 209);
					}
				}
				dasm_put(Dst, 211);
				break;
			
            case INT16_TYPE:
				lua_pop(L, 1);
				if(regs.ints<8){
					if (mt->is_unsigned) {
						dasm_put(Dst, 215, 0x60+(regs.ints++<<3));
					} else {
						dasm_put(Dst, 218, 0x60+(regs.ints++<<3));
					}
				}else {
					if (mt->is_unsigned) {
						dasm_put(Dst, 221);
					} else {
						dasm_put(Dst, 223);
					}
				}
				dasm_put(Dst, 225);
				break;
				
            case ENUM_TYPE:
            case INT32_TYPE:
                lua_pop(L, 1);
				load_int(Dst,&regs);
				
                dasm_put(Dst, 229);
                break;

            case FLOAT_TYPE:
                lua_pop(L, 1);
				load_float(Dst,&regs,1,0);
                dasm_put(Dst, 233);
                break;

            case DOUBLE_TYPE:
                lua_pop(L, 1);
				load_float(Dst,&regs,0,0);
                dasm_put(Dst, 237);
                break;
				
            default:
                luaL_error(L, "NYI: callback arg type");
            }
        }
    }

    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    dasm_put(Dst, 241, ((mt->pointers || mt->is_reference || mt->type != VOID_TYPE) ? 1 : 0), nargs);
    

	
    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        lua_rawseti(L, -3, ++num_upvals); /* usr value */
        lua_rawseti(L, -2, ++num_upvals); /* mt */

        dasm_put(Dst, 249, num_upvals-1, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));
        goto single_no_pop;
    } else {
        switch (mt->type) {
        case ENUM_TYPE:
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */

            dasm_put(Dst, 269, num_upvals-1, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));

            goto single_no_pop;

        case VOID_TYPE:
            dasm_put(Dst, 289);
            lua_pop(L, 1);
            break;

        case BOOL_TYPE:
        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
		    dasm_put(Dst, 294);
            if (mt->is_unsigned) {
                dasm_put(Dst, 296);
            } else {
                dasm_put(Dst, 300);
            }
            goto single;

        case INT64_TYPE:
            dasm_put(Dst, 304);
            if (mt->is_unsigned) {
                dasm_put(Dst, 306);
            } else {
                dasm_put(Dst, 310);
            }
			
			goto single;

        case INTPTR_TYPE:
            dasm_put(Dst, 314);
            goto single;

        case FLOAT_TYPE:
            dasm_put(Dst, 319);
			
            dasm_put(Dst, 324);
            lua_pop(L, 1);
			break;
        case DOUBLE_TYPE:
            dasm_put(Dst, 331);
			
            dasm_put(Dst, 336);
			
            lua_pop(L, 1);
			break;
			
		case COMPLEX_DOUBLE_TYPE:

			dasm_put(Dst, 343);
		    
			goto complex_ret;
		case COMPLEX_FLOAT_TYPE:
			dasm_put(Dst, 348);
			
		complex_ret:	
			dasm_put(Dst, 353);

            lua_pop(L, 1);			
			break;
        single:
            lua_pop(L, 1);
		single_no_pop:	
            dasm_put(Dst, 362);
            break;

        
        default:
            luaL_error(L, "NYI: callback return type");
        }
    }
	
	dasm_put(Dst, 369,  (0x20 + ((regs.floats!=0)||(regs.ints!=0)) * 0x80));
	
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
static ALWAYS_INLINE void storeFloat(struct jit* Dst,reg_info* regs,int isFloat,int isComplex){
	if(regs->floats<8){
		if(!isComplex||regs->floats==7){
			dasm_put(Dst, 375, (regs->floats++<<3));
			if(isComplex){
				if(isFloat){
					dasm_put(Dst, 378);
				}else{
					dasm_put(Dst, 380);
				}
			}
		}else{
			dasm_put(Dst, 382, (regs->floats<<3));
			regs->floats+=2;
		}
	}else if(isComplex){
		//complex float is packed as one double on stack
		if(isFloat){
			dasm_put(Dst, 385);
		}else{
			dasm_put(Dst, 387);
		}
	}else {
		if(isFloat){
			dasm_put(Dst, 389);
		}else {
			dasm_put(Dst, 391);
		}
	}
}

void compile_function(lua_State* L, cfunction func, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals;
    const struct ctype* mt;
    void* p;

    int top = lua_gettop(L);

    ct_usr = lua_absindex(L, ct_usr);
    nargs = (int) lua_rawlen(L, ct_usr);

    p = push_cdata(L, ct_usr, ct);
    *(cfunction*) p = func;
    num_upvals = 1;

    dasm_setup(Dst, build_actionlist);

    reg_info regs={0,0};
	
	dasm_put(Dst, 393);
	
    /* reserve enough stack space for all of the arguments (16 bytes per
     * argument for complex double and maintains alignment). */
	 
    if (ct->has_var_arg) {
        dasm_put(Dst, 400, nargs, (unsigned short)("too few arguments"), (((unsigned int)("too few arguments"))>>16), (unsigned short)((unsigned long)("too few arguments")>>32), (unsigned short)((unsigned long)("too few arguments")>>48));
    }else if(nargs>0){
		dasm_put(Dst, 423, (nargs<<4)+0x80);
    }

    for (i = 1,regs.ints=0,regs.floats=0; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference || mt->type == FUNCTION_PTR_TYPE || mt->type == ENUM_TYPE) {
            lua_getuservalue(L, -1);
            num_upvals += 2;

			
			dasm_put(Dst, 427, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16), i);
			
            if (mt->pointers || mt->is_reference) {
                dasm_put(Dst, 442);
            } else if (mt->type == FUNCTION_PTR_TYPE) {
                dasm_put(Dst, 446);
            } else if (mt->type == ENUM_TYPE) {
                dasm_put(Dst, 450);
            }
			goto longstore;

        } else {
            lua_pop(L, 1);
            dasm_put(Dst, 454, i);

            switch (mt->type) {
            case BOOL_TYPE:
                dasm_put(Dst, 457);
                goto intstore;
            case INT8_TYPE:
                dasm_put(Dst, 463);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 467);
                } else {
                    dasm_put(Dst, 469);
                }
				goto intstore;

            case INT16_TYPE:
                dasm_put(Dst, 471);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 475);
                } else {
                    dasm_put(Dst, 477);
                }
				goto intstore;

            case INT32_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 479);
                } else {
                    dasm_put(Dst, 483);
                }
 				goto intstore;

            case INT64_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 487);
                } else {
                    dasm_put(Dst, 491);
                }
              	goto longstore;
				
            case INTPTR_TYPE:
                dasm_put(Dst, 495);
                
                goto longstore;

            case DOUBLE_TYPE:
                dasm_put(Dst, 499);
                storeFloat(Dst,&regs,0,0);
                break;

            case FLOAT_TYPE:
                dasm_put(Dst, 503);
                storeFloat(Dst,&regs,1,0);
                break;
			case COMPLEX_DOUBLE_TYPE:
                dasm_put(Dst, 507);
				storeFloat(Dst,&regs,0,1);
                break;

            case COMPLEX_FLOAT_TYPE:
                dasm_put(Dst, 511);
				storeFloat(Dst,&regs,1,1);
                break;
				
			intstore:
				if(regs.ints<8)
					dasm_put(Dst, 515, 0x40+(regs.ints++<<3));
				else
					dasm_put(Dst, 518);
				
                break;
			longstore:
				if(regs.ints<8)
					dasm_put(Dst, 520, 0x40+(regs.ints++<<3));
				else
					dasm_put(Dst, 523);
                break;
				
            default:
                luaL_error(L, "NYI: call arg type");
            }
        }
    }

    if (ct->has_var_arg) {
		if(regs.floats<8){
			dasm_put(Dst, 525, regs.floats<<3, (8-regs.floats), nargs+1);
		}
		if(regs.ints<8){
			dasm_put(Dst, 536, 0x40+(regs.ints<<3), (8-regs.ints), nargs+1);
		}
		dasm_put(Dst, 547, (nargs>8?nargs:8), (8-regs.floats), (8-regs.ints), nargs+1);
		regs.floats=regs.ints=8;
    }

	//pop all args in registers
	switch(regs.ints){
		case 8:
		case 7:
            dasm_put(Dst, 564);
		case 6:
		case 5:
            dasm_put(Dst, 566);
		case 4:
		case 3:
            dasm_put(Dst, 568);
		case 2:
		case 1:
			dasm_put(Dst, 570);
    }
	
	switch(regs.floats){
		case 8:
		case 7:
            dasm_put(Dst, 572);
		case 6:
		case 5:
            dasm_put(Dst, 574);
		case 4:
		case 3:
            dasm_put(Dst, 576);
		case 2:
		case 1:
			dasm_put(Dst, 578);
    }
	if(regs.ints==8|| regs.floats==8){// fix stack case registers is full
		dasm_put(Dst, 580);
	}
    dasm_put(Dst, 582, (unsigned short)(func), (((unsigned int)(func))>>16), (unsigned short)((unsigned long)(func)>>32), (unsigned short)((unsigned long)(func)>>48));


    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    if (mt->pointers || mt->is_reference || mt->type==FUNCTION_PTR_TYPE) {
        lua_getuservalue(L, -1);
        num_upvals += 2;
        dasm_put(Dst, 592, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));

    } else {
        switch (mt->type) {
        case INT64_TYPE:
		#if LUA_VERSION_NUM>=503
			 lua_pop(L, 1);
            dasm_put(Dst, 617);
            break;
		#endif

        case INTPTR_TYPE:
            num_upvals++;
            dasm_put(Dst, 629, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48));
			break;
        case VOID_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 651);
            break;

        case BOOL_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 659);
            break;

        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
        case ENUM_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 671);
            if (mt->is_unsigned) {
                dasm_put(Dst, 673);
            } else {
                dasm_put(Dst, 677);
            }
            dasm_put(Dst, 681);
            break;

        case FLOAT_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 689);
            break;

        case DOUBLE_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 700);
            break;
		case COMPLEX_FLOAT_TYPE:
            lua_getuservalue(L, -1);
            num_upvals+=2;
            dasm_put(Dst, 711, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));
            break;

        case COMPLEX_DOUBLE_TYPE:
            lua_getuservalue(L, -1);
            num_upvals+=2;
            dasm_put(Dst, 739, (unsigned short)(mt), (((unsigned int)(mt))>>16), (unsigned short)((unsigned long)(mt)>>32), (unsigned short)((unsigned long)(mt)>>48), (unsigned short)(lua_upvalueindex(num_upvals)), (((unsigned int)(lua_upvalueindex(num_upvals)))>>16));
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

