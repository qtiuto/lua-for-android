package com.oslorde.luadroid.ui;

import java.util.List;

public class JMember implements TransmitObj{
    private int type;
    private String des;
    private String name;

    private JMember(){}

    public JMember(int type, String des, String name) {
        this.type = type;
        this.des = des;
        this.name = name;
    }

    public boolean isConstructor(){
        return type==0;
    }
    public boolean isMethod(){
        return type==1;
    }
    public boolean isField(){
        return type==2;
    }

    public String getDes() {
        return des;
    }

    public String getName() {
        return name;
    }

    @Override
    public List<Object> save() {
        return null;
    }

    @Override
    public void restoreFrom(List<Object> objects) {

    }
}
