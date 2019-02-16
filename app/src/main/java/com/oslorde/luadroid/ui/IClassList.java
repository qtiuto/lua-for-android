package com.oslorde.luadroid.ui;

import java.util.List;

public interface IClassList  {
    List<String> findClassWithPrefix(String prefix);
    JClass resolveClass(String c) throws Exception;
}
