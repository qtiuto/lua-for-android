/*
 * Copyright (c) 2009, 2017, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

package com.sun.tools.javac.file;

import com.sun.tools.javac.file.RelativePath.RelativeFile;
import com.sun.tools.javac.util.DefinedBy;
import com.sun.tools.javac.util.DefinedBy.Api;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.Reader;
import java.io.Writer;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.CharsetDecoder;

import javax.lang.model.element.Modifier;
import javax.lang.model.element.NestingKind;
import javax.tools.FileObject;
import javax.tools.JavaFileObject;

import java9.util.Objects;


/**
 *  Implementation of JavaFileObject using java.nio.file API.
 *
 *  <p>PathFileObjects are, for the most part, straightforward wrappers around
 *  immutable absolute Path objects. Different subtypes are used to provide
 *  specialized implementations of "inferBinaryName" and "getName" that capture
 *  additional information available at the time the object is created.
 *
 *  <p>In general, {@link javax.tools.JavaFileManager#isSameFile} should be used to
 *  determine whether two file objects refer to the same file on disk.
 *  PathFileObject also supports the standard {@code equals} and {@code hashCode}
 *  methods, primarily for convenience when working with collections.
 *  All of these operations delegate to the equivalent operations on the
 *  underlying Path object.
 *
 *  <p><b>This is NOT part of any supported API.
 *  If you write code that depends on this, you do so at your own risk.
 *  This code and its internal interfaces are subject to change or
 *  deletion without notice.</b>
 */
public abstract class PathFileObject implements JavaFileObject {

    protected final BaseFileManager fileManager;
    protected final File path;
    private boolean hasParents;

    /**
     * Create a PathFileObject for a file within a directory, such that the
     * binary name can be inferred from the relationship to an enclosing directory.
     *
     * The binary name is derived from {@code relativePath}.
     * The name is derived from the composition of {@code userPackageRootDir}
     * and {@code relativePath}.
     *
     * @param fileManager the file manager creating this file object
     * @param path the absolute path referred to by this file object
     * @param userPackageRootDir the path of the directory containing the
     *          root of the package hierarchy
     * @param relativePath the path of this file relative to {@code userPackageRootDir}
     */
    static PathFileObject forDirectoryPath(BaseFileManager fileManager, File path,
            File userPackageRootDir, RelativePath relativePath) {
        return new DirectoryFileObject(fileManager, path, userPackageRootDir, relativePath);
    }

    private static class DirectoryFileObject extends PathFileObject {
        private final File userPackageRootDir;
        private final RelativePath relativePath;

        private DirectoryFileObject(BaseFileManager fileManager, File path,
                File userPackageRootDir, RelativePath relativePath) {
            super(fileManager, path);
            this.userPackageRootDir = Objects.requireNonNull(userPackageRootDir);
            this.relativePath = relativePath;
        }

        @Override @DefinedBy(Api.COMPILER)
        public String getName() {
            return relativePath.resolveAgainst(userPackageRootDir).toString();
        }

        @Override
        public String inferBinaryName(Iterable<? extends File> paths) {
            return toBinaryName(relativePath);
        }

        @Override
        public String toString() {
            return "DirectoryFileObject[" + userPackageRootDir + ":" + relativePath.path + "]";
        }

        @Override
        PathFileObject getSibling(String baseName) {
            return new DirectoryFileObject(fileManager,
                    FileUtils.resolveSibling(path,baseName),
                    userPackageRootDir,
                    new RelativeFile(relativePath.dirname(), baseName)
            );
        }
    }

    /**
     * Create a PathFileObject for a file in a file system such as a jar file,
     * such that the binary name can be inferred from its position within the
     * file system.
     *
     * The binary name is derived from {@code path}.
     * The name is derived from the composition of {@code userJarPath}
     * and {@code path}.
     *
     * @param fileManager the file manager creating this file object
     * @param path the path referred to by this file object
     * @param userJarPath the path of the jar file containing the file system.
     * @return the file object
     */
    public static PathFileObject forJarPath(BaseFileManager fileManager,
            File path, File userJarPath, RelativePath relativePath) {
        return new JarFileObject(fileManager, path, userJarPath,relativePath);
    }

