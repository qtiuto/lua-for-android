package com.sun.tools.javac.file;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;

public class FileUtils {
    public static File resolve(File f,String o){
        if(o.charAt(0)=='/') return new File(o);
        if(f instanceof FakeFile) return ((FakeFile) f).resolve(o);
        return new File(f,o).getAbsoluteFile();
    }

    private static File resolveSibling(File first,File other){
        String parent=first.getParent();
        return parent==null?other:new File(parent,other.getPath()).getAbsoluteFile();
    }

    public static File resolveSibling(File first,String other){
        if(other.charAt(0)=='/') return new File(other);
        if(first instanceof FakeFile) return ((FakeFile) first).resolve(other);
       return resolveSibling(first,new File(other));
    }

    public static String relativize(File f,File o){
        String first=f.getPath();
        String other=o.getPath();
        if(first.equals(other)) return null;
        if(other.startsWith(first)) {
            char c=other.charAt(first.length());
            if(c=='!'||c=='/'){
                return other.substring(first.length()+1);
            }
        }
        return null;
    }
    public static File get(URI uri){
        return new File(uri.getPath());
    }

    public static File get(String first, String... more){
        File file=new File(first);
        for (String s:more){
            file=resolve(file,s);
        }
        return file;
    }

    public static boolean startsWith(File f,File s){
        return f.getClass()==s.getClass()&&(f.getPath().equals(s.getPath())||f.getParentFile().getPath().startsWith(s.getPath()));
    }

    public static InputStream newInputStream(File f) throws IOException{
        if(f instanceof FakeFile) return ((FakeFile) f).openInput();
        return new FileInputStream(f);
    }
}
