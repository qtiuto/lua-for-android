/*
 * Copyright (c) 2005, 2017, Oracle and/or its affiliates. All rights reserved.
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

import com.oslorde.dexresolver.Dex;
import com.oslorde.dexresolver.DexResolver;
import com.sun.tools.javac.file.RelativePath.RelativeDirectory;
import com.sun.tools.javac.file.RelativePath.RelativeFile;
import com.sun.tools.javac.util.Context;
import com.sun.tools.javac.util.Context.Factory;
import com.sun.tools.javac.util.DefinedBy;
import com.sun.tools.javac.util.DefinedBy.Api;
import com.sun.tools.javac.util.JDK9Wrappers.Configuration;
import com.sun.tools.javac.util.JDK9Wrappers.Layer;
import com.sun.tools.javac.util.JDK9Wrappers.Module;
import com.sun.tools.javac.util.JDK9Wrappers.ModuleFinder;
import com.sun.tools.javac.util.JDK9Wrappers.ServiceLoaderHelper;
import com.sun.tools.javac.util.List;
import com.sun.tools.javac.util.ListBuffer;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.nio.file.ProviderNotFoundException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.ServiceLoader;
import java.util.Set;

import javax.lang.model.SourceVersion;
import javax.tools.FileObject;
import javax.tools.JavaFileManager;
import javax.tools.JavaFileObject;
import javax.tools.StandardJavaFileManager;

import static javax.tools.StandardLocation.CLASS_OUTPUT;
import static javax.tools.StandardLocation.SOURCE_OUTPUT;

/**
 * This class provides access to the source, class and other files
 * used by the compiler and related tools.
 *
 * <p><b>This is NOT part of any supported API.
 * If you write code that depends on this, you do so at your own risk.
 * This code and its internal interfaces are subject to change or
 * deletion without notice.</b>
 */
public class JavacFileManager extends BaseFileManager implements StandardJavaFileManager {

    public static char[] toArray(CharBuffer buffer) {
        if (buffer.hasArray())
            return ((CharBuffer) buffer.compact().flip()).array();
        else
            return buffer.toString().toCharArray();
    }

    private FSInfo fsInfo;

    private final Set<JavaFileObject.Kind> sourceOrClass =
            EnumSet.of(JavaFileObject.Kind.SOURCE, JavaFileObject.Kind.CLASS);

    protected boolean symbolFileEnabled;


    protected enum SortFiles implements Comparator<File> {
        FORWARD {
            @Override
            public int compare(File f1, File f2) {
                return f1.getPath().compareTo(f2.getPath());
            }
        },
        REVERSE {
            @Override
            public int compare(File f1, File f2) {
                return -f1.getPath().compareTo(f2.getPath());

            }
        }
    }

    protected SortFiles sortFiles;

    /**
     * Register a Context.Factory to create a JavacFileManager.
     */
    public static void preRegister(Context context) {
        context.put(JavaFileManager.class,
                (Factory<JavaFileManager>) c -> new JavacFileManager(c, true, null));
    }

    /**
     * Create a JavacFileManager using a given context, optionally registering
     * it as the JavaFileManager for that context.
     */
    public JavacFileManager(Context context, boolean register, Charset charset) {
        super(charset);
        if (register)
            context.put(JavaFileManager.class, this);
        setContext(context);
    }

    /**
     * Set the context for JavacFileManager.
     */
    @Override
    public void setContext(Context context) {
        super.setContext(context);

        fsInfo = FSInfo.instance(context);

        symbolFileEnabled = !options.isSet("ignore.symbol.file");

        String sf = options.get("sortFiles");
        if (sf != null) {
            sortFiles = (sf.equals("reverse") ? SortFiles.REVERSE : SortFiles.FORWARD);
        }
    }


    private File getPath(String first, String... more) {
        return FileUtils.get(first, more);
    }

    /**
     * Set whether or not to use ct.sym as an alternate to rt.jar.
     */
    public void setSymbolFileEnabled(boolean b) {
        symbolFileEnabled = b;
    }

    public boolean isSymbolFileEnabled() {
        return symbolFileEnabled;
    }

    // used by tests
    public JavaFileObject getJavaFileObject(String name) {
        return getJavaFileObjects(name).iterator().next();
    }

    // used by tests
    public JavaFileObject getJavaFileObject(File file) {
        return getJavaFileObjects(file).iterator().next();
    }

