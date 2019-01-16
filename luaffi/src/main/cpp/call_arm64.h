/*
** This file has been pre-processed with DynASM.
** http://luajit.org/dynasm.html
** DynASM version 1.4.0, DynASM arm version 1.4.0
** DO NOT EDIT! The original file is in "E:/AppProjects/luadroid/luaffi/src/main/cpp/call_arm64.dasc".
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

static const unsigned int build_actionlist[976] = {
0x910003e9,
0xd10103ff,
0xa90307e0,
0xa9020fe2,
0xa90117e4,
0xa9001fe6,
0x910003ea,
0xd10103ff,
0x6d0307e0,
0x6d020fe2,
0x6d0117e4,
0x6d001fe6,
0x910003eb,
0xd100c3ff,
0xa9027bfd,
0xa90157f4,
0xa9005ff6,
0x00000000,
0xaa0b03f7,
0xaa0a03f6,
0xaa0903f5,
0xaa0903fd,
0x58000014,
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
0x00090000,
0x00060010,
0x18000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xaa1403e0,
0x94000000,
0x00030000,
0x00000000,
0x52800002,
0x000a0205,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030000,
0x00000000,
0x52800002,
0x000a0205,
0x12800001,
0x000a0205,
0xaa1403e0,
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
0xaa1403e0,
0x94000000,
0x00030001,
0x00000000,
0xf84086c2,
0x00000000,
0xf84086a2,
0x00000000,
0xf9000002,
0x12800021,
0xaa1403e0,
0x94000000,
0x00030002,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030003,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030001,
0x00000000,
0xf84086a1,
0xf9000001,
0x00000000,
0xf84086e1,
0xf9000401,
0x00000000,
0xf84086a1,
0xf9000401,
0x00000000,
0xa8c10aa1,
0xa9000801,
0x00000000,
0x12800021,
0xaa1403e0,
0x94000000,
0x00030002,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030003,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030001,
0x00000000,
0xb84086a1,
0xb9000001,
0x00000000,
0xb84086e1,
0xb9000401,
0x00000000,
0xb84086a1,
0xb9000401,
0x00000000,
0x28c10aa1,
0x29000801,
0x00000000,
0xf9000002,
0x12800021,
0xaa1403e0,
0x94000000,
0x00030002,
0x00000000,
0xf84086c1,
0x00000000,
0xf84086a1,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030004,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030003,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030001,
0x00000000,
0xf84086c1,
0x00000000,
0xf84086a1,
0x00000000,
0xf9000001,
0x12800021,
0xaa1403e0,
0x94000000,
0x00030002,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030003,
0x18000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030001,
0x00000000,
0xb84086c1,
0x00000000,
0xb84086a1,
0x00000000,
0xf9000001,
0x12800021,
0xaa1403e0,
0x94000000,
0x00030002,
0x00000000,
0xb84086c1,
0x00000000,
0xb84086a1,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030005,
0x00000000,
0xb84086c1,
0x00000000,
0xb84086a1,
0x00000000,
0x2a0163e1,
0x00000000,
0x2a4163e1,
0x00000000,
0x2a8163e1,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030006,
0x00000000,
0xb84086c1,
0x00000000,
0xb84086a1,
0x00000000,
0x2a0143e1,
0x00000000,
0x2a4143e1,
0x00000000,
0x2a8143e1,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030006,
0x00000000,
0xb84086c1,
0x00000000,
0xb84086a1,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030006,
0x00000000,
0xbc4086e0,
0x00000000,
0xbc4086a0,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030007,
0x00000000,
0xfc4086e0,
0x00000000,
0xfc4086a0,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030008,
0x00000000,
0x52800002,
0x000a0205,
0x52800001,
0x000a0205,
0xaa1403e0,
0x94000000,
0x00030009,
0x00000000,
0x52800002,
0x000a0205,
0x12800021,
0xaa1403e0,
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
0x12800002,
0x12800021,
0xaa1403e0,
0x94000000,
0x0003000a,
0x00000000,
0x52800002,
0x000a0205,
0x12800021,
0xaa1403e0,
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
0x12800002,
0x12800021,
0xaa1403e0,
0x94000000,
0x0003000b,
0x00000000,
0x12800021,
0xaa1403e0,
0x94000000,
0x0003000c,
0x00000000,
0x12800001,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000d,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000e,
0x00000000,
0x12800001,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000f,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030010,
0x00000000,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030011,
0x00000000,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030012,
0x00000000,
0x9e660015,
0x12800041,
0xaa1403e0,
0x94000000,
0x0003000c,
0x9e6702a0,
0x00000000,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030013,
0x00000000,
0x9e660015,
0x12800041,
0xaa1403e0,
0x94000000,
0x0003000c,
0x9e6702a0,
0x00000000,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030014,
0x00000000,
0x9e660015,
0x9e660036,
0x12800041,
0xaa1403e0,
0x94000000,
0x0003000c,
0x9e6702a0,
0x9e6702c1,
0x00000000,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030015,
0x00000000,
0x9e660015,
0x9e660036,
0x12800041,
0xaa1403e0,
0x94000000,
0x0003000c,
0x9e6702a0,
0x9e6702c1,
0x00000000,
0xaa0003f5,
0x12800041,
0xaa1403e0,
0x94000000,
0x0003000c,
0xaa1503e0,
0x00000000,
0xaa1d03e9,
0xa9405ff6,
0xa94157f4,
0xa9427bfd,
0x9100013f,
0xd65f03c0,
0x00000000,
0xd10103ff,
0xa9037bfd,
0xa90253f3,
0xa9015bf5,
0xf90003f7,
0x910003fd,
0xaa0003f4,
0xaa1403e0,
0x94000000,
0x00030016,
0xaa0003f3,
0xf100027f,
0x000c0000,
0x00000000,
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
0xaa1403e0,
0x94000000,
0x00030017,
0x0006000b,
0x00000000,
0x54000000,
0x00050801,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xaa1403e0,
0x94000000,
0x00030017,
0x0006000b,
0x00000000,
0xf100227f,
0x5400000d,
0x00050801,
0xd1002269,
0xcb2973ff,
0x910003f5,
0x0006000b,
0xd10103ff,
0x910003f6,
0xd10103ff,
0x910003f7,
0x00000000,
0x58000003,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x52800001,
0x000a0205,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000a,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030018,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000b,
0x00000000,
0x52800001,
0x000a0205,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000e,
0x00000000,
0x12001c00,
0x00000000,
0x2a0063e0,
0x2a8063e0,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000e,
0x00000000,
0x2a0043e0,
0x2a4043e0,
0x00000000,
0x2a0043e0,
0x2a8043e0,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000d,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000e,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003000f,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030010,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030013,
0x00000000,
0xfc0086e0,
0x00000000,
0xfc0086a0,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030012,
0x00000000,
0xbc0086e0,
0x00000000,
0xbc0086a0,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030014,
0x00000000,
0xfc0086e0,
0x00000000,
0xfc0086e1,
0x00000000,
0xfc0086a1,
0x00000000,
0x6c8106a0,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030015,
0x00000000,
0xbc0086e0,
0x00000000,
0xbc0086e1,
0x00000000,
0xbc0086a1,
0x00000000,
0x2c8106a0,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030011,
0x00000000,
0xb80086c0,
0x00000000,
0xb80086a0,
0x00000000,
0xf80086c0,
0x00000000,
0xf80086a0,
0x00000000,
0xaa1703e4,
0x52800003,
0x000a0205,
0xaa1303e2,
0x52800001,
0x000a0205,
0xaa1403e0,
0x94000000,
0x00030019,
0x00000000,
0xaa1603e4,
0x52800003,
0x000a0205,
0xaa1303e2,
0x52800001,
0x000a0205,
0xaa1403e0,
0x94000000,
0x0003001a,
0x00000000,
0xf100227f,
0x5400000d,
0x00050801,
0xaa1503e5,
0x52800003,
0x000a0205,
0x52800003,
0x000a0205,
0xaa1303e2,
0x52800001,
0x000a0205,
0xaa1403e0,
0x94000000,
0x0003001b,
0x0006000b,
0x00000000,
0x58000000,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9400000,
0x94000000,
0x0003001c,
0x00000000,
0x6d4007e0,
0x6d410fe2,
0x6d4217e4,
0x6d431fe6,
0xa94407e0,
0xa9450fe2,
0xa94617e4,
0xa9471fe6,
0x910203ff,
0x00000000,
0x94000000,
0x0003001d,
0x00000000,
0xaa0003f5,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xaa1403e0,
0x94000000,
0x00030001,
0xf9000015,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0xaa0003f5,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0xaa1503e1,
0xaa1403e0,
0x94000000,
0x00030004,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0xaa0003f5,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0xaa1403e0,
0x94000000,
0x00030003,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030001,
0xf9000015,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0xaa0003f5,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0xaa1403e0,
0x94000000,
0x00030003,
0x58000002,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0x12800001,
0xaa1403e0,
0x94000000,
0x00030001,
0xf9000015,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0x52800000,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0xaa0003f5,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0xaa1503e1,
0xaa1403e0,
0x94000000,
0x00030005,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0xaa0003f5,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0xaa1503e1,
0x00000000,
0xaa1403e0,
0x94000000,
0x0003001f,
0x00000000,
0xaa1403e0,
0x94000000,
0x00030006,
0x00000000,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0x9e660015,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0x9e6702a0,
0xaa1403e0,
0x94000000,
0x00030007,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0x9e660015,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0x9e6702a0,
0xaa1403e0,
0x94000000,
0x00030008,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0x9e660015,
0x9e660036,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0x9e6702a0,
0x9e6702c1,
0xaa1403e0,
0x94000000,
0x00030007,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
0x00000000,
0x9e660015,
0x9e660036,
0x94000000,
0x0003001e,
0x58000001,
0x00050805,
0x14000000,
0x00050006,
0x0006000f,
0x00090000,
0x00090000,
0x00060010,
0xb9000020,
0x9e6702a0,
0x9e6702c1,
0xaa1403e0,
0x94000000,
0x00030008,
0x52800020,
0x910003bf,
0xf94003f7,
0xa9415bf5,
0xa94253f3,
0xa9437bfd,
0x910103ff,
0xd65f03c0,
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
  "unpack_varargs_float",
  "unpack_varargs_int",
  "unpack_varargs_stack_skip",
  "SetLastError",
  "FUNCTION",
  "GetLastError",
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
	 
	 /*l: ptr*/
	 *(cfunction*) code = func;
	 /* ldr x9,#-8*/
	  *(uint32_t*) &code[8] = 0x58FFFFC9;
	 /*br x9*/
	 *(uint32_t*) &code[12] = 0xD61F0120;
	
}



