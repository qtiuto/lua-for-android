package com.oslorde.luadroid;

/**
 * To index a list
 * @param <T> supported type
 */
public interface ListIndexer<T> extends Indexer<T> {
    /**
     *{@inheritDoc}
     * However,only int and long is supported for this method.
     */
    @Override
    default boolean supportsKey(Object key) {
        if(key instanceof Integer)
            return true;
        return key instanceof Long && (Long) key == ((Long) key).intValue();
    }

    /**
     * {@inheritDoc}
     */
    default Object at(T obj,Object key)throws Throwable{
        return at(obj,((Number)key).intValue());
    }
    /**
     * {@inheritDoc}
     */
    default void set(T obj,Object key,Object v)throws Throwable{
        set(obj,((Number)key).intValue(),v);
    }

    /**
     * index with integer key
     * @see Indexer#at(Object, Object)
     */
    Object at(T obj,int key) throws Throwable;
    /**
     * index with integer key
     * @see Indexer#set(Object, Object, Object)
     */
    void set(T obj,int key,Object v) throws Throwable;
}
