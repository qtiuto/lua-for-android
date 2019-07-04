package com.sun.tools.javac.file;



import com.oslorde.dexresolver.Dex;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public class DexClass extends File implements FakeFile {
    private static final DexClass[] EMPTY_STRING_ARRAY = new DexClass[0];
    private Dex dex;
    private String className;

    public DexClass(Dex dex, String className) {
        super(dex.getName()+"!/"+className.replace('.','/'));
        this.dex=dex;
        this.className=className;
    }

    public String getClassName(){
        return className;
    }

    public Class loadClass(){
        try {
            return dex.loadClass(className);
        } catch (ClassNotFoundException e) {
            return null;
        }
    }

    @Override
    public File resolve(String path) {
        return new DexClass(dex,className+'.'+path);
    }

    @Override
    public File getParentFile() {
        int endIndex = className.lastIndexOf('.');
        if(endIndex<0) return new File(dex.getName());
        String parent=className.substring(0, endIndex);
        return new DexClass(dex,parent);
    }

    @Override
    public InputStream openInput() throws IOException {
        throw new IOException("cannot be opened");
    }

    @Override
    public String getParent() {
        throw new UnsupportedOperationException();
    }

    private static String simpleName(String cls){
        return cls.substring(cls.lastIndexOf('.')+1);
    }

    @Override
    public File[] listFiles() {
        ArrayList<DexClass> entries=new ArrayList<>();
        dex.listPackage(className,true,false,(dex,name)->{
            entries.add(new DexClass(dex,name));
        });
        return entries.toArray(EMPTY_STRING_ARRAY);
    }

    @Override
    public boolean exists() {
        return dex.hasClass(className);
    }


    @Override
    public boolean isFile() {
        return exists();
    }

    @Override
    public boolean isDirectory() {
        return dex.hasPackage(className);
    }

    @Override
    public long lastModified() {
        return 0;
    }

    @Override
    public boolean delete() {
        return false;
    }
}