    private static class JarFileObject extends PathFileObject {
        private final File userJarPath;
        private final RelativePath relativePath;

        private JarFileObject(BaseFileManager fileManager, File path, File userJarPath,RelativePath relativePath) {
            super(fileManager, path);
            this.userJarPath = userJarPath;
            this.relativePath=relativePath;
        }

        @Override @DefinedBy(Api.COMPILER)
        public String getName() {
            // The use of ( ) to delimit the entry name is not ideal
            // but it does match earlier behavior
            return userJarPath + "(" + relativePath.path + ")";
        }

        @Override
        public String inferBinaryName(Iterable<? extends File> paths) {
            //root always is /
            return toBinaryName(relativePath);
        }

        @Override @DefinedBy(Api.COMPILER)
        public URI toUri() {
            // Work around bug JDK-8134451:
            // path.toUri() returns double-encoded URIs, that cannot be opened by URLConnection
            return createJarUri(userJarPath, relativePath.path);
        }

        @Override
        public InputStream openInputStream() throws IOException {
            return FileUtils.newInputStream(path);
        }

        @Override
        public OutputStream openOutputStream() throws IOException {
           throw new UnsupportedOperationException();
        }

        @Override
        public String toString() {
            return "JarFileObject[" + userJarPath + ":" + path + "]";
        }

        @Override
        PathFileObject getSibling(String baseName) {
            return new JarFileObject(fileManager,
                    FileUtils.resolveSibling(path,baseName),
                    userJarPath,new RelativeFile(relativePath.dirname(),baseName)
            );
        }

        private static URI createJarUri(File jarFile, String entryName) {
            URI jarURI = jarFile.toURI().normalize();
            String separator = entryName.startsWith("/") ? "!" : "!/";
            try {
                // The jar URI convention appears to be not to re-encode the jarURI
                return new URI("jar:" + jarURI + separator + entryName);
            } catch (URISyntaxException e) {
                throw new CannotCreateUriError(jarURI + separator + entryName, e);
            }
        }
    }


    static PathFileObject forDexClass(BaseFileManager fileManager, File dexClass){
        return new DexFileObject(fileManager,dexClass);
    }

    private static class DexFileObject extends PathFileObject{

        /**
         * Create a PathFileObject, for a specified path, in the context of
         * a given file manager.
         * <p>
         * In general, this path should be an
         * absolute path}, if not a real path.
         * It will be used as the basis of {@code equals}, {@code hashCode}
         * and {@code isSameFile} methods on this file object.
         * <p>
         * A PathFileObject should also have a "friendly name" per the
         * specification for {@link FileObject#getName}. The friendly name
         * is provided by the various subtypes of {@code PathFileObject}.
         *
         * @param fileManager the file manager creating this file object
         * @param path        the path contained in this file object.
         */
        protected DexFileObject(BaseFileManager fileManager, File path) {
            super(fileManager, path);
        }

        @Override
        public Kind getKind() {
            return Kind.DEX_CALSS;
        }


        @Override
        String inferBinaryName(Iterable<? extends File> paths) {
            return toBinaryName(((DexClass)path).getClassName());
        }

        @Override
        PathFileObject getSibling(String basename) {
            return forDexClass(fileManager,FileUtils.resolveSibling(path,basename));
        }

        @Override
        public String getName() {
            return path.getPath();
        }

    }

    /**
     * Create a PathFileObject for a file whose binary name must be inferred
     * from its position on a search path.
     *
     * The binary name is inferred by finding an enclosing directory in
     * the sequence of paths associated with the location given to
     * {@link javax.tools.JavaFileManager#inferBinaryName).
     * The name is derived from {@code userPath}.
     *
     * @param fileManager the file manager creating this file object
     * @param path the path referred to by this file object
     * @param userPath the "user-friendly" name for this path.
     */
    static PathFileObject forSimplePath(BaseFileManager fileManager,
            File path, File userPath) {
        return new SimpleFileObject(fileManager, path, userPath);
    }

    private static class SimpleFileObject extends PathFileObject {
        private final File userPath;
        private SimpleFileObject(BaseFileManager fileManager, File path, File userPath) {
            super(fileManager, path);
            this.userPath = userPath;
        }

