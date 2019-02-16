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

static const unsigned int build_actionlist[783] = {
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
0x58000015,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x18000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00060010,
0x18000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00060010,
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
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x18000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000003,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000003,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000003,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x18000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00060010,
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
0x58000009,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xd63f0120,
0x00000000,
0xaa0003f4,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x18000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00060010,
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
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
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
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x18000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00060010,
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
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x18000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00060010,
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
	
	dasm_put(Dst, 50, (int)(L), (int)(((unsigned long)(L))>>32), (int)(ref), (int)(LUA_REGISTRYINDEX));
	
    dasm_put(Dst, 76, num_upvals);
	

    for (i = 1,regs.ints=0,regs.floats=0; i <= nargs; ++i) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference) {
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */
			
            dasm_put(Dst, 83, num_upvals-1, i, (int)(mt), (int)(((unsigned long)(mt))>>32));
			load_int(Dst,&regs);
            dasm_put(Dst, 103);

        } else {
            switch (mt->type) {
			case COMPLEX_DOUBLE_TYPE:
				lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 109, num_upvals-1, i, (int)(mt), (int)(((unsigned long)(mt))>>32));
				load_float(Dst,&regs,0,1);
                dasm_put(Dst, 129);
				
				break;
			case COMPLEX_FLOAT_TYPE:
				lua_getuservalue(L, -1);
				lua_rawseti(L, -3, ++num_upvals); /* usr value */
				lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 135, num_upvals-1, i, (int)(mt), (int)(((unsigned long)(mt))>>32));
				load_float(Dst,&regs,1,1);
                dasm_put(Dst, 155);
				
				break;
            case INT64_TYPE:
			    #if LUA_VERSION_NUM>=503
                lua_pop(L, 1);
				load_int(Dst,&regs);
                dasm_put(Dst, 161);
				
				#else
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 165, (int)(mt), (int)(((unsigned long)(mt))>>32));
                load_int(Dst,&regs);
                dasm_put(Dst, 178);
				
				#endif
                break;

            case INTPTR_TYPE:
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 184, (int)(mt), (int)(((unsigned long)(mt))>>32));
                load_int(Dst,&regs);
                dasm_put(Dst, 197);
				
                break;

            case BOOL_TYPE:
                lua_pop(L, 1);
				
				load_int(Dst,&regs);
				dasm_put(Dst, 203);
				
                break;

            case INT8_TYPE:
				lua_pop(L, 1);
				if(regs.ints<8){
					if (mt->is_unsigned) {
						dasm_put(Dst, 207, 0x60+(regs.ints++<<3));
					} else {
						dasm_put(Dst, 210, 0x60+(regs.ints++<<3));
					}
				}else {
					if (mt->is_unsigned) {
						dasm_put(Dst, 213);
					} else {
						dasm_put(Dst, 215);
					}
				}
				dasm_put(Dst, 217);
				break;
			
            case INT16_TYPE:
				lua_pop(L, 1);
				if(regs.ints<8){
					if (mt->is_unsigned) {
						dasm_put(Dst, 221, 0x60+(regs.ints++<<3));
					} else {
						dasm_put(Dst, 224, 0x60+(regs.ints++<<3));
					}
				}else {
					if (mt->is_unsigned) {
						dasm_put(Dst, 227);
					} else {
						dasm_put(Dst, 229);
					}
				}
				dasm_put(Dst, 231);
				break;
				
            case ENUM_TYPE:
            case INT32_TYPE:
                lua_pop(L, 1);
				load_int(Dst,&regs);
				
                dasm_put(Dst, 235);
                break;

            case FLOAT_TYPE:
                lua_pop(L, 1);
				load_float(Dst,&regs,1,0);
                dasm_put(Dst, 239);
                break;

            case DOUBLE_TYPE:
                lua_pop(L, 1);
				load_float(Dst,&regs,0,0);
                dasm_put(Dst, 243);
                break;
				
            default:
                luaL_error(L, "NYI: callback arg type");
            }
        }
    }

    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    dasm_put(Dst, 247, ((mt->pointers || mt->is_reference || mt->type != VOID_TYPE) ? 1 : 0), nargs);
    

	
    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        lua_rawseti(L, -3, ++num_upvals); /* usr value */
        lua_rawseti(L, -2, ++num_upvals); /* mt */

        dasm_put(Dst, 255, num_upvals-1, (int)(mt), (int)(((unsigned long)(mt))>>32));
        goto single_no_pop;
    } else {
        switch (mt->type) {
        case ENUM_TYPE:
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */

            dasm_put(Dst, 275, num_upvals-1, (int)(mt), (int)(((unsigned long)(mt))>>32));

            goto single_no_pop;

        case VOID_TYPE:
            dasm_put(Dst, 295);
            lua_pop(L, 1);
            break;

        case BOOL_TYPE:
        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
		    dasm_put(Dst, 300);
            if (mt->is_unsigned) {
                dasm_put(Dst, 302);
            } else {
                dasm_put(Dst, 306);
            }
            goto single;

        case INT64_TYPE:
            dasm_put(Dst, 310);
            if (mt->is_unsigned) {
                dasm_put(Dst, 312);
            } else {
                dasm_put(Dst, 316);
            }
			
			goto single;

        case INTPTR_TYPE:
            dasm_put(Dst, 320);
            goto single;

        case FLOAT_TYPE:
            dasm_put(Dst, 325);
			
            dasm_put(Dst, 330);
            lua_pop(L, 1);
			break;
        case DOUBLE_TYPE:
            dasm_put(Dst, 337);
			
            dasm_put(Dst, 342);
			
            lua_pop(L, 1);
			break;
			
		case COMPLEX_DOUBLE_TYPE:

			dasm_put(Dst, 349);
		    
			goto complex_ret;
		case COMPLEX_FLOAT_TYPE:
			dasm_put(Dst, 354);
			
		complex_ret:	
			dasm_put(Dst, 359);

            lua_pop(L, 1);			
			break;
        single:
            lua_pop(L, 1);
		single_no_pop:	
            dasm_put(Dst, 368);
            break;

        
        default:
            luaL_error(L, "NYI: callback return type");
        }
    }
	
	dasm_put(Dst, 375,  (0x20 + ((regs.floats!=0)||(regs.ints!=0)) * 0x80));
	
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
			dasm_put(Dst, 381, (regs->floats++<<3));
			if(isComplex){
				if(isFloat){
					dasm_put(Dst, 384);
				}else{
					dasm_put(Dst, 386);
				}
			}
		}else{
			dasm_put(Dst, 388, (regs->floats<<3));
			regs->floats+=2;
		}
	}else if(isComplex){
		//complex float is packed as one double on stack
		if(isFloat){
			dasm_put(Dst, 391);
		}else{
			dasm_put(Dst, 393);
		}
	}else {
		if(isFloat){
			dasm_put(Dst, 395);
		}else {
			dasm_put(Dst, 397);
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
	
	dasm_put(Dst, 399);
	
    /* reserve enough stack space for all of the arguments (16 bytes per
     * argument for complex double and maintains alignment). */
	 
    if (ct->has_var_arg) {
        dasm_put(Dst, 406, nargs, (int)("too few arguments"), (int)(((unsigned long)("too few arguments"))>>32));
    }else if(nargs>0){
		dasm_put(Dst, 429, (nargs<<4)+0x80);
    }

    for (i = 1,regs.ints=0,regs.floats=0; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference || mt->type == FUNCTION_PTR_TYPE || mt->type == ENUM_TYPE) {
            lua_getuservalue(L, -1);
            num_upvals += 2;

			
			dasm_put(Dst, 433, (int)(mt), (int)(((unsigned long)(mt))>>32), (int)(lua_upvalueindex(num_upvals)), i);
			
            if (mt->pointers || mt->is_reference) {
                dasm_put(Dst, 451);
            } else if (mt->type == FUNCTION_PTR_TYPE) {
                dasm_put(Dst, 455);
            } else if (mt->type == ENUM_TYPE) {
                dasm_put(Dst, 459);
            }
			goto longstore;

        } else {
            lua_pop(L, 1);
            dasm_put(Dst, 463, i);

            switch (mt->type) {
            case BOOL_TYPE:
                dasm_put(Dst, 466);
                goto intstore;
            case INT8_TYPE:
                dasm_put(Dst, 472);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 476);
                } else {
                    dasm_put(Dst, 478);
                }
				goto intstore;

            case INT16_TYPE:
                dasm_put(Dst, 480);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 484);
                } else {
                    dasm_put(Dst, 486);
                }
				goto intstore;

            case INT32_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 488);
                } else {
                    dasm_put(Dst, 492);
                }
 				goto intstore;

            case INT64_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 496);
                } else {
                    dasm_put(Dst, 500);
                }
              	goto longstore;
				
            case INTPTR_TYPE:
                dasm_put(Dst, 504);
                
                goto longstore;

            case DOUBLE_TYPE:
                dasm_put(Dst, 508);
                storeFloat(Dst,&regs,0,0);
                break;

            case FLOAT_TYPE:
                dasm_put(Dst, 512);
                storeFloat(Dst,&regs,1,0);
                break;
			case COMPLEX_DOUBLE_TYPE:
                dasm_put(Dst, 516);
				storeFloat(Dst,&regs,0,1);
                break;

            case COMPLEX_FLOAT_TYPE:
                dasm_put(Dst, 520);
				storeFloat(Dst,&regs,1,1);
                break;
				
			intstore:
				if(regs.ints<8)
					dasm_put(Dst, 524, 0x40+(regs.ints++<<3));
				else
					dasm_put(Dst, 527);
				
                break;
			longstore:
				if(regs.ints<8)
					dasm_put(Dst, 529, 0x40+(regs.ints++<<3));
				else
					dasm_put(Dst, 532);
                break;
				
            default:
                luaL_error(L, "NYI: call arg type");
            }
        }
    }

    if (ct->has_var_arg) {
		if(regs.floats<8){
			dasm_put(Dst, 534, regs.floats<<3, (8-regs.floats), nargs+1);
		}
		if(regs.ints<8){
			dasm_put(Dst, 545, 0x40+(regs.ints<<3), (8-regs.ints), nargs+1);
		}
		dasm_put(Dst, 556, (nargs>8?nargs:8), (8-regs.floats), (8-regs.ints), nargs+1);
		regs.floats=regs.ints=8;
    }

	//pop all args in registers
	switch(regs.ints){
		case 8:
		case 7:
            dasm_put(Dst, 573);
		case 6:
		case 5:
            dasm_put(Dst, 575);
		case 4:
		case 3:
            dasm_put(Dst, 577);
		case 2:
		case 1:
			dasm_put(Dst, 579);
    }
	
	switch(regs.floats){
		case 8:
		case 7:
            dasm_put(Dst, 581);
		case 6:
		case 5:
            dasm_put(Dst, 583);
		case 4:
		case 3:
            dasm_put(Dst, 585);
		case 2:
		case 1:
			dasm_put(Dst, 587);
    }
	if(regs.ints==8|| regs.floats==8){// fix stack case registers is full
		dasm_put(Dst, 589);
	}
    dasm_put(Dst, 591, (int)(func), (int)(((unsigned long)(func))>>32));


    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    if (mt->pointers || mt->is_reference || mt->type==FUNCTION_PTR_TYPE) {
        lua_getuservalue(L, -1);
        num_upvals += 2;
        dasm_put(Dst, 601, (int)(mt), (int)(((unsigned long)(mt))>>32), (int)(lua_upvalueindex(num_upvals)));

    } else {
        switch (mt->type) {
        case INT64_TYPE:
		#if LUA_VERSION_NUM>=503
			 lua_pop(L, 1);
            dasm_put(Dst, 629);
            break;
		#endif

        case INTPTR_TYPE:
            num_upvals++;
            dasm_put(Dst, 641, (int)(mt), (int)(((unsigned long)(mt))>>32));
			break;
        case VOID_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 663);
            break;

        case BOOL_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 671);
            break;

        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
        case ENUM_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 683);
            if (mt->is_unsigned) {
                dasm_put(Dst, 685);
            } else {
                dasm_put(Dst, 689);
            }
            dasm_put(Dst, 693);
            break;

        case FLOAT_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 701);
            break;

        case DOUBLE_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 712);
            break;
		case COMPLEX_FLOAT_TYPE:
            lua_getuservalue(L, -1);
            num_upvals+=2;
            dasm_put(Dst, 723, (int)(mt), (int)(((unsigned long)(mt))>>32), (int)(lua_upvalueindex(num_upvals)));
            break;

        case COMPLEX_DOUBLE_TYPE:
            lua_getuservalue(L, -1);
            num_upvals+=2;
            dasm_put(Dst, 754, (int)(mt), (int)(((unsigned long)(mt))>>32), (int)(lua_upvalueindex(num_upvals)));
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

