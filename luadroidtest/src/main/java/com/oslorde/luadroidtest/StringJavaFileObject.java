package com.oslorde.luadroidtest;

import java.io.IOException;
import java.net.URI;

import javax.tools.SimpleJavaFileObject;

public class StringJavaFileObject extends SimpleJavaFileObject
{
    private String src;
    /**
     * Construct a SimpleJavaFileObject of the given kind and with the
     * given URI.
     *
     * @param path  the URI for this file object
     * @param src the src string of this file object
     */
    public StringJavaFileObject(String path,String src) {
        super(URI.create(path), Kind.SOURCE);
        this.src=src;
    }

    @Override
    public CharSequence getCharContent(boolean ignoreEncodingErrors) throws IOException {
        return src;
    }
}