        @Override @DefinedBy(Api.COMPILER)
        public String getName() {
            return userPath.toString();
        }

        @Override
        public String inferBinaryName(Iterable<? extends File> paths) {
            File absPath = path.getAbsoluteFile();
            for (File p: paths) {
                File ap = p.getAbsoluteFile();
                if (FileUtils.startsWith(absPath,ap)) {
                    try {
                        String rp = FileUtils.relativize(ap,absPath);
                        if (rp != null) // maybe null if absPath same as ap
                            return toBinaryName(rp);
                    } catch (IllegalArgumentException e) {
                        // ignore this p if cannot relativize path to p
                    }
                }
            }
            return null;
        }

        @Override @DefinedBy(Api.COMPILER)
        public Kind getKind() {
            return BaseFileManager.getKind(userPath);
        }

        @Override @DefinedBy(Api.COMPILER)
        public boolean isNameCompatible(String simpleName, Kind kind) {
            return isPathNameCompatible(userPath, simpleName, kind);
        }

        @Override @DefinedBy(Api.COMPILER)
        public URI toUri() {
            return userPath.toURI().normalize();
        }

        @Override
        PathFileObject getSibling(String baseName) {
            return new SimpleFileObject(fileManager,
                    FileUtils.resolveSibling(path,baseName),
                    FileUtils.resolveSibling(userPath,baseName)
            );
        }
    }

    /**
     * Create a PathFileObject, for a specified path, in the context of
     * a given file manager.
     *
     * In general, this path should be an
     * absolute path}, if not a real path.
     * It will be used as the basis of {@code equals}, {@code hashCode}
     * and {@code isSameFile} methods on this file object.
     *
     * A PathFileObject should also have a "friendly name" per the
     * specification for {@link FileObject#getName}. The friendly name
     * is provided by the various subtypes of {@code PathFileObject}.
     *
     * @param fileManager the file manager creating this file object
     * @param path the path contained in this file object.
     */
    protected PathFileObject(BaseFileManager fileManager, File path) {
        this.fileManager = Objects.requireNonNull(fileManager);
        if (path.isDirectory()) {
            throw new IllegalArgumentException("directories not supported");
        }
        this.path = path;
    }

    /**
     * See {@link JavacFileManager#inferBinaryName}.
     */
    abstract String inferBinaryName(Iterable<? extends File> paths);

    /**
     * Return the file object for a sibling file with a given file name.
     * See {@link JavacFileManager#getFileForOutput} and
     * {@link JavacFileManager#getJavaFileForOutput}.
     */
    abstract PathFileObject getSibling(String basename);

    /**
     * Return the Path for this object.
     * @return the Path for this object.
     */
    public File getPath() {
        return path;
    }

    /**
     * The short name is used when generating raw diagnostics.
     * @return the last component of the path
     */
    public String getShortName() {
        return path.getPath();
    }

    @Override @DefinedBy(Api.COMPILER)
    public Kind getKind() {
        return BaseFileManager.getKind(path);
    }

    @Override @DefinedBy(Api.COMPILER)
    public boolean isNameCompatible(String simpleName, Kind kind) {
        return isPathNameCompatible(path, simpleName, kind);
    }

    protected boolean isPathNameCompatible(File p, String simpleName, Kind kind) {
        Objects.requireNonNull(simpleName);
        Objects.requireNonNull(kind);

        if (kind == Kind.OTHER && BaseFileManager.getKind(p) != kind) {
            return false;
        }

        String sn = simpleName + kind.extension;
        String pn = p.getPath();
        return pn.equals(sn);

    }

    @Override @DefinedBy(Api.COMPILER)
    public NestingKind getNestingKind() {
        return null;
    }

    @Override @DefinedBy(Api.COMPILER)
    public Modifier getAccessLevel() {
        return null;
    }

    @Override @DefinedBy(Api.COMPILER)
    public URI toUri() {
        return path.toURI();
    }

    @Override @DefinedBy(Api.COMPILER)
    public InputStream openInputStream() throws IOException {
        fileManager.updateLastUsedTime();
        return FileUtils.newInputStream(path);
    }

