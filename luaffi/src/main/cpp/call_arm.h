/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM arm version 1.4.0
** DO NOT EDIT! The original file is in "E:/AppProjects/luadroid/luaffi/src/main/cpp/call_arm.dasc".
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

static const unsigned int build_actionlist[703] = {
0xe1a0c00d,
0xe92d000f,
0xe92d50f0,
0xe24c6010,
0xe59f5008,
0xe59f2008,
0xe59f1008,
0xea000000,
0x00050001,
0x00090000,
0x00090000,
0x00090000,
0x0006000b,
0xe1a00005,
0xeb000000,
0x00030000,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030000,
0x00000000,
0xe2866004,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01000,
0x000b0000,
0xe1a00005,
0xeb000000,
0x00030000,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe4962004,
0xe5802000,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x00030002,
0x00000000,
0xe2866004,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe4962004,
0xe4963004,
0xe5802000,
0xe5803004,
0xe4962004,
0xe4963004,
0xe5802008,
0xe580300c,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x00030002,
0x00000000,
0xe2866004,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe4962004,
0xe4963004,
0xe5802000,
0xe5803004,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x00030002,
0x00000000,
0xe2866004,
0x00000000,
0xe4961004,
0xe4962004,
0xe1a00005,
0xeb000000,
0x00030004,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe4962004,
0xe4963004,
0xe5802000,
0xe5803004,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x00030002,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe4962004,
0xe5802000,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x00030002,
0x00000000,
0xe4961004,
0xe1a00005,
0xeb000000,
0x00030005,
0x00000000,
0xe4961004,
0xe1a01c01,
0x00000000,
0xe1a01c21,
0x00000000,
0xe1a01c41,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030006,
0x00000000,
0xe4961004,
0xe1a01801,
0x00000000,
0xe1a01821,
0x00000000,
0xe1a01841,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030006,
0x00000000,
0xe4961004,
0xe1a00005,
0xeb000000,
0x00030006,
0x00000000,
0xe4961004,
0xe1a00005,
0xeb000000,
0x00030007,
0x00000000,
0xe2866004,
0x00000000,
0xe8b60006,
0xe1a00005,
0xeb000000,
0x00030008,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3a01000,
0x000b0000,
0xe1a00005,
0xeb000000,
0x00030009,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x00030000,
0xe59f3000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e02000,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x0003000a,
0x00000000,
0xe3a02000,
0x000b0000,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x00030000,
0xe59f3000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e02000,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x0003000b,
0x00000000,
0xe3e01001,
0xe1a00005,
0xeb000000,
0x0003000c,
0x00000000,
0xe3e01000,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000d,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000e,
0x00000000,
0xe3e01000,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000f,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030010,
0x00000000,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030011,
0x00000000,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030012,
0x00000000,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030013,
0x00000000,
0xe3e02000,
0xe1a01005,
0xe59d0020,
0xeb000000,
0x00030014,
0xe3e01002,
0xe1a00005,
0xeb000000,
0x0003000c,
0x00000000,
0xe3e02000,
0xe1a01005,
0xe59d0020,
0xeb000000,
0x00030015,
0xe3e01002,
0xe1a00005,
0xeb000000,
0x0003000c,
0x00000000,
0xe1a06000,
0xe3e01002,
0xe1a00005,
0xeb000000,
0x0003000c,
0xe1a00006,
0x00000000,
0xe1a06000,
0xe1a07001,
0xe3e01002,
0xe1a00005,
0xeb000000,
0x0003000c,
0xe1a00006,
0xe1a01007,
0x00000000,
0xe89da0f0,
0x00000000,
0xe1a0c00d,
0xe92d5cf0,
0xe1a0b00c,
0xe1a05000,
0xe1a00005,
0xeb000000,
0x00030016,
0xe1a04000,
0xe3540000,
0x000b0000,
0x00000000,
0xaa000000,
0x00050001,
0x00000000,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe1a00005,
0xeb000000,
0x00030017,
0x0006000b,
0x00000000,
0x0a000000,
0x00050001,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe1a00005,
0xeb000000,
0x00030017,
0x0006000b,
0x00000000,
0xe24dd010,
0xe1a0700d,
0x00000000,
0xe04dd204,
0xe24dd010,
0xe1a0600d,
0x00000000,
0xe59f3004,
0xe59f2004,
0xea000000,
0x00050001,
0x00090000,
0x00090000,
0x0006000b,
0xe3a01000,
0x000b0000,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000a,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030018,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000b,
0x00000000,
0xe4860004,
0x00000000,
0xe3a01000,
0x000b0000,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000e,
0x00000000,
0xe20000ff,
0x00000000,
0xe1a00c00,
0xe1a00c40,
0x00000000,
0xe4860004,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000e,
0x00000000,
0xe1a00800,
0xe1a00820,
0x00000000,
0xe1a00800,
0xe1a00840,
0x00000000,
0xe4860004,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000d,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000e,
0x00000000,
0xe4860004,
0x00000000,
0xe2866004,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003000f,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030010,
0x00000000,
0xe4860004,
0xe4861004,
0x00000000,
0xe2866004,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030013,
0xe4860004,
0xe4861004,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030012,
0xe4860004,
0x00000000,
0xe2866004,
0x00000000,
0xe1a02001,
0xe1a01005,
0xe1a00006,
0xeb000000,
0x00030014,
0xe2866010,
0x00000000,
0xe2866004,
0x00000000,
0xe1a02001,
0xe1a01005,
0xe1a00006,
0xeb000000,
0x00030015,
0xe2866008,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030011,
0xe4860004,
0x00000000,
0xe1a03006,
0xe1a02004,
0xe3a01000,
0x000b0000,
0xe1a00005,
0xeb000000,
0x00030019,
0x00000000,
0xe59f0000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5900000,
0xeb000000,
0x0003001a,
0x00000000,
0xe8bd000f,
0x00000000,
0xe1a00007,
0x00000000,
0xeb000000,
0x0003001b,
0x00000000,
0xe1a06000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe59f2004,
0xe59f1004,
0xea000000,
0x00050001,
0x00090000,
0x00090000,
0x0006000b,
0xe1a00005,
0xeb000000,
0x00030001,
0xe5806000,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xe1a06000,
0xe1a07001,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe5806000,
0xe5807004,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xe1a06000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe5806000,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe3a00000,
0xe91bacf0,
0x00000000,
0xe1a06000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a01006,
0xe1a00005,
0xeb000000,
0x00030005,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xe1a06000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a01006,
0x00000000,
0xe1a00005,
0xeb000000,
0x0003001d,
0x00000000,
0xe1a00005,
0xeb000000,
0x00030006,
0x00000000,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xe1a06000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a01006,
0xe1a00005,
0xeb000000,
0x00030007,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xe1a06000,
0xe1a07001,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a02007,
0xe1a01006,
0xe1a00005,
0xeb000000,
0x00030008,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe5976000,
0xe5806000,
0xe5976004,
0xe5806004,
0xe5976008,
0xe5806008,
0xe597600c,
0xe580600c,
0xe3a00001,
0xe91bacf0,
0x00000000,
0xeb000000,
0x0003001c,
0xe59f1000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe5810000,
0xe1a00005,
0xeb000000,
0x00030003,
0xe59f2000,
0xea000000,
0x00050005,
0x00090000,
0x0006000f,
0xe3e01000,
0xe1a00005,
0xeb000000,
0x00030001,
0xe5976000,
0xe5806000,
0xe5976004,
0xe5806004,
0xe3a00001,
0xe91bacf0,
0x00000000
};

