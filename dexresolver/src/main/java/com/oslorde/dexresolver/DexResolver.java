package com.oslorde.dexresolver;

import android.os.Build;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexFile;
import dalvik.system.PathClassLoader;

public class DexResolver {


    static {
        System.loadLibrary("dexresolver");
    }

    private static Field sDexCookie;
    private static Dex[] sBootDexFiles;
    private static int sInstanceCount;

    private static native String[][] getBootClassList();

    private static native String[][] getClassList(Object cookie);

    private List<Dex> dexes;
    private Set<String> dexFileNames;

    private List<BaseDexClassLoader> oneFileLoaders;

    public DexResolver(){
        sInstanceCount++;
    }


    public Dex[] getBootDexFiles() throws Exception{
        Dex[] bootDexFiles=sBootDexFiles;
        if(bootDexFiles!=null)
            return bootDexFiles;
        String[] bootJars = System.getenv("BOOTCLASSPATH").split(":");
        ArrayList<Dex> dexFiles=new ArrayList<>(bootJars.length);
        ByteBuffer buffer=null;
        ByteBuffer tmp=null;
        if(Build.VERSION.SDK_INT<21){
            for (String path:bootJars){
                File odexSrc=new File(path.replaceAll("\\..*",".odex"));
                if(!odexSrc.exists())
                    odexSrc=new File("/data/dalvik-cache/"+path.replace('/','@')+"@classes.dex");
                if(odexSrc.exists()){
                    FileInputStream in=new FileInputStream(odexSrc);
                    FileChannel channel=in.getChannel();
                    MappedByteBuffer byteBuffer=channel.map(FileChannel.MapMode.READ_ONLY,0,channel.size());
                    String[][] list=getClassList(byteBuffer);
                    for (String[] dex:list) dexFiles.add( new Dex(dex,null,null));
                    channel.close();
                    in.close();
                }else if(path.matches(".+(\\.jar|\\.zip|\\.apk)$")){
                    ZipFile zipFile;
                    ZipEntry entry;
                    try{
                        zipFile = new ZipFile(path);
                        entry = zipFile.getEntry("classes.dex");
                        if(entry==null){
                            zipFile.close();
                            continue;
                        }
                    }catch (Exception e){//will it be a bad file?
                        continue;
                    }

                    InputStream stream =zipFile.getInputStream(entry);
                    if(tmp==null){
                        tmp=ByteBuffer.allocate(1024);
                        tmp.order(ByteOrder.LITTLE_ENDIAN);
                    }
                    int size = (int)entry.getSize();
                    if(buffer==null||buffer.capacity()<size){
                        buffer=ByteBuffer.allocateDirect(size);
                    }
                    int read;
                    while ((read=stream.read(tmp.array()))>0){
                        tmp.position(0);
                        tmp.limit(read);
                        buffer.put(tmp);
                    }
                    buffer.position(0);
                    String[][] list=getClassList(buffer);
                    if(list!=null) for (String[] dex:list) dexFiles.add( new Dex(dex,null,null));
                    stream.close();
                    zipFile.close();
                }
            }
        }
        else if (Build.VERSION.SDK_INT <23) {
            for (String path : bootJars) {
                File file = new File(path);
                if(!file.exists()) continue;
                try {
                    DexFile dexFile = new DexFile(file);
                    addDexFile(dexFiles,dexFile,null);
                }catch (Exception ignored){
                }
            }
        } else {
            String[][] bootClassList = getBootClassList();
            if (bootClassList != null) {
                for (String[] dex:bootClassList) dexFiles.add( new Dex(dex,null,null));
            }
        }

        bootDexFiles = dexFiles.toArray(new Dex[0]);
        sBootDexFiles=bootDexFiles;
        return bootDexFiles;
    }

    private static Object getDexFileCookie(DexFile dexFile){
        try {
            if(sDexCookie ==null){
                sDexCookie =dexFile.getClass().getDeclaredField("mCookie");
                sDexCookie.setAccessible(true);
            }
            return sDexCookie.get(dexFile);
        }catch (Exception ignored){

        }
        return null;
    }

    public List<String> getBootClasses()  {
        ArrayList<String> ret=new ArrayList<>();
        try {
            for (Dex dex: getBootDexFiles()){
                Collections.addAll(ret,dex.listClasses());
            }
        }catch (Exception ignored){}

        return ret;
    }

