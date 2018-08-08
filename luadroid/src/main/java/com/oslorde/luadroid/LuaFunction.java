package com.oslorde.luadroid;

/**
 * Created by Karven on 2018/2/11.
 */

public interface LuaFunction {
    Object[] invoke(Object... args);
}