static const char *const globnames[] = {
  (const char *)0
};
static const char *const extnames[] = {
  "lua_rawgeti",
  "push_cdata",
  "lua_remove",
  "lua_pushnil",
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
  "SetLastError",
  "FUNCTION",
  "GetLastError",
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
#define ALIGNED(x, align) (((int)(x) & (align - 1)) == 0)
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
    /* ldr pc, [pc - 12] */
    *(uint32_t*) &code[4] = 0xE51FF00CU;
	
}





void compile_globals(struct jit* jit, lua_State* L)
{
    (void) jit;
}

cfunction compile_callback(lua_State* L, int fidx, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals, ref,regOff;
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
	
    /* prolog and get the upval table */
    dasm_put(Dst, 0, (int)(L), (int)(ref), (int)(LUA_REGISTRYINDEX));
	
    /* get the lua function */
    lua_pushvalue(L, fidx);
    lua_rawseti(L, -2, ++num_upvals);
	
	
    dasm_put(Dst, 17, num_upvals);
	
	// Complex type is return in the address stored in r0
	lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);
	regOff=0;
	if(!mt->pointers && !mt->is_reference&&(mt->type==COMPLEX_DOUBLE_TYPE||mt->type==COMPLEX_FLOAT_TYPE)){
		dasm_put(Dst, 24);
		regOff=4;
	} 
	lua_pop(L,1);
	
	//whether 64 bit type requires 8 bytes alignment in stack is defined by compiler.android compiler reqiures only 4 byte alignment;
	//actually the stack it self may reqiures 8 bytes alignment
    for (i = 1; i <= nargs; i++,regOff+=ROUND_UP(mt->base_size,4)) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference) {
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */
			
            dasm_put(Dst, 26, num_upvals-1, i, (int)(mt));

        } else {
            switch (mt->type) {
			case COMPLEX_DOUBLE_TYPE:
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 49);
				}
				lua_rawseti(L, -2, ++num_upvals); /* mt */
                dasm_put(Dst, 51, (int)(mt));
				break;
			case COMPLEX_FLOAT_TYPE:
				lua_rawseti(L, -2, ++num_upvals); /* mt */
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 76);
				}
                dasm_put(Dst, 78, (int)(mt));
				break;
            case INT64_TYPE:
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 99);
				}
			    #if LUA_VERSION_NUM>=503
                lua_pop(L, 1);
				
                dasm_put(Dst, 101);
				#else
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 107, (int)(mt));
				#endif
                break;

            case INTPTR_TYPE:
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 128, (int)(mt));
				
                break;

            case BOOL_TYPE:
                lua_pop(L, 1);
				
                dasm_put(Dst, 147);
                break;

            case INT8_TYPE:
                lua_pop(L, 1);
                
                dasm_put(Dst, 152);
				
                if (mt->is_unsigned) {
                    dasm_put(Dst, 155);
                } else {
                    dasm_put(Dst, 157);
                }
                dasm_put(Dst, 159);
                break;

            case INT16_TYPE:
                lua_pop(L, 1);
                dasm_put(Dst, 163);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 166);
                } else {
                    dasm_put(Dst, 168);
                }
                dasm_put(Dst, 170);
                break;

            case ENUM_TYPE:
            case INT32_TYPE:
                lua_pop(L, 1);
				
                dasm_put(Dst, 174);
                break;

            case FLOAT_TYPE:
                lua_pop(L, 1);
                
                dasm_put(Dst, 179);
                break;

            case DOUBLE_TYPE:
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 184);
				}
                lua_pop(L, 1);
                dasm_put(Dst, 186);
                break;
				
            default:
                luaL_error(L, "NYI: callback arg type");
            }
        }
    }

    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    dasm_put(Dst, 191, ((mt->pointers || mt->is_reference || mt->type != VOID_TYPE) ? 1 : 0), nargs);
    

	
    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        lua_rawseti(L, -3, ++num_upvals); /* usr value */
        lua_rawseti(L, -2, ++num_upvals); /* mt */

        dasm_put(Dst, 199, num_upvals-1, (int)(mt));
        goto single;
    } else {
        switch (mt->type) {
        case ENUM_TYPE:
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */

            dasm_put(Dst, 216, num_upvals-1, (int)(mt));

            goto single;

        case VOID_TYPE:
            dasm_put(Dst, 233);
            lua_pop(L, 1);
            break;

        case BOOL_TYPE:
        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
		    dasm_put(Dst, 238);
            if (mt->is_unsigned) {
                dasm_put(Dst, 240);
            } else {
                dasm_put(Dst, 244);
            }
            goto single;

        case INT64_TYPE:
            dasm_put(Dst, 248);
            if (mt->is_unsigned) {
                dasm_put(Dst, 250);
            } else {
                dasm_put(Dst, 254);
            }
            goto dual;

        case INTPTR_TYPE:
            dasm_put(Dst, 258);
            goto single;

        case FLOAT_TYPE:
            dasm_put(Dst, 263);
            goto single;

        case DOUBLE_TYPE:
            dasm_put(Dst, 268);
           
            goto dual;
		case COMPLEX_DOUBLE_TYPE:
            lua_pop(L, 1);
			
            dasm_put(Dst, 273);
			
			break;
		case COMPLEX_FLOAT_TYPE:
            lua_pop(L, 1);
			
            dasm_put(Dst, 283);
			break;
			
        single:
		    
            dasm_put(Dst, 293);
			
            lua_pop(L, 1);
            break;

        dual:
		
            dasm_put(Dst, 300);
			
            lua_pop(L, 1);
            break;

        
        default:
            luaL_error(L, "NYI: callback return type");
        }
    }
	
    dasm_put(Dst, 309);
	
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

