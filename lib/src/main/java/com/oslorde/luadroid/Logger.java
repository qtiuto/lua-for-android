package com.oslorde.luadroid;

import java.nio.ByteBuffer;

public interface Logger {
    void onNewLog(CharSequence log,ByteBuffer raw);
}
