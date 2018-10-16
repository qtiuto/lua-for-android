package com.oslorde.luadroid;

/**
 * General exception type for lua code
 */
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
