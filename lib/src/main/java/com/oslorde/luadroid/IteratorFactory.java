package com.oslorde.luadroid;

/**
 * Generate an iterator for given type
 * @param <T> given type
 */
public interface IteratorFactory<T> {
    /**
     *
     * @param v the object to iterate
     * @return a iterator for the obj
     * @throws Throwable if can not get an iterator
     */
    MapIterator generate(T v) throws Throwable;
}
