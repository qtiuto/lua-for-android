package com.oslorde.luadroid;

import java.lang.reflect.Array;
import java.util.*;
import java.util.ListIterator;

class DataMap<K,V> implements Map<K,V> ,Iterable<DataMap.CustomEntry<K,V>>{
    private ArrayList<CustomEntry<K,V>> entries;

    //DataMap(){entries=new ArrayList<>();}
    DataMap(int len){
        entries=new ArrayList<>(len);
    }
    @Override
    public int size() {
        return entries.size();
    }

    @Override
    public boolean isEmpty() {
        return entries.isEmpty();
    }

    private static boolean Equals(Object f,Object s){
        if(f==null) return s==null;
        return f.equals(s);
    }

    @Override
    public boolean containsKey( Object key) {
      return indexOf(key)!=-1;
    }

    @Override
    public boolean containsValue( Object value) {
        for(int i=entries.size()-1;i>=0;--i){
            if(Equals(entries.get(i).getValue(),value))
                return true;
        }
        return false;
    }

    private int indexOf(Object key){
        for(int i=entries.size()-1;i>=0;--i){
            CustomEntry<K,V> entry = entries.get(i);
            if(Equals(entry.getKey(),key))
                return i;
        }
        return -1;
    }
    
    @Override
    public V get( Object key) {
        for(int i=entries.size()-1;i>=0;--i){
            CustomEntry<K,V> entry = entries.get(i);
            if(Equals(entry.getKey(),key))
                return entry.getValue();
        }
        return null;
    }

    
    @Override
    public V put( K key,  V value) {
       entries.add(new CustomEntry<>(key,value));
        return null;//never check for duplicate value
    }

    
    @Override
    public V remove( Object key) {
        int index=indexOf(key);
        return entries.remove(index).value;
    }

    @Override
    public void putAll( Map<? extends K, ? extends V> m) {
        for(Entry<? extends K, ? extends V> entry:m.entrySet()){
            entries.add(new CustomEntry<>(entry.getKey(),entry.getValue()));
        }
    }

    @Override
    public void clear() {
       entries.clear();
    }

    
    @Override
    public Set<K> keySet() {
        return new Set<K>() {
            @Override
            public int size() {
                return entries.size();
            }

            @Override
            public boolean isEmpty() {
                return entries.isEmpty();
            }

            @Override
            public boolean contains( Object o) {
                return containsKey(o);
            }

            
            @Override
            public Iterator<K> iterator() {
                Iterator<CustomEntry<K,V>> iterator=entries.iterator();
                return new Iterator<K>() {
                    @Override
                    public boolean hasNext() {
                        return iterator.hasNext();
                    }

                    @Override
                    public K next() {
                        return iterator.next().getKey();
                    }
                };
            }

            
            @Override
            public Object[] toArray() {
                Object[] ret=new Object[size()];
                for (int i=0,len=size();i<len;++i){
                    ret[i]=entries.get(i).getKey();
                }
                return ret;
            }

            @Override
            public <T> T[] toArray( T[] a) {
                T[] ret=a.length<size()?(T[])Array.newInstance(a.getClass().getComponentType(),size()) :a;
                for (int i=0,len=size();i<len;++i){
                    ret[i]=(T)entries.get(i).getKey();
                }
                return ret;
            }

            @Override
            public boolean add(K k) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean remove( Object o) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean containsAll( Collection<?> c) {
                for (Object v:c){
                    if(!containsKey(v))
                        return false;
                }
                return true;
            }

            @Override
            public boolean addAll( Collection<? extends K> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean retainAll( Collection<?> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean removeAll( Collection<?> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public void clear() {
                throw new UnsupportedOperationException();
            }
        };
    }

    
    @Override
    public Collection<V> values() {
        return new Collection<V>() {
            @Override
            public int size() {
                return entries.size();
            }

            @Override
            public boolean isEmpty() {
                return entries.isEmpty();
            }

            @Override
            public boolean contains( Object o) {
                return containsKey(o);
            }


            @Override
            public Iterator<V> iterator() {
                Iterator<CustomEntry<K,V>> iterator=entries.iterator();
                return new Iterator<V>() {
                    @Override
                    public boolean hasNext() {
                        return iterator.hasNext();
                    }

                    @Override
                    public V next() {
                        return iterator.next().getValue();
                    }
                };
            }


            @Override
            public Object[] toArray() {
                Object[] ret=new Object[size()];
                for (int i=0,len=size();i<len;++i){
                    ret[i]=entries.get(i).getValue();
                }
                return ret;
            }

            @Override
            public <T> T[] toArray( T[] a) {
                T[] ret=a.length<size()?(T[])Array.newInstance(a.getClass().getComponentType(),size()) :a;
                for (int i=0,len=size();i<len;++i){
                    ret[i]=(T)entries.get(i).getValue();
                }
                return ret;
            }

            @Override
            public boolean add(V k) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean remove( Object o) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean containsAll( Collection<?> c) {
                for (Object v:c){
                    if(!containsKey(v))
                        return false;
                }
                return true;
            }

            @Override
            public boolean addAll( Collection<? extends V> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean retainAll( Collection<?> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean removeAll( Collection<?> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public void clear() {
                throw new UnsupportedOperationException();
            }
        };
    }


    @Override
    public ListIterator<CustomEntry<K,V>> iterator() {
        return entries.listIterator();
    }

    static class CustomEntry<T,P> implements Map.Entry<T,P>{
        T key;
        P value;
        CustomEntry(T k,P v){
            key=k;
            value=v;
        }
        @Override
        public T getKey() {
            return key;
        }

        @Override
        public P getValue() {
            return value;
        }

        public void setKey(T key) {
            this.key = key;
        }

        @Override
        public P setValue(P value) {
            P old=this.value;
            this.value = value;
            return old;
        }
    }
    
    @Override
    public Set<Entry<K, V>> entrySet() {

        return new Set<Entry<K, V>>() {
            @Override
            public int size() {
                return entries.size();
            }

            @Override
            public boolean isEmpty() {
                return size()>0;
            }

            @Override
            public boolean contains( Object o) {
                return entries.contains(o);
            }

            
            @Override
            public Iterator<Entry<K, V>> iterator() {
                Iterator<CustomEntry<K,V>> iterator=entries.iterator();
                return new Iterator<Entry<K, V>>() {
                    @Override
                    public boolean hasNext() {
                        return iterator.hasNext();
                    }

                    @Override
                    public Entry<K, V> next() {
                        return iterator.next();
                    }
                };
            }

            
            @Override
            public Object[] toArray() {
                return entries.toArray();
            }

            @Override
            public <T> T[] toArray( T[] a) {
                return  entries.toArray(a);
            }

            @Override
            public boolean add(Entry<K, V> kvEntry) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean remove( Object o) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean containsAll( Collection<?> c) {
                return entries.contains(c);
            }

            @Override
            public boolean addAll( Collection<? extends Entry<K, V>> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean retainAll( Collection<?> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public boolean removeAll( Collection<?> c) {
                throw new UnsupportedOperationException();
            }

            @Override
            public void clear() {
                throw new UnsupportedOperationException();
            }
        };
    }


}