    @Override @DefinedBy(Api.COMPILER)
    public OutputStream openOutputStream() throws IOException {
        fileManager.updateLastUsedTime();
        fileManager.flushCache(this);
        ensureParentDirectoriesExist();
        return new FileOutputStream(path);
    }

    @Override @DefinedBy(Api.COMPILER)
    public Reader openReader(boolean ignoreEncodingErrors) throws IOException {
        CharsetDecoder decoder = fileManager.getDecoder(fileManager.getEncodingName(), ignoreEncodingErrors);
        return new InputStreamReader(openInputStream(), decoder);
    }

    @Override @DefinedBy(Api.COMPILER)
    public CharSequence getCharContent(boolean ignoreEncodingErrors) throws IOException {
        CharBuffer cb = fileManager.getCachedContent(this);
        if (cb == null) {
            try (InputStream in = openInputStream()) {
                ByteBuffer bb = fileManager.makeByteBuffer(in);
                JavaFileObject prev = fileManager.log.useSource(this);
                try {
                    cb = fileManager.decode(bb, ignoreEncodingErrors);
                } finally {
                    fileManager.log.useSource(prev);
                }
                fileManager.recycleByteBuffer(bb);
                if (!ignoreEncodingErrors) {
                    fileManager.cache(this, cb);
                }
            }
        }
        return cb;
    }

    @Override @DefinedBy(Api.COMPILER)
    public Writer openWriter() throws IOException {
        fileManager.updateLastUsedTime();
        fileManager.flushCache(this);
        ensureParentDirectoriesExist();
        return new OutputStreamWriter(new FileOutputStream(path), fileManager.getEncodingName());
    }

    @Override @DefinedBy(Api.COMPILER)
    public long getLastModified() {

        return path.lastModified();
    }

    @Override @DefinedBy(Api.COMPILER)
    public boolean delete() {

        return path.delete();
    }

    boolean isSameFile(PathFileObject other) {
        // By construction, the "path" field should be canonical in all likely, supported scenarios.
        // (Any exceptions would involve the use of symlinks within a package hierarchy.)
        // Therefore, it is sufficient to check that the paths are .equals.
        return path.equals(other.path);
    }

    @Override
    public boolean equals(Object other) {
        return (other instanceof PathFileObject && path.equals(((PathFileObject) other).path));
    }

    @Override
    public int hashCode() {
        return path.hashCode();
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + "[" + path + "]";
    }

    private void ensureParentDirectoriesExist() throws IOException {
        if (!hasParents) {
            File parent = path.getParentFile();
            if (parent != null && !parent.isDirectory()) {
                if(!parent.mkdirs())
                    throw new IOException("could not create parent directories");
            }
            hasParents = true;
        }
    }

    protected static String toBinaryName(RelativePath relativePath) {
        return toBinaryName(relativePath.path, "/");
    }
    protected static String toBinaryName(String relativePath){
        return toBinaryName(relativePath,File.separator);
    }
    protected static String toBinaryName(File relativePath) {
        return toBinaryName(relativePath.toString(),
                File.separator);
    }

    private static String toBinaryName(String relativePath, String sep) {
        return removeExtension(relativePath).replace(sep, ".");
    }

    private static String removeExtension(String fileName) {
        int lastDot = fileName.lastIndexOf(".");
        return (lastDot == -1 ? fileName : fileName.substring(0, lastDot));
    }

    /**
     * Return the last component of a presumed hierarchical URI.
     * From the scheme specific part of the URI, it returns the substring
     * after the last "/" if any, or everything if no "/" is found.
     * @param fo the file object
     * @return the simple name of the file object
     */
    public static String getSimpleName(FileObject fo) {
        URI uri = fo.toUri();
        String s = uri.getSchemeSpecificPart();
        return s.substring(s.lastIndexOf("/") + 1); // safe when / not found

    }

    /** Used when URLSyntaxException is thrown unexpectedly during
     *  implementations of FileObject.toURI(). */
    public static class CannotCreateUriError extends Error {
        private static final long serialVersionUID = 9101708840997613546L;
        public CannotCreateUriError(String value, Throwable cause) {
            super(value, cause);
        }
    }
}
