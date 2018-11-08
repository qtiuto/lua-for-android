package com.oslorde.luadroid;

import java.util.*;

public class ClassList {
   static public class ClassEntry implements Comparable<ClassEntry>{
       String name;
       List<String> classes;

       ClassEntry(String name){
           this.name=name;
       }

       @Override
       public int compareTo(ClassEntry o) {
           return name.compareTo(o.name);
       }

       @Override
       public boolean equals( Object obj) {
           if(obj instanceof ClassEntry)
               return ((ClassEntry) obj).name.equals(name);
           return false;
       }

       @Override
       public int hashCode() {
           return name.hashCode();
       }
   }
   static private Comparator<ClassEntry> sClassEntryComparator= (o1, o2) -> {
       if(o1.name.startsWith(o2.name)) return 0;
       return o1.compareTo(o2);
   };

   private static ClassEntry tmp=new ClassEntry(null);

   static private String getSimpleName(String name){
       int idx=name.lastIndexOf('.');
       if(idx==-1) return name;
       return name.substring(idx+1).intern();
   }

   private ClassEntry[] entries;
   public ClassList(ScriptContext context){
       List<String> classes=context.getClasses();
       Map<ClassEntry,ClassEntry> entries=new HashMap<>();
       for (String cl:classes){
           String name=getSimpleName(cl);
           tmp.name=name;
           ClassEntry existed=entries.get(tmp);
           if(existed==null){
               existed=new ClassEntry(name);
               existed.classes=new ArrayList<>(1);
               existed.classes.add(cl);
               entries.put(existed,existed);
           }else {
               existed.classes.add(cl);
           }
       }
       this.entries=entries.values().toArray(new ClassEntry[0]);
       Arrays.sort(this.entries);
       for (ClassEntry entry : this.entries) {
           if(entry.classes.size()>1){
               entry.classes=new ArrayList<>(new LinkedHashSet<>(entry.classes));
           }
       }
   }

    public synchronized List<String>  findClassWithPrefix(String prefix){
        tmp.name=prefix;
        int index=Arrays.binarySearch(entries,tmp,sClassEntryComparator);
        if(index<0) return Collections.emptyList();
        int start;
        for (start=index;start>=0;--start){
            if(!entries[start].name.startsWith(prefix)){
                break;
            }
        }
        ++start;
        int end;
        int len=entries.length;
        for (end=index+1;end<len;++end){
            if(!entries[end].name.startsWith(prefix)){
                break;
            }
        }
        ArrayList<String> ret=new ArrayList<>();
        for (int i = start; i <end ; i++) {
            ret.addAll(entries[i].classes);
        }
        return ret;
    }

}
