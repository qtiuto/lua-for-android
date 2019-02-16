package com.oslorde.luadroid.ui;

import java.util.ArrayList;
import java.util.List;

public  class JClass implements TransmitObj {
    private int flag;
    private String name;
    private IClass iClass;

    public static JClass getInstance(String cl){
        try {
            return new JClass(Class.forName(cl));
        }catch (Exception e){
            return null;
        }
    }
    private JClass(){}
    public JClass(Class c) {
        this.flag = c.getModifiers();
        this.name = c.getName();
        this.iClass=new IClassImpl(c);
    }

    int getModifiers(){
        return flag;
    }
    String getName(){
        return name;
    }
    public  List<JMember> filterMembers(String constraint){
        return iClass.filterMembers(constraint);
    }

    @Override
    public List<Object> save() {
        List<Object> out=new ArrayList<>();
        out.add(flag);
        out.add(name);
        out.add(iClass);
        return out;
    }

    @Override
    public void restoreFrom(List<Object> objects) {
        flag= (int) objects.get(0);
        name= (String) objects.get(1);
        iClass= (IClass) objects.get(2);
    }
}