    public JavaFileObject getFileForOutput(String classname,
                                           JavaFileObject.Kind kind,
                                           JavaFileObject sibling)
            throws IOException {
        return getJavaFileForOutput(CLASS_OUTPUT, classname, kind, sibling);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public Iterable<? extends JavaFileObject> getJavaFileObjectsFromStrings(Iterable<String> names) {
        ListBuffer<File> paths = new ListBuffer<>();
        for (String name : names)
            paths.append(getPath(nullCheck(name)));
        return getJavaFileObjectsFromFiles(paths.toList());
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public Iterable<? extends JavaFileObject> getJavaFileObjects(String... names) {
        return getJavaFileObjectsFromStrings(Arrays.asList(nullCheck(names)));
    }

    private static boolean isValidName(String name) {
        // Arguably, isValidName should reject keywords (such as in SourceVersion.isName() ),
        // but the set of keywords depends on the source level, and we don't want
        // impls of JavaFileManager to have to be dependent on the source level.
        // Therefore we simply check that the argument is a sequence of identifiers
        // separated by ".".
        for (String s : name.split("\\.", -1)) {
            if (!SourceVersion.isIdentifier(s))
                return false;
        }
        return true;
    }

    private static void validateClassName(String className) {
        if (!isValidName(className))
            throw new IllegalArgumentException("Invalid class name: " + className);
    }

    private static void validatePackageName(String packageName) {
        if (packageName.length() > 0 && !isValidName(packageName))
            throw new IllegalArgumentException("Invalid packageName name: " + packageName);
    }

    public static void testName(String name,
                                boolean isValidPackageName,
                                boolean isValidClassName) {
        try {
            validatePackageName(name);
            if (!isValidPackageName)
                throw new AssertionError("Invalid package name accepted: " + name);
            printAscii("Valid package name: \"%s\"", name);
        } catch (IllegalArgumentException e) {
            if (isValidPackageName)
                throw new AssertionError("Valid package name rejected: " + name);
            printAscii("Invalid package name: \"%s\"", name);
        }
        try {
            validateClassName(name);
            if (!isValidClassName)
                throw new AssertionError("Invalid class name accepted: " + name);
            printAscii("Valid class name: \"%s\"", name);
        } catch (IllegalArgumentException e) {
            if (isValidClassName)
                throw new AssertionError("Valid class name rejected: " + name);
            printAscii("Invalid class name: \"%s\"", name);
        }
    }

    private static void printAscii(String format, Object... args) {
        String message;
        try {
            final String ascii = "US-ASCII";
            message = new String(String.format(null, format, args).getBytes(ascii), ascii);
        } catch (java.io.UnsupportedEncodingException ex) {
            throw new AssertionError(ex);
        }
        System.out.println(message);
    }

    private final Map<File, Container> containers = new HashMap<>();
    private DexResolver resolver=new DexResolver();

    {
        try {
            Container bootContainer=new DexContainer(resolver.getBootDexFiles());
            containers.put(Locations.BOOT_LOCATION,bootContainer);
        } catch (Exception e) {
        }
    }

    synchronized Container getContainer(File path) throws IOException {
        Container fs = containers.get(path);

        if (fs != null) {
            return fs;
        }

        File realPath = fsInfo.getCanonicalFile(path);

        fs = containers.get(realPath);

        if (fs != null) {
            containers.put(path, fs);
            return fs;
        }


        if (realPath.isDirectory()) {
            fs = new DirectoryContainer(realPath);
        } else {
            try {
                try {
                    fs=new DexContainer(resolver.addFile(path));
                }catch (IOException ignored){
                    fs=new ArchiveContainer(realPath);
                }
            } catch (Exception ex) {
                throw new IOException(ex);
            }
        }


        containers.put(realPath, fs);
        containers.put(path, fs);

        return fs;
    }

    private interface Container {
        /**
         * Insert all files in subdirectory subdirectory of container which
         * match fileKinds into resultList
         */
        void list(File userPath,
                  RelativeDirectory subdirectory,
                  Set<JavaFileObject.Kind> fileKinds,
                  boolean recurse,
                  ListBuffer<JavaFileObject> resultList) throws IOException;

        JavaFileObject getFileObject(File userPath, RelativeFile name) throws IOException;

        void close() throws IOException;
    }

    private static final Container MISSING_CONTAINER = new Container() {
        @Override
        public void list(File userPath,
                         RelativeDirectory subdirectory,
                         Set<JavaFileObject.Kind> fileKinds,
                         boolean recurse,
                         ListBuffer<JavaFileObject> resultList) throws IOException {
        }

        @Override
        public JavaFileObject getFileObject(File userPath, RelativeFile name) throws IOException {
            return null;
        }

        @Override
        public void close() throws IOException {
        }
    };


    private final class DirectoryContainer implements Container {
        private final File directory;

        public DirectoryContainer(File directory) {
            this.directory = directory;
        }

        /**
         * Insert all files in subdirectory subdirectory of directory userPath
         * which match fileKinds into resultList
         */
        @Override
        public void list(File userPath,
                         RelativeDirectory subdirectory,
                         Set<JavaFileObject.Kind> fileKinds,
                         boolean recurse,
                         ListBuffer<JavaFileObject> resultList) throws IOException {
            File d = subdirectory.resolveAgainst(userPath);
            if (!d.exists()) {
                return;
            }
            //no need
            /*if (!caseMapCheck(d, subdirectory)) {
                return;
            }*/

            File[] files;
            try {
                 files=d.listFiles();
                 if(sortFiles!=null) Arrays.sort(files,sortFiles);
            } catch (Exception ignore) {
                return;
            }

            for (File f : files) {
                String fname = f.getPath();
                if (fname.endsWith("/"))
                    fname = fname.substring(0, fname.length() - 1);
                if (f.isDirectory()) {
                    if (recurse && SourceVersion.isIdentifier(fname)) {
                        list(userPath,
                                new RelativeDirectory(subdirectory, fname),
                                fileKinds,
                                recurse,
                                resultList);
                    }
                } else {
                    if (isValidFile(fname, fileKinds)) {
                        try {
                            RelativeFile file = new RelativeFile(subdirectory, fname);
                            JavaFileObject fe = PathFileObject.forDirectoryPath(JavacFileManager.this,
                                    file.resolveAgainst(directory), userPath, file);
                            resultList.append(fe);
                        } catch (Exception e) {
                            throw new IOException("error accessing directory " + directory + e);
                        }
                    }
                }
            }
        }

        @Override
        public JavaFileObject getFileObject(File userPath, RelativeFile name) throws IOException {
            try {
                File f = name.resolveAgainst(userPath);
                if (f.exists())
                    return PathFileObject.forSimplePath(JavacFileManager.this,
                            fsInfo.getCanonicalFile(f), f);
            } catch (Exception ignore) {
            }
            return null;
        }

        @Override
        public void close() throws IOException {
        }
    }

    private final class ArchiveContainer implements Container {
        private final File archivePath;
        private final OrderedZipFile orderedZipFile;
        private final Map<RelativePath, File> packages;

        public ArchiveContainer(File archivePath) throws IOException, ProviderNotFoundException, SecurityException {
            this.archivePath = archivePath;

            this.orderedZipFile = new OrderedZipFile(archivePath.getPath());

            packages = new HashMap<>();
            for (OrderedZipFile.EntryFile f:orderedZipFile.getEntries()){
                if(f.isDirectory()&&isValid(f.getEntryName())){
                    packages.put(new RelativeDirectory(f.getEntryName()), f);
                }
            }
        }

        /**
         * Insert all files in subdirectory subdirectory of this archive
         * which match fileKinds into resultList
         */
        @Override
        public void list(File userPath,
                         RelativeDirectory subdirectory,
                         Set<JavaFileObject.Kind> fileKinds,
                         boolean recurse,
                         ListBuffer<JavaFileObject> resultList) throws IOException {
            File resolvedSubdirectory = packages.get(subdirectory);

            if (resolvedSubdirectory == null)
                return;

            File[] files;
            try {
                files=resolvedSubdirectory.listFiles();
                if(sortFiles!=null) Arrays.sort(files,sortFiles);
            } catch (Exception ignore) {
                return;
            }

            for (File f : files) {
                String fname = f.getPath();
                if (fname.endsWith("/"))
                    fname = fname.substring(0, fname.length() - 1);
                if (f.isDirectory()) {
                    if (recurse && SourceVersion.isIdentifier(fname)) {
                        list(userPath,
                                new RelativeDirectory(subdirectory, fname),
                                fileKinds,
                                recurse,
                                resultList);
                    }
                } else {
                    if (isValidFile(fname, fileKinds)) {
                        RelativeFile file = new RelativeFile(subdirectory, fname);
                        JavaFileObject fe = PathFileObject.forDirectoryPath(JavacFileManager.this,
                                f, archivePath, file);
                        resultList.append(fe);
                    }
                }
            }

        }

        private boolean isValid(String name) {
            if (name == null) {
                return true;
            } else {
                if (name.endsWith("/")) {
                    name = name.substring(0, name.length() - 1);
                }
                return SourceVersion.isIdentifier(name);
            }

        }

        @Override
        public JavaFileObject getFileObject(File userPath, RelativeFile name) throws IOException {
            RelativeDirectory root = name.dirname();
            File packagepath = packages.get(root);
            if (packagepath != null) {
                File relpath = FileUtils.resolve(packagepath,name.basename());
                if (relpath.exists()) {
                    return PathFileObject.forJarPath(JavacFileManager.this, relpath, archivePath,name);
                }
            }
            return null;
        }

        @Override
        public void close() throws IOException {
            orderedZipFile.close();
        }
    }

    private final class DexContainer implements Container {

        Dex[] dexes;

        DexContainer(Dex[] dexes) {
            this.dexes=dexes;
        }

        @Override
        public void list(File userPath, RelativeDirectory subdirectory, Set<JavaFileObject.Kind> fileKinds, boolean recurse, ListBuffer<JavaFileObject> resultList) throws IOException {
            //HashSet<String> packages=new HashSet<>();
            String pack=subdirectory.path;
            int length=pack.length();
            for(Dex dex:dexes){
                dex.listPackage(pack,true,recurse,(d,name)->{
//                    int lastDot=name.indexOf('.',length);
//                    if(lastDot!=-1){
//                        String subPack=name.substring(0,lastDot);
//                        if(packages.add(subPack)){
//                            resultList.add(PathFileObject.forDexClass(JavacFileManager.this,new DexClass(d,subPack)));
//                        }
//                        if(recurse){
//                            while ((lastDot=name.indexOf('.',lastDot+1))!=-1){
//                                subPack=name.substring(0,lastDot);
//                                if(packages.add(subPack)){
//                                    resultList.add(PathFileObject.forDexClass(JavacFileManager.this,new DexClass(d,subPack)));
//                                }
//                            }
//                        }
//                    }else
                    resultList.add(PathFileObject.forDexClass(JavacFileManager.this,new DexClass(d,name)));
                });
            }

        }

        @Override
        public JavaFileObject getFileObject(File userPath, RelativeFile name) throws IOException {
            for (Dex dex:dexes){
                DexClass dexClass=new DexClass(dex,name.path.replace('/','.'));
                if(dexClass.exists()){
                    return PathFileObject.forDexClass(JavacFileManager.this,dexClass);
                }
            }
            return null;
        }

        @Override
        public void close() throws IOException {
            resolver.remove(dexes);
        }
    }

    /**
     * container is a directory, a zip file, or a non-existent path.
     */
    private boolean isValidFile(String s, Set<JavaFileObject.Kind> fileKinds) {
        JavaFileObject.Kind kind = getKind(s);
        return fileKinds.contains(kind);
    }

    private static final boolean fileSystemIsCaseSensitive =
            File.separatorChar == '/';

    /**
     * Hack to make Windows case sensitive. Test whether given path
     * ends in a string of characters with the same case as given name.
     * Ignore file separators in both path and name.
     */
    private boolean caseMapCheck(File f, RelativePath name) {
        if (fileSystemIsCaseSensitive) return true;
        // Note that toRealPath() returns the case-sensitive
        // spelled file name.
        String path;
        char sep = '/';
        try {
            path = f.getCanonicalPath();
        } catch (IOException ex) {
            return false;
        }
        char[] pcs = path.toCharArray();
        char[] ncs = name.path.toCharArray();
        int i = pcs.length - 1;
        int j = ncs.length - 1;
        while (i >= 0 && j >= 0) {
            while (i >= 0 && pcs[i] == sep) i--;
            while (j >= 0 && ncs[j] == '/') j--;
            if (i >= 0 && j >= 0) {
                if (pcs[i] != ncs[j]) return false;
                i--;
                j--;
            }
        }
        return j < 0;
    }

    /**
     * Flush any output resources.
     */
    @Override
    @DefinedBy(Api.COMPILER)
    public void flush() {
        contentCache.clear();
    }

    /**
     * Close the JavaFileManager, releasing resources.
     */
    @Override
    @DefinedBy(Api.COMPILER)
    public void close() throws IOException {
        if (deferredCloseTimeout > 0) {
            deferredClose();
            return;
        }

        locations.close();
        for (Container container : containers.values()) {
            container.close();
        }
        containers.clear();
        contentCache.clear();
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public ClassLoader getClassLoader(Location location) {
        checkNotModuleOrientedLocation(location);
        Iterable<? extends File> path = getLocation(location);
        if (path == null)
            return null;
        ListBuffer<URL> lb = new ListBuffer<>();
        for (File f : path) {
            try {
                lb.append(f.toURI().toURL());
            } catch (MalformedURLException e) {
                throw new AssertionError(e);
            }
        }

        return getClassLoader(lb.toArray(new URL[lb.size()]));
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public Iterable<JavaFileObject> list(Location location,
                                         String packageName,
                                         Set<JavaFileObject.Kind> kinds,
                                         boolean recurse)
            throws IOException {
        checkNotModuleOrientedLocation(location);
        // validatePackageName(packageName);
        nullCheck(packageName);
        nullCheck(kinds);

        Iterable<? extends File> path = getLocation(location);
        if (path == null)
            return List.nil();
        RelativeDirectory subdirectory = RelativeDirectory.forPackage(packageName);
        ListBuffer<JavaFileObject> results = new ListBuffer<>();

        for (File directory : path) {
            Container container = getContainer(directory);

            container.list(directory, subdirectory, kinds, recurse, results);
        }

        return results.toList();
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public String inferBinaryName(Location location, JavaFileObject file) {
        checkNotModuleOrientedLocation(location);
        Objects.requireNonNull(file);
        // Need to match the path semantics of list(location, ...)
        Iterable<? extends File> path = getLocation(location);
        if (path == null) {
            return null;
        }

        if (file instanceof PathFileObject) {
            return ((PathFileObject) file).inferBinaryName(path);
        } else
            throw new IllegalArgumentException(file.getClass().getName());
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public boolean isSameFile(FileObject a, FileObject b) {
        nullCheck(a);
        nullCheck(b);
        if (a instanceof PathFileObject && b instanceof PathFileObject)
            return ((PathFileObject) a).isSameFile((PathFileObject) b);
        return a.equals(b);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public boolean hasLocation(Location location) {
        nullCheck(location);
        return locations.hasLocation(location);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public JavaFileObject getJavaFileForInput(Location location,
                                              String className,
                                              JavaFileObject.Kind kind)
            throws IOException {
        checkNotModuleOrientedLocation(location);
        // validateClassName(className);
        nullCheck(className);
        nullCheck(kind);
        if (!sourceOrClass.contains(kind))
            throw new IllegalArgumentException("Invalid kind: " + kind);
        return getFileForInput(location, RelativeFile.forClass(className, kind));
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public FileObject getFileForInput(Location location,
                                      String packageName,
                                      String relativeName)
            throws IOException {
        checkNotModuleOrientedLocation(location);
        // validatePackageName(packageName);
        nullCheck(packageName);
        if (!isRelativeUri(relativeName))
            throw new IllegalArgumentException("Invalid relative name: " + relativeName);
        RelativeFile name = packageName.length() == 0
                ? new RelativeFile(relativeName)
                : new RelativeFile(RelativeDirectory.forPackage(packageName), relativeName);
        return getFileForInput(location, name);
    }

    private JavaFileObject getFileForInput(Location location, RelativeFile name) throws IOException {
        Iterable<? extends File> path = getLocation(location);
        if (path == null)
            return null;

        for (File file : path) {
            JavaFileObject fo = getContainer(file).getFileObject(file, name);

            if (fo != null) {
                return fo;
            }
        }
        return null;
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public JavaFileObject getJavaFileForOutput(Location location,
                                               String className,
                                               JavaFileObject.Kind kind,
                                               FileObject sibling)
            throws IOException {
        checkOutputLocation(location);
        // validateClassName(className);
        nullCheck(className);
        nullCheck(kind);
        if (!sourceOrClass.contains(kind))
            throw new IllegalArgumentException("Invalid kind: " + kind);
        return getFileForOutput(location, RelativeFile.forClass(className, kind), sibling);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public FileObject getFileForOutput(Location location,
                                       String packageName,
                                       String relativeName,
                                       FileObject sibling)
            throws IOException {
        checkOutputLocation(location);
        // validatePackageName(packageName);
        nullCheck(packageName);
        if (!isRelativeUri(relativeName))
            throw new IllegalArgumentException("Invalid relative name: " + relativeName);
        RelativeFile name = packageName.length() == 0
                ? new RelativeFile(relativeName)
                : new RelativeFile(RelativeDirectory.forPackage(packageName), relativeName);
        return getFileForOutput(location, name, sibling);
    }

    private JavaFileObject getFileForOutput(Location location,
                                            RelativeFile fileName,
                                            FileObject sibling)
            throws IOException {
        File dir;
        if (location == CLASS_OUTPUT) {
            if (getClassOutDir() != null) {
                dir = getClassOutDir();
            } else {
                String baseName = fileName.basename();
                if (sibling != null && sibling instanceof PathFileObject) {
                    return ((PathFileObject) sibling).getSibling(baseName);
                } else {
                    File p = getPath(baseName);
                    File real = fsInfo.getCanonicalFile(p);
                    return PathFileObject.forSimplePath(this, real, p);
                }
            }
        } else if (location == SOURCE_OUTPUT) {
            dir = (getSourceOutDir() != null ? getSourceOutDir() : getClassOutDir());
        } else {
            Iterable<? extends File> path = locations.getLocation(location);
            dir = null;
            for (File f : path) {
                dir = f;
                break;
            }
        }


        if (dir == null) {
            dir = getPath(System.getProperty("user.dir"));
        }
        File path = fileName.resolveAgainst(fsInfo.getCanonicalFile(dir));
        return PathFileObject.forDirectoryPath(this, path, dir, fileName);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public Iterable<? extends JavaFileObject> getJavaFileObjectsFromFiles(
            Iterable<? extends File> files) {
        ArrayList<PathFileObject> result;
        if (files instanceof Collection<?>)
            result = new ArrayList<>(((Collection<?>) files).size());
        else
            result = new ArrayList<>();
        for (File f : files) {
            Objects.requireNonNull(f);
            result.add(PathFileObject.forSimplePath(this,
                    fsInfo.getCanonicalFile(f), f));
        }
        return result;
    }


    @Override
    @DefinedBy(Api.COMPILER)
    public Iterable<? extends JavaFileObject> getJavaFileObjects(File... files) {
        return getJavaFileObjectsFromFiles(Arrays.asList(nullCheck(files)));
    }


    @Override
    @DefinedBy(Api.COMPILER)
    public void setLocation(Location location,
                            Iterable<? extends File> searchpath)
            throws IOException {
        nullCheck(location);
        locations.setLocation(location, searchpath);
    }


    @Override
    @DefinedBy(Api.COMPILER)
    public Iterable<? extends File> getLocation(Location location) {
        nullCheck(location);
        return locations.getLocation(location);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public boolean contains(Location location, FileObject fo) throws IOException {
        nullCheck(location);
        nullCheck(fo);
        return locations.contains(location, asPath(fo));
    }

    private File getClassOutDir() {
        return locations.getOutputLocation(CLASS_OUTPUT);
    }

    private File getSourceOutDir() {
        return locations.getOutputLocation(SOURCE_OUTPUT);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public Location getLocationForModule(Location location, String moduleName) throws IOException {
        checkModuleOrientedOrOutputLocation(location);
        nullCheck(moduleName);
        if (location == SOURCE_OUTPUT && getSourceOutDir() == null)
            location = CLASS_OUTPUT;
        return locations.getLocationForModule(location, moduleName);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public <S> ServiceLoader<S> getServiceLoader(Location location, Class<S> service) throws IOException {
        nullCheck(location);
        nullCheck(service);
        Module.getModule(getClass()).addUses(service);
        if (location.isModuleOrientedLocation()) {
            Collection<File> paths = locations.getLocation(location);
            ModuleFinder finder = ModuleFinder.of(paths.toArray(new File[paths.size()]));
            Layer bootLayer = Layer.boot();
            Configuration cf = bootLayer.configuration().resolveAndBind(ModuleFinder.of(), finder, Collections.emptySet());
            Layer layer = bootLayer.defineModulesWithOneLoader(cf, ClassLoader.getSystemClassLoader());
            return ServiceLoaderHelper.load(layer, service);
        } else {
            return ServiceLoader.load(service, getClassLoader(location));
        }
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public Location getLocationForModule(Location location, JavaFileObject fo) throws IOException {
        checkModuleOrientedOrOutputLocation(location);
        if (!(fo instanceof PathFileObject))
            return null;
        File p = Locations.normalize(((PathFileObject) fo).path);
        // need to find p in location
        return locations.getLocationForModule(location, p);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public void setLocationForModule(Location location, String moduleName, Collection<? extends File> paths)
            throws IOException {
        nullCheck(location);
        checkModuleOrientedOrOutputLocation(location);
        locations.setLocationForModule(location, nullCheck(moduleName), nullCheck(paths));
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public String inferModuleName(Location location) {
        checkNotModuleOrientedLocation(location);
        return locations.inferModuleName(location);
    }

    @Override
    @DefinedBy(Api.COMPILER)
    public Iterable<Set<Location>> listLocationsForModules(Location location) throws IOException {
        checkModuleOrientedOrOutputLocation(location);
        return locations.listLocationsForModules(location);
    }

    //@Override
    @DefinedBy(Api.COMPILER)
    public File asPath(FileObject file) {
        if (file instanceof PathFileObject) {
            return ((PathFileObject) file).path;
        } else
            throw new IllegalArgumentException(file.getName());
    }

    /**
     * Enforces the specification of a "relative" name as used in
     * {@linkplain #getFileForInput(Location, String, String)
     * getFileForInput}.  This method must follow the rules defined in
     * that method, do not make any changes without consulting the
     * specification.
     */
    protected static boolean isRelativeUri(URI uri) {
        if (uri.isAbsolute())
            return false;
        String path = uri.normalize().getPath();
        if (path.length() == 0 /* isEmpty() is mustang API */)
            return false;
        if (!path.equals(uri.getPath())) // implicitly checks for embedded . and ..
            return false;
        return !path.startsWith("/") && !path.startsWith("./") && !path.startsWith("../");
    }

    // Convenience method
    protected static boolean isRelativeUri(String u) {
        try {
            return isRelativeUri(new URI(u));
        } catch (URISyntaxException e) {
            return false;
        }
    }

    /**
     * Converts a relative file name to a relative URI.  This is
     * different from File.toURI as this method does not canonicalize
     * the file before creating the URI.  Furthermore, no schema is
     * used.
     *
     * @param file a relative file name
     * @return a relative URI
     * @throws IllegalArgumentException if the file name is not
     *                                  relative according to the definition given in {@link
     *                                  javax.tools.JavaFileManager#getFileForInput}
     */
    public static String getRelativeName(File file) {
        if (!file.isAbsolute()) {
            String result = file.getPath().replace(File.separatorChar, '/');
            if (isRelativeUri(result))
                return result;
        }
        throw new IllegalArgumentException("Invalid relative path: " + file);
    }

    /**
     * Get a detail message from an IOException.
     * Most, but not all, instances of IOException provide a non-null result
     * for getLocalizedMessage().  But some instances return null: in these
     * cases, fallover to getMessage(), and if even that is null, return the
     * name of the exception itself.
     *
     * @param e an IOException
     * @return a string to include in a compiler diagnostic
     */
    public static String getMessage(IOException e) {
        String s = e.getLocalizedMessage();
        if (s != null)
            return s;
        s = e.getMessage();
        if (s != null)
            return s;
        return e.toString();
    }

    private void checkOutputLocation(Location location) {
        Objects.requireNonNull(location);
        if (!location.isOutputLocation())
            throw new IllegalArgumentException("location is not an output location: " + location.getName());
    }

    private void checkModuleOrientedOrOutputLocation(Location location) {
        Objects.requireNonNull(location);
        if (!location.isModuleOrientedLocation() && !location.isOutputLocation())
            throw new IllegalArgumentException(
                    "location is not an output location or a module-oriented location: "
                            + location.getName());
    }

    private void checkNotModuleOrientedLocation(Location location) {
        Objects.requireNonNull(location);
        if (location.isModuleOrientedLocation())
            throw new IllegalArgumentException("location is module-oriented: " + location.getName());
    }

}
