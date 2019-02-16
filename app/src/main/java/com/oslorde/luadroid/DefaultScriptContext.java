package com.oslorde.luadroid;

import com.oslorde.luadroid.ui.IClassList;
import com.oslorde.luadroid.ui.IScriptContext;
import com.oslorde.luadroid.ui.RemoteObj;

import java.io.File;

public class DefaultScriptContext implements IScriptContext, RemoteObj {
    private ScriptContext context=new ScriptContext();
    private IClassList classList;
    @Override
    public String run(String s) {
        return formatResults(context.run(s));
    }

    private String formatResults( Object[] results){
        if(results==null) return null;
        StringBuilder res=new StringBuilder();
        if(results.length==1){
            res.append(results[0]);
        }else {
            for (int i = 0, len = results.length; i < len; ++i) {
                res.append(i + 1).append(". ").append(results[i]).append('\n');
            }
            res.deleteCharAt(res.length() - 1);
        }
        return res.toString();
    }

    @Override
    public String run(File file) {
        return formatResults(context.run(file));
    }

    @Override
    public void setLogger(Logger out, Logger err) {
        context.setLogger(out,err);
    }

    @Override
    public void addToLua(String name, Object obj, boolean local) {
         context.addToLua(name,obj,local);
    }

    @Override
    public void addToLua(String luaName, String memberName, Object inst, Class type, boolean local) {
        context.addToLua(luaName,memberName,inst,type,local);
    }

    @Override
    public IClassList getClasses() {
        if(classList==null)
            classList=new ClassList(context);
        return classList;
    }

    @Override
    public void flushLog() {
        context.flushLog();
    }

    @Override
    public Class getProxyType() {
        return IScriptContext.class;
    }

    ScriptContext get(){
        return context;
    }
}