    public Class findClass(String name) throws Exception {
        Class ret;
        for (Dex dex:getBootDexFiles()){
            if((ret=dex.findClass(name))!=null) return ret;
        }
        if(dexes!=null){
            Iterator<Dex> iterator=dexes.iterator();
            while (iterator.hasNext()){
                Dex dex=iterator.next();
                if(dex.isDead()){
                    iterator.remove();
                    continue;
                }
                if((ret=dex.findClass(name))!=null) return ret;
            }
        }
        return null;
    }

    public Set<String> getPackages() throws Exception {
        Set<String> packs=new HashSet<>();
        for (Dex dex:getBootDexFiles()){
            dex.listPackages(packs);
        }
        if(dexes!=null){
            Iterator<Dex> iterator=dexes.iterator();
            while (iterator.hasNext()){
                Dex dex=iterator.next();
                if(dex.isDead()){
                    iterator.remove();
                    continue;
                }
                dex.listPackages(packs);
            }
        }
        return packs;
    }


    public  void listPackage(String pack,  Dex.ClassCallback callback) throws Exception{
        listPackage(pack,false,false,callback);
    }

    public  void listPackage(String pack, boolean includeInner, boolean recursive, Dex.ClassCallback callback) throws Exception {
        pack=Dex.fixPackageName(pack,includeInner);
        for (Dex dex:getBootDexFiles()){
            dex.listPackage(pack,includeInner,recursive,callback);
        }
        if(dexes==null) return;
        Iterator<Dex> iterator=dexes.iterator();
        while (iterator.hasNext()){
            Dex dex=iterator.next();
            if(dex.isDead()){
                iterator.remove();
                continue;
            }
            dex.listPackage(pack,includeInner,recursive,callback);
        }
    }

    public Dex[] addFile(File file) throws Exception{
        if(oneFileLoaders ==null) oneFileLoaders =new ArrayList<>();
        BaseDexClassLoader loader=new PathClassLoader(file.getPath(),DexResolver.class.getClassLoader());
        int beforeSize=dexes.size();
        loadClassLoader(loader);

        if(dexes.size()==beforeSize){
            throw new IOException("No dex file found in path:"+file);
        }
        oneFileLoaders.add(loader);
        return dexes.subList(beforeSize,dexes.size()).toArray(new Dex[0]);
    }

    public void loadClassLoader(ClassLoader loader) throws Exception {
        Field fPathList = BaseDexClassLoader.class.getDeclaredField("pathList");
        Field fDexElements=null;
        Field fDexFile=null;
        while (loader!=null){
            if(loader instanceof BaseDexClassLoader){
                fPathList.setAccessible(true);
                Object pathList= fPathList.get(loader);
                if(pathList==null) continue;
                if(fDexElements==null){
                    fDexElements = pathList.getClass().getDeclaredField("dexElements");
                    fDexElements.setAccessible(true);
                }
                Object[] elements= (Object[]) fDexElements.get(pathList);
                if(elements==null) continue;
                for (Object ele : elements) {
                    if(fDexFile==null){
                        fDexFile = ele.getClass().getDeclaredField("dexFile");
                        fDexFile.setAccessible(true);
                    }
                    DexFile dexFile= (DexFile) fDexFile.get(ele);
                    if(dexFile==null) continue;
                    addDexFile(dexFile,loader);

                }
            }
            loader=loader.getParent();
        }

    }


    public void addDexFile(DexFile dexFile, ClassLoader loader)  throws Exception{
        if(dexes ==null){
            dexes =new ArrayList<>();
            dexFileNames=new HashSet<>(1);// We don't need to add any boot jars
        }
        if(dexFileNames.contains(dexFile.getName())) return;
        addDexFile(dexes,dexFile,loader);
    }

    public void remove(Dex[] dexes){
        for (Dex dex:dexes){
            this.dexes.remove(dex);
            oneFileLoaders.remove((BaseDexClassLoader) dex.getClassLoader());
        }
    }

    //Classes in dex file are sorted before return, so no need to sort it
    private void addDexFile(List<Dex> dexFiles, DexFile dexFile, ClassLoader loader) throws Exception{
        if(Build.VERSION.SDK_INT>=21){
            String[][] list=getClassList(getDexFileCookie(dexFile));
            for (String[] dex:list) dexFiles.add(new Dex(dex,loader==null?null:dexFile,loader));
        }else {
            Enumeration<String> entries = dexFile.entries();
            Field field=entries.getClass().getDeclaredField("mNameList");
            field.setAccessible(true);
            String[] list= (String[]) field.get(entries);
            dexFiles.add(new Dex(list,dexFile,loader));
        }
    }

    @Override
    protected  void finalize() throws Throwable {
        super.finalize();
        if(--sInstanceCount ==0){
            sBootDexFiles=null;
        }
    }
}
