package com.oslorde.luadroid;

/**
 * Special iterator for list-like object
 */
public interface ListIterator extends MapIterator {
    @Override
    default Object[] nextEntry()throws Throwable{
        return new Object[]{next()};
    }

    /**
     * @return the next value of the iterator
     * @throws Throwable any
     */
    Object next()throws Throwable;
}
