package com.oslorde.luadroid;

/**
 * Mapping for lua functions.
 */
public interface LuaFunction {
    Object[] invoke(Object... args);
}
