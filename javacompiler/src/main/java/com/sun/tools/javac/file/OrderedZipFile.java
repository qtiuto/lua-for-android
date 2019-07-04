package com.sun.tools.javac.file;

import android.os.Build;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class OrderedZipFile extends File implements FakeFile{

    ZipFile innerZip;
    HashMap<String,ZipEntryFile> entryMap;
    Set<EntryFile> rootEntries=new HashSet<>();


    public OrderedZipFile(String path) throws IOException {
        super(path);
        innerZip =new ZipFile(path);
        HashMap<String,ZipEntryFile> entryMap=new HashMap<>();
        this.entryMap=entryMap;
        Enumeration<? extends ZipEntry> entries= innerZip.entries();
        while (entries.hasMoreElements()){
            ZipEntry next=entries.nextElement();
            if(next.isDirectory()) continue;
            ZipEntryFile value = new ZipEntryFile(this, next);
            entryMap.put(next.getName(), value);
            fillParentEntries(value,entryMap);
        }
    }

    private void fillParentEntries(ZipEntryFile entryFile, Map<String,ZipEntryFile> entryMap){
        String pName=entryFile.getParentEntryName();
        if(pName.isEmpty()){
            entryFile.parent=this;
            rootEntries.add(entryFile);
        }
        else {
            ZipEntryFile existed=entryMap.get(pName);
            if(existed==null){
                existed=new ZipEntryFile(this,pName);
                entryMap.put(pName,existed);
            }
            existed.addEntry(entryFile);
            fillParentEntries(existed,entryMap);
        }

    }

    @Override
    public File[] listFiles() {
        return rootEntries.toArray(EntryFile.EMPTY_FILE_ARRAY);
    }

    public  Collection<? extends EntryFile> getEntries(){
        return entryMap.values();
    }

    EntryFile getEntryFile(String name){
        EntryFile ret= entryMap.get(name);
        if(ret==null) ret=new InvalidEntryFile(this,name);
        return ret;
    }

    @Override
    protected void finalize() throws Throwable {
        innerZip.close();
    }

    public void close() throws IOException{
        innerZip.close();

    }

    @Override
    public File resolve(String path) {
        if(path.isEmpty()) return this;
        return getEntryFile(path);
    }

    @Override
    public InputStream openInput() throws IOException{
        return new FileInputStream(getPath());
    }

    public static abstract class EntryFile extends File implements FakeFile{

        public static final File[] EMPTY_FILE_ARRAY = new File[0];
        protected OrderedZipFile zipFile;
        protected String entryName;
        public EntryFile(OrderedZipFile zipFile,String entryName) {
            super(zipFile.getName()+"!/"+entryName);
            this.entryName=entryName;
        }

        @Override
        public File resolve(String path) {
            return zipFile.resolve(entryName+"/"+path);
        }

        @Override
        public File[] listFiles() {
            return EMPTY_FILE_ARRAY;
        }

        @Override
        public boolean exists() {
            return false;
        }

        @Override
        public File getParentFile() {
            return zipFile.resolve(getParentEntryName());
        }

        @Override
        public String getParent() {
            throw new UnsupportedOperationException();
        }

        String getParentEntryName(){
            return entryName.substring(0,entryName.lastIndexOf('/')+1);
        }

        public String getEntryName(){return entryName;}

        @Override
        public boolean delete() {
            return false;
        }

        @Override
        public long lastModified() {
            return 0;
        }

        @Override
        public int hashCode() {
            return entryName.hashCode();
        }

        @Override
        public boolean equals(Object obj) {
            if(obj==null||obj.getClass()!=getClass()) return false;
            return entryName.equals(((EntryFile)obj).entryName);
        }
    }
    static class InvalidEntryFile extends EntryFile {
        InvalidEntryFile(OrderedZipFile zipFile,String entryName){
            super(zipFile,entryName);
        }

        @Override
        public boolean exists() {
            return false;
        }

        @Override
        public InputStream openInput() throws IOException {
            throw new IOException("invalid file");
        }
    }
    static class ZipEntryFile extends EntryFile{
        List<ZipEntryFile> subEntries;
        File parent;
        ZipEntry entry;
        public ZipEntryFile(OrderedZipFile zipFile,String entryName) {
            super(zipFile,entryName);
        }
        public ZipEntryFile(OrderedZipFile zipFile,ZipEntry entry) {
            super(zipFile,entry.getName());
            this.entry=entry;
        }

        void addEntry(ZipEntryFile entry){
            if(subEntries==null) subEntries=new ArrayList<>();
            subEntries.add(entry);
        }

        @Override
        public File[] listFiles() {
            if(subEntries==null) return EMPTY_FILE_ARRAY;
            return subEntries.toArray(EMPTY_FILE_ARRAY);
        }

        @Override
        public File getParentFile() {
            return parent;
        }

        @Override
        public boolean exists() {
            return true;
        }

        @Override
        public boolean isDirectory() {
            return entry==null;
        }

        @Override
        public boolean isFile() {
            return entry!=null;
        }

        @Override
        public InputStream openInput() throws IOException {
            if(isDirectory()) throw new IOException("is directory");
            return zipFile.innerZip.getInputStream(entry);
        }

        @Override
        public long lastModified() {
            if(Build.VERSION.SDK_INT>=26)
            return entry.getLastModifiedTime().toMillis();
            else return 0;
        }
    }
}
