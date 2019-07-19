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
    private int isDir=-1;

    public DexClass(Dex dex, String className) {
        super(dex.getName()+"!/"+className.replace('.','/'));
        this.dex=dex;
        this.className=className;
    }

    private DexClass(Dex dex, String className,boolean isDir) {
        this(dex,className);
        this.isDir=isDir?0:1;
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
        return new DexClass(dex,parent,true);
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
        if(isDir==-1) isDir=dex.isDirectory(className,true)?1:0;
        return isDir!=0;
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
