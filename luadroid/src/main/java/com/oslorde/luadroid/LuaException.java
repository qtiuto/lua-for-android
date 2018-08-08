package com.oslorde.luadroid;

public class LuaException extends RuntimeException {
    public LuaException() {
    }

    public LuaException(String message) {
        super(message);
    }

    public LuaException(String message, Throwable cause) {
        super(message, cause);
    }

    public LuaException(Throwable cause) {
        super(cause);
    }
}
