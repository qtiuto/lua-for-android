package com.oslorde.luadroid.ui;

import com.oslorde.luadroid.Logger;

import java.io.File;

public interface IScriptContext {
    String run(String s);
    String run(File file);
    void setLogger(Logger out, Logger err);
    default void addToLua(String name, Object obj){
         addToLua(name,obj,false);
    }
    void addToLua(String name, Object obj, boolean local);
    default void addToLua(String luaName, String memberName, Object instOrType){
         addToLua(luaName,memberName,instOrType,false);
    }
    default void addToLua(String luaName, String memberName, Object instOrType, boolean local){
        if(instOrType instanceof Class)
             addToLua(luaName,memberName,null,(Class) instOrType,local);
        else addToLua(luaName,memberName,instOrType,null,local);
    }
    void addToLua(String luaName, String memberName, Object inst, Class type, boolean local);

    IClassList getClasses();

    void flushLog();
}
