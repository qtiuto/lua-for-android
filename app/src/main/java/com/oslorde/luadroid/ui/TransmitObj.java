package com.oslorde.luadroid.ui;

import java.util.List;

public interface TransmitObj {
    List<Object> save();
    void restoreFrom(List<Object> objects);
}
