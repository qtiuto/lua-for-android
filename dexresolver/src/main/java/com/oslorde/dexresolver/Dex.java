package com.oslorde.dexresolver;

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Set;

import dalvik.system.DexFile;

public class Dex {

    public interface ClassCallback {
         void onClass(Dex dex, String className) throws Exception;
    }

    private String[] classes;
    private WeakReference<DexFile> dexFile;
    private WeakReference<ClassLoader> classLoader;
    private static final Comparator<String> CLASS_COMPARATOR= Dex::classCompare;
    private static final Comparator<String> PACK_COMPARATOR=(cl, pack) -> startsWithPackage(cl,pack)?0:classCompare(cl,pack);
    private static final Comparator<String> PACK_COMPARATOR_INCLUDE_INNER=(cl,pack)->startsWithPackageIncludeInner(cl,pack)?0:classCompare(cl,pack);

    Dex(String[] classes){
        this.classes=classes;
    }

    Dex(String[] classes, DexFile dexFile, ClassLoader loader){
        this.classes=classes;
        this.dexFile=dexFile==null?null:new WeakReference<>(dexFile);
        this.classLoader=loader==null?null:new WeakReference<>(loader);
    }

    private static boolean startsWithPackage(String cl,String pack,boolean includeInner){
        return includeInner?startsWithPackageIncludeInner(cl,pack):startsWithPackage(cl,pack);
    }
    private static boolean startsWithPackageIncludeInner(String cl,String pack){
        /*int len=pack.length();
        if(cl.length()<len+1) return false;
        char c=cl.charAt(len);
        if(c!='.') return false;*/
        return cl.startsWith(pack);
    }
    private static boolean startsWithPackage(String cl,String pack){
        int len=pack.length();
        if(cl.length()<len) return false;
        char c,c1;
        /*if(cl.length()<len+1) return false;
        c=cl.charAt(len);
        if(c!='.'&&c!='$') return false;*/
        while (len-->0){
            c=cl.charAt(len);
            c1=pack.charAt(len);
            if(c!=c1&&!(c=='$'&&c1=='.'))
                return false;
        }
        return true;
    }

    private static  int classCompare(String orig,String other){
        int len=orig.length();
        int len2=other.length();

        char c1;
        char c2;
        for(int i=0,N=Math.min(len,len2);i<N;++i) {
            c1=orig.charAt(i);
            c2=other.charAt(i);
            int dif=c1-c2;
            if(dif!=0&&!(c1=='$'&&c2=='.'))
                return dif;
        }
        if(len==len2) return 0;
        else if(len<len2){//inner class position lower
            return other.charAt(len)=='$'?1:-1;
        } else {
            return orig.charAt(len2)=='$'?-1:1;
        }
    }

    public Class findClass(String name)  {
        int idx=Arrays.binarySearch(classes,name,CLASS_COMPARATOR);
        if(idx>=0){
            try {
                return loadClass(classes[idx]);
            }catch (Exception ignored){}
        }
        return null;
    }


    public boolean hasClass(String clazzName){
        return Arrays.binarySearch(classes,clazzName,CLASS_COMPARATOR)>=0;
    }


    public boolean isDirectory(String dir,boolean includeInner){
        dir=fixPackageName(dir,includeInner);
        int idx= Arrays.binarySearch(classes,dir,includeInner?PACK_COMPARATOR_INCLUDE_INNER:PACK_COMPARATOR);
        if(idx>=0){
            int searchBefore=idx;
            if(classes[searchBefore].equals(dir))searchBefore--;
            return startsWithPackage(classes[searchBefore],dir,includeInner)
                ||startsWithPackage(classes[idx+1],dir,includeInner);
        }
        return false;
    }

    /**
     *
     * @param pack the package to search
     * @param includeInner  include inner class in the result
     * @param recursive get all package under the pack
     * @param callback callback to run
     */

    public void listPackage(String pack, boolean includeInner, boolean recursive, ClassCallback callback){
        pack = fixPackageName(pack, includeInner);
        int fromIndex=pack.length();
        int mid=pack.length()==0?classes.length-1:Arrays.binarySearch(classes,pack,includeInner?PACK_COMPARATOR_INCLUDE_INNER:PACK_COMPARATOR);
        if(mid<0) return ;
        for (int j = mid; j-- != 0; ) {
            String cl = classes[j];
            if (startsWithPackage(cl,pack,includeInner)) {
                if (recursive||cl.indexOf('.', fromIndex) == -1&&(includeInner||cl.indexOf('$', fromIndex)==-1)){
                    try {
                        callback.onClass(this,cl);
                    }catch (Exception ignored){}

                }

            } else break;
        }
        if (mid != classes.length - 1)
            for (int j = mid , len = classes.length; j < len; ++j) {
                String cl = classes[j];
                if (startsWithPackage(cl,pack,includeInner)) {
                    if (recursive||cl.indexOf('.', fromIndex) == -1&&(includeInner||cl.indexOf('$', fromIndex)==-1))
                        try {
                            callback.onClass(this,cl);
                        }catch (Exception ignored){}
                } else break;
            }

    }

    public static String fixPackageName(String pack, boolean includeInner) {
        if(!pack.isEmpty()){
            char lastChar = pack.charAt(pack.length() - 1);
            if(lastChar != '.' && (includeInner || lastChar != '$'))
                pack=pack.concat(".");
        }
        return pack;
    }

    private static String getPackageName(String name){
        int sep=name.lastIndexOf('.');
        if(sep>=0) return name.substring(0,sep);
        else return name;
    }

    public void listPackages(Set<String> packages){
        String[] names=listClasses();
        for (String name:names){
            packages.add(getPackageName(name));
        }
    }

    public String[] listClasses(){
        return classes;
    }

    boolean isDead(){
        return classLoader!=null&&classLoader.get()==null;
    }

    public Class loadClass(String clazz) throws ClassNotFoundException{
        ClassLoader loader=classLoader==null?null:classLoader.get();
        if(loader!=null){
            return dexFile.get().loadClass(clazz,loader);
        }else return Class.forName(clazz);
    }

    public ClassLoader getClassLoader(){
        return classLoader==null?null:classLoader.get();
    }

    public String getName(){
        DexFile dex;
        return dexFile==null?null:(dex=dexFile.get())==null?null:dex.getName();
    }

}
