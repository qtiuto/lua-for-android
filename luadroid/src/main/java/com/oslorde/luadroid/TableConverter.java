package com.oslorde.luadroid;

import java.util.Map;

interface TableConverter<T> {
    T convert(Map<?, ?> table) throws Throwable;
}
