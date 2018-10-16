package com.oslorde.luadroid;

/**
 * To index a list or map
 * @param <T> supported type
 */
public interface Indexer<T> {
    /**
     *To test whether the key is allowed by the list or map.
     * @param key the key
     * @return true to support and false not
     */
    default boolean supportsKey(Object key){
        return true;
    }

    /**
     * Get the size of the list or map.
     * @param obj the list or map
     * @return the size
     * @throws Exception
     */
    int size(T obj) throws Exception;

    /**
     *Get the value for the list for map.
     * @param obj the list or map
     * @param key the key
     * @return the value
     * @throws Throwable any
     */
    Object at(T obj,Object key) throws Throwable;

    /**
     *Set  the list for map at the key with the value.
     * @param obj the list or map
     * @param key the key
     * @param v the value
     * @throws Throwable any
     */
    void set(T obj,Object key,Object v) throws Throwable;
}