void compile_function(lua_State* L, cfunction func, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i, nargs, num_upvals,retByAddr,regOff;
    const struct ctype* mt;
    void* p;

    int top = lua_gettop(L);

    ct_usr = lua_absindex(L, ct_usr);
    nargs = (int) lua_rawlen(L, ct_usr);

    p = push_cdata(L, ct_usr, ct);
    *(cfunction*) p = func;
    num_upvals = 1;

    dasm_setup(Dst, build_actionlist);

    dasm_put(Dst, 311, nargs);
	
    if (ct->has_var_arg) {
        dasm_put(Dst, 322);
		
        dasm_put(Dst, 325, (int)("too few arguments"));
    } else {
        dasm_put(Dst, 335, (int)("incorrect number of arguments"));
    }

	lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);
	retByAddr=!mt->pointers && !mt->is_reference && (mt->type==COMPLEX_DOUBLE_TYPE||mt->type==COMPLEX_FLOAT_TYPE);
    if (retByAddr) {
		dasm_put(Dst, 347);
	}
	lua_pop(L,1);
    /* reserve enough stack space for all of the arguments (16 bytes per
     * argument for complex double and maintains alignment). Add an extra 16/64 bytes so
     * that the pop {r0, r1, r2, r3} or pop arguments doesn't clean out our stack frame */
    dasm_put(Dst, 350);
	
	// Complex type is return in the address stored in r0
	regOff=0;
    for (i = 1; i <= nargs; i++,regOff+=ROUND_UP(mt->base_size,4)) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);
        if (mt->pointers || mt->is_reference || mt->type == FUNCTION_PTR_TYPE || mt->type == ENUM_TYPE) {
            lua_getuservalue(L, -1);
            num_upvals += 2;

			
            dasm_put(Dst, 354, (int)(mt), (int)(lua_upvalueindex(num_upvals)), i);
			
            if (mt->pointers || mt->is_reference) {
                dasm_put(Dst, 364);
            } else if (mt->type == FUNCTION_PTR_TYPE) {
                dasm_put(Dst, 368);
            } else if (mt->type == ENUM_TYPE) {
                dasm_put(Dst, 372);
            }

            dasm_put(Dst, 376);
			
        } else {
            lua_pop(L, 1);
            dasm_put(Dst, 378, i);

            switch (mt->type) {
            case INT8_TYPE:
                dasm_put(Dst, 381);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 385);
                } else {
                    dasm_put(Dst, 387);
                }
                dasm_put(Dst, 390);
                break;

            case INT16_TYPE:
                dasm_put(Dst, 392);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 396);
                } else {
                    dasm_put(Dst, 399);
                }
                dasm_put(Dst, 402);
                break;

            case INT32_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 404);
                } else {
                    dasm_put(Dst, 408);
                }
                dasm_put(Dst, 412);
                break;

            case INT64_TYPE:
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 414);
				}
                if (mt->is_unsigned) {
                    dasm_put(Dst, 416);
                } else {
                    dasm_put(Dst, 420);
                }
                dasm_put(Dst, 424);
                break;

            case DOUBLE_TYPE:
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 427);
				}
                dasm_put(Dst, 429);
                break;

            case FLOAT_TYPE:
                dasm_put(Dst, 435);
                break;
			case COMPLEX_DOUBLE_TYPE:
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 440);
				}
				dasm_put(Dst, 442);
				break;
			case COMPLEX_FLOAT_TYPE:
				if(regOff<16&&!ALIGNED(regOff,8)){
					regOff+=4;
					dasm_put(Dst, 449);
				}
				dasm_put(Dst, 451);
                break;
            case INTPTR_TYPE:
                dasm_put(Dst, 458);
                break;
            default:
                luaL_error(L, "NYI: call arg type");
            }
        }
    }

    if (ct->has_var_arg) {
        dasm_put(Dst, 463, nargs+1);
    }

    dasm_put(Dst, 471, (int)(&Dst->last_errno));
     
	dasm_put(Dst, 480);
	if(retByAddr){
		dasm_put(Dst, 482);
	} 
	
    dasm_put(Dst, 484);



    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        num_upvals += 2;
        dasm_put(Dst, 487, (int)(&Dst->last_errno), (int)(mt), (int)(lua_upvalueindex(num_upvals)));

    } else {
        switch (mt->type) {
        case INT64_TYPE:
            num_upvals++;
            dasm_put(Dst, 510, (int)(&Dst->last_errno), (int)(mt));
            break;

        case INTPTR_TYPE:
            num_upvals++;
            dasm_put(Dst, 537, (int)(&Dst->last_errno), (int)(mt));
            break;

        case VOID_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 562, (int)(&Dst->last_errno));
            break;

        case BOOL_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 573, (int)(&Dst->last_errno));
            break;

        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
        case ENUM_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 589, (int)(&Dst->last_errno));
            if (mt->is_unsigned) {
                dasm_put(Dst, 600);
            } else {
                dasm_put(Dst, 604);
            }
            dasm_put(Dst, 608);
            break;

        case FLOAT_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 611, (int)(&Dst->last_errno));
            break;

        case DOUBLE_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 627, (int)(&Dst->last_errno));
            break;
		case COMPLEX_DOUBLE_TYPE:
			 num_upvals++;
            dasm_put(Dst, 645, (int)(&Dst->last_errno), (int)(mt));
			break;
		case COMPLEX_FLOAT_TYPE:
			 num_upvals++;
            dasm_put(Dst, 676, (int)(&Dst->last_errno), (int)(mt));
			break;
        default:
            luaL_error(L, "NYI: call return type");
        }
    }

    assert(lua_gettop(L) == top + num_upvals);
    lua_pushcclosure(L, (lua_CFunction) compile(Dst, L, func, LUA_NOREF), num_upvals);
}

