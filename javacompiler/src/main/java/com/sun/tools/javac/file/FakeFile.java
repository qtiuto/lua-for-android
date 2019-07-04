package com.sun.tools.javac.file;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

public interface FakeFile {
    File resolve(String path);
    File getParentFile();
    InputStream openInput() throws IOException;
    default File resolveSibling(String path){
        return FileUtils.resolve(getParentFile(),path);
    }
}
