package com.oslorde.luadroid;

import java.lang.reflect.Type;
import java.util.Map;

/**
 * For converting a table to given type
 * @param <T> given type
 */
public interface TableConverter<T>  {
    /**
     * @return the type you expect the the sub table in the key
     * of the the table to be,null default
     */
    default Type expectedKeyTableType(){
        return null;
    }
    /**
     * @return the type you expect the the sub table in the value
     * of the the table to be,null default
     */
    default Type expectedValueTableType(){
        return null;
    }

    T convert(Map<?, ?> table) throws Throwable;
}
