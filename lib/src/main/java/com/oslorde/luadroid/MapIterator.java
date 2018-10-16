package com.oslorde.luadroid;

/**
 * To iterate over a list or map
 */
public interface MapIterator {
    /**
     * @return whether has any more value
     * @throws Throwable any
     */
    boolean hasNext()throws Throwable;

    /**
     * the next entry
     * @return an array contains key and value (length 2) or only
     * value (length 1)(the key will be an automatically incremented integer)
     * @throws Throwable any
     */
    Object[] nextEntry() throws Throwable;
}