void compile_globals(struct jit* jit, lua_State* L)
{
    (void) jit;
}

cfunction compile_callback(lua_State* L, int fidx, int ct_usr, const struct ctype* ct)
{
    struct jit* Dst = get_jit(L);;
    int i,fi,ii, nargs, num_upvals, ref;
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
	
	
	//8 integer reigsters and 8 floating registers
	dasm_put(Dst, 0);
	
	dasm_put(Dst, 18, (int)(L), (int)(((unsigned long)(L))>>32), (int)(ref), (int)(((unsigned long)(ref))>>32), (int)(LUA_REGISTRYINDEX), (int)(((unsigned long)(LUA_REGISTRYINDEX))>>32));
	
    /* get the lua function */
    lua_pushvalue(L, fidx);
    lua_rawseti(L, -2, ++num_upvals);
	
	
    dasm_put(Dst, 50, num_upvals);
	
#define isReg(x) ((x)==8?0:++(x))

    for (i = 1,ii=0,fi=0; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference) {
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */
			
            dasm_put(Dst, 57, num_upvals-1, i, (int)(mt), (int)(((unsigned long)(mt))>>32));
			if(isReg(ii)){
				dasm_put(Dst, 77);
			}else {
				dasm_put(Dst, 79);
			}
            dasm_put(Dst, 81);

        } else {
            switch (mt->type) {
			case COMPLEX_DOUBLE_TYPE:
				lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 87, (int)(mt), (int)(((unsigned long)(mt))>>32));
				if(isReg(fi)){
					dasm_put(Dst, 103);
					if(isReg(fi)){
						dasm_put(Dst, 106);
					}else {
						dasm_put(Dst, 109);
					}
				}else {
					dasm_put(Dst, 112);
				}
					
                dasm_put(Dst, 115);
				
				break;
			case COMPLEX_FLOAT_TYPE:
				lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 120, (int)(mt), (int)(((unsigned long)(mt))>>32));
				if(isReg(fi)){
					dasm_put(Dst, 136);
					if(isReg(fi)){
						dasm_put(Dst, 139);
					}else {
						dasm_put(Dst, 142);
					}
				}else {
					dasm_put(Dst, 145);
				}
                dasm_put(Dst, 148);
				
				break;
            case INT64_TYPE:
			    #if LUA_VERSION_NUM>=503
                lua_pop(L, 1);
				if(isReg(ii)){
					dasm_put(Dst, 154);
				}else{
					dasm_put(Dst, 156);
				} 
                dasm_put(Dst, 158);
				
				#else
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 162, (int)(mt), (int)(((unsigned long)(mt))>>32));
                if(isReg(ii)){
					dasm_put(Dst, 178);
				}else{ 
					dasm_put(Dst, 180);
				}
                dasm_put(Dst, 182);
				
				#endif
                break;

            case INTPTR_TYPE:
                lua_rawseti(L, -2, ++num_upvals); /* mt */
				
                dasm_put(Dst, 188, (int)(mt), (int)(((unsigned long)(mt))>>32));
                if(isReg(ii)){
					dasm_put(Dst, 204);
				}else {
					dasm_put(Dst, 206);
				}
                dasm_put(Dst, 208);
				
                break;

            case BOOL_TYPE:
                lua_pop(L, 1);
				
				 if(isReg(ii)){
					dasm_put(Dst, 214);
				}else{
					dasm_put(Dst, 216);
                }
				dasm_put(Dst, 218);
				
                break;

            case INT8_TYPE:
                lua_pop(L, 1);
               
                if(isReg(ii)){
					dasm_put(Dst, 222);
				}else {
					dasm_put(Dst, 224);
                }
				dasm_put(Dst, 226);
				
                if (mt->is_unsigned) {
                    dasm_put(Dst, 228);
                } else {
                    dasm_put(Dst, 230);
					
                }
                dasm_put(Dst, 232);
                break;

            case INT16_TYPE:
                lua_pop(L, 1);
				
                if(isReg(ii)){
					dasm_put(Dst, 236);
				}else {
					dasm_put(Dst, 238);
				}
				dasm_put(Dst, 240);
				
                if (mt->is_unsigned) {
                    dasm_put(Dst, 242);
                } else {
                    dasm_put(Dst, 244);
                }
                dasm_put(Dst, 246);
                break;

            case ENUM_TYPE:
            case INT32_TYPE:
                lua_pop(L, 1);
				
				if(isReg(ii)){
					dasm_put(Dst, 250);
				}else {
					dasm_put(Dst, 252);
				}
				
                dasm_put(Dst, 254);
                break;

            case FLOAT_TYPE:
                lua_pop(L, 1);
				
				if(isReg(fi)){
					dasm_put(Dst, 258);
				}else{
					dasm_put(Dst, 260);
				} 
                dasm_put(Dst, 262);
				
                break;

            case DOUBLE_TYPE:
                lua_pop(L, 1);
				if(isReg(fi)){
					dasm_put(Dst, 266);
				}else {
					dasm_put(Dst, 268);
				}
                dasm_put(Dst, 270);
                break;
				
            default:
                luaL_error(L, "NYI: callback arg type");
            }
        }
    }

    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    dasm_put(Dst, 274, ((mt->pointers || mt->is_reference || mt->type != VOID_TYPE) ? 1 : 0), nargs);
    

	
    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        lua_rawseti(L, -3, ++num_upvals); /* usr value */
        lua_rawseti(L, -2, ++num_upvals); /* mt */

        dasm_put(Dst, 282, num_upvals-1, (int)(mt), (int)(((unsigned long)(mt))>>32));
        goto single;
    } else {
        switch (mt->type) {
        case ENUM_TYPE:
            lua_getuservalue(L, -1);
            lua_rawseti(L, -3, ++num_upvals); /* usr value */
            lua_rawseti(L, -2, ++num_upvals); /* mt */

            dasm_put(Dst, 302, num_upvals-1, (int)(mt), (int)(((unsigned long)(mt))>>32));

            goto single;

        case VOID_TYPE:
            dasm_put(Dst, 322);
            lua_pop(L, 1);
            break;

        case BOOL_TYPE:
        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
		    dasm_put(Dst, 327);
            if (mt->is_unsigned) {
                dasm_put(Dst, 329);
            } else {
                dasm_put(Dst, 333);
            }
            goto single;

        case INT64_TYPE:
            dasm_put(Dst, 337);
            if (mt->is_unsigned) {
                dasm_put(Dst, 339);
            } else {
                dasm_put(Dst, 343);
            }
			
			goto single;

        case INTPTR_TYPE:
            dasm_put(Dst, 347);
            goto single;

        case FLOAT_TYPE:
            dasm_put(Dst, 352);
			
            dasm_put(Dst, 357);
            lua_pop(L, 1);
			break;
        case DOUBLE_TYPE:
            dasm_put(Dst, 364);
			
            dasm_put(Dst, 369);
			
            lua_pop(L, 1);
			break;
			
		case COMPLEX_DOUBLE_TYPE:

			dasm_put(Dst, 376);
		    
			dasm_put(Dst, 381);
			
            lua_pop(L, 1);
			break;	
		case COMPLEX_FLOAT_TYPE:
			dasm_put(Dst, 390);
			
			dasm_put(Dst, 395);

            lua_pop(L, 1);			
			break;
        single:
            dasm_put(Dst, 404);
            lua_pop(L, 1);
            break;

        
        default:
            luaL_error(L, "NYI: callback return type");
        }
    }
	
	
	dasm_put(Dst, 411);
	
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
    int i, nargs, num_upvals,fi,ii;
    const struct ctype* mt;
    void* p;

    int top = lua_gettop(L);

    ct_usr = lua_absindex(L, ct_usr);
    nargs = (int) lua_rawlen(L, ct_usr);

    p = push_cdata(L, ct_usr, ct);
    *(cfunction*) p = func;
    num_upvals = 1;

    dasm_setup(Dst, build_actionlist);

    dasm_put(Dst, 418, nargs);
	
    if (ct->has_var_arg) {
        dasm_put(Dst, 432, (int)("too few arguments"), (int)(((unsigned long)("too few arguments"))>>32));
    } else {
        dasm_put(Dst, 447, (int)("incorrect number of arguments"), (int)(((unsigned long)("incorrect number of arguments"))>>32));
    }

    /* reserve enough stack space for all of the arguments (16 bytes per
     * argument for complex double and maintains alignment). if */
	
	dasm_put(Dst, 462);
	

    for (i = 1,ii=0,fi=0; i <= nargs; i++) {
        lua_rawgeti(L, ct_usr, i);
        mt = (const struct ctype*) lua_touserdata(L, -1);

        if (mt->pointers || mt->is_reference || mt->type == FUNCTION_PTR_TYPE || mt->type == ENUM_TYPE) {
            lua_getuservalue(L, -1);
            num_upvals += 2;

			
			dasm_put(Dst, 474, (int)(mt), (int)(((unsigned long)(mt))>>32), (int)(lua_upvalueindex(num_upvals)), (int)(((unsigned long)(lua_upvalueindex(num_upvals)))>>32), i);
			
            if (mt->pointers || mt->is_reference) {
                dasm_put(Dst, 493);
            } else if (mt->type == FUNCTION_PTR_TYPE) {
                dasm_put(Dst, 497);
            } else if (mt->type == ENUM_TYPE) {
                dasm_put(Dst, 501);
            }

			goto longstore;

        } else {
            lua_pop(L, 1);
            dasm_put(Dst, 505, i);

            switch (mt->type) {
            case INT8_TYPE:
                dasm_put(Dst, 508);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 512);
                } else {
                    dasm_put(Dst, 514);
                }
				goto intstore;

            case INT16_TYPE:
                dasm_put(Dst, 517);
                if (mt->is_unsigned) {
                    dasm_put(Dst, 521);
                } else {
                    dasm_put(Dst, 524);
                }
				goto intstore;

            case INT32_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 527);
                } else {
                    dasm_put(Dst, 531);
                }
 				goto intstore;

            case INT64_TYPE:
                if (mt->is_unsigned) {
                    dasm_put(Dst, 535);
                } else {
                    dasm_put(Dst, 539);
                }
				
              	goto longstore;

            case DOUBLE_TYPE:
                dasm_put(Dst, 543);
                if(isReg(fi))
					dasm_put(Dst, 547);
				else	
					dasm_put(Dst, 549);
                break;

            case FLOAT_TYPE:
                dasm_put(Dst, 551);
                if(isReg(fi))
					dasm_put(Dst, 555);
				else	
					dasm_put(Dst, 557);
                break;
			case COMPLEX_DOUBLE_TYPE:
                dasm_put(Dst, 559);
				if(isReg(fi)){
					dasm_put(Dst, 563);
					if(isReg(fi)){
						dasm_put(Dst, 565);
					}else{
						dasm_put(Dst, 567);
					}
				}
				else{
					dasm_put(Dst, 569);
				}	
                break;

            case COMPLEX_FLOAT_TYPE:
                dasm_put(Dst, 571);
				// complex float is saved as two single precision registers on stack
				if(isReg(fi)){
					dasm_put(Dst, 575);
					if(isReg(fi)){
						dasm_put(Dst, 577);
					}else{
						dasm_put(Dst, 579);
					}
				}
				else{
					dasm_put(Dst, 581);
				}
                break;
            case INTPTR_TYPE:
                dasm_put(Dst, 583);
                
				
                goto longstore;
				
			intstore:
				if(isReg(ii))
					dasm_put(Dst, 587);
				else	
					dasm_put(Dst, 589);
                break;
			longstore:
				if(isReg(ii))
					dasm_put(Dst, 591);
				else	
					dasm_put(Dst, 593);
                break;
				
            default:
                luaL_error(L, "NYI: call arg type");
            }
        }
    }

    if (ct->has_var_arg) {
		if(fi<8){
			dasm_put(Dst, 595, (8-fi), nargs+1);
		}
		if(ii<8){
			dasm_put(Dst, 605, (8-ii), nargs+1);
		}
		
		//case when DATA is not allocated, all arg is skipped
		dasm_put(Dst, 615, (8-fi), (8-ii), nargs+1);
		
    }

    dasm_put(Dst, 631, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));

	//pop all args in registers
	dasm_put(Dst, 643);
	
    dasm_put(Dst, 653);



    lua_rawgeti(L, ct_usr, 0);
    mt = (const struct ctype*) lua_touserdata(L, -1);

    if (mt->pointers || mt->is_reference) {
        lua_getuservalue(L, -1);
        num_upvals += 2;
        dasm_put(Dst, 656, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32), (int)(mt), (int)(((unsigned long)(mt))>>32), (int)(lua_upvalueindex(num_upvals)), (int)(((unsigned long)(lua_upvalueindex(num_upvals)))>>32));

    } else {
        switch (mt->type) {
        case INT64_TYPE:
		#if LUA_VERSION_NUM>=503
			 lua_pop(L, 1);
            dasm_put(Dst, 697, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
		#else
            num_upvals++;
            dasm_put(Dst, 722, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32), (int)(mt), (int)(((unsigned long)(mt))>>32));
		#endif
            break;

        case INTPTR_TYPE:
            num_upvals++;
            dasm_put(Dst, 759, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32), (int)(mt), (int)(((unsigned long)(mt))>>32));
            break;

        case VOID_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 796, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
            break;

        case BOOL_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 816, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
            break;

        case INT8_TYPE:
        case INT16_TYPE:
        case INT32_TYPE:
        case ENUM_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 841, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
            if (mt->is_unsigned) {
                dasm_put(Dst, 855);
            } else {
                dasm_put(Dst, 859);
            }
            dasm_put(Dst, 863);
            break;

        case FLOAT_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 872, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
            break;

        case DOUBLE_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 897, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
            break;
		case COMPLEX_FLOAT_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 922, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
            break;

        case COMPLEX_DOUBLE_TYPE:
            lua_pop(L, 1);
            dasm_put(Dst, 949, (int)(&Dst->last_errno), (int)(((unsigned long)(&Dst->last_errno))>>32));
            break;
        default:
            luaL_error(L, "NYI: call return type");
        }
    }

    assert(lua_gettop(L) == top + num_upvals);
    lua_pushcclosure(L, (lua_CFunction) compile(Dst, L, func, LUA_NOREF), num_upvals);
}

