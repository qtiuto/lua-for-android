/*
 * Copyright (c) 2003, 2017, Oracle and/or its affiliates. All rights reserved.
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

import com.sun.tools.javac.code.Lint;
import com.sun.tools.javac.code.Lint.LintCategory;
import com.sun.tools.javac.jvm.ModuleNameReader;
import com.sun.tools.javac.main.Option;
import com.sun.tools.javac.resources.CompilerProperties.Errors;
import com.sun.tools.javac.resources.CompilerProperties.Warnings;
import com.sun.tools.javac.util.DefinedBy;
import com.sun.tools.javac.util.DefinedBy.Api;
import com.sun.tools.javac.util.Iterators;
import com.sun.tools.javac.util.ListBuffer;
import com.sun.tools.javac.util.Log;
import com.sun.tools.javac.util.Pair;
import com.sun.tools.javac.util.StringUtils;

import java.io.Closeable;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.EnumMap;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Objects;
import java.util.Set;
import java.util.jar.Attributes;
import java.util.jar.Manifest;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.zip.ZipFile;

import javax.lang.model.SourceVersion;
import javax.tools.JavaFileManager;
import javax.tools.JavaFileManager.Location;
import javax.tools.JavaFileObject;
import javax.tools.StandardJavaFileManager;
import javax.tools.StandardLocation;

import java9.util.function.Predicate;
import java9.util.stream.Collectors;
import java9.util.stream.Stream;
import java9.util.stream.StreamSupport;

import static com.sun.tools.javac.main.Option.BOOT_CLASS_PATH;
import static com.sun.tools.javac.main.Option.ENDORSEDDIRS;
import static com.sun.tools.javac.main.Option.EXTDIRS;
import static com.sun.tools.javac.main.Option.XBOOTCLASSPATH_APPEND;
import static com.sun.tools.javac.main.Option.XBOOTCLASSPATH_PREPEND;

/**
 * This class converts command line arguments, environment variables and system properties (in
 * File.pathSeparator-separated String form) into a boot class path, user class path, and source
 * path (in {@code Collection<String>} form).
 *
 * <p>
 * <b>This is NOT part of any supported API. If you write code that depends on this, you do so at
 * your own risk. This code and its internal interfaces are subject to change or deletion without
 * notice.</b>
 */
public class Locations {
    public static File BOOT_LOCATION =new File("/system/framework/"){
        @Override
        public boolean equals(Object obj) {
            return obj==this;
        }//FIXME, may work if actually occurs
    };

    /**
     * The log to use for warning output
     */
    private Log log;

    /**
     * Access to (possibly cached) file info
     */
    private FSInfo fsInfo;

    /**
     * Whether to warn about non-existent path elements
     */
    private boolean warn;

    private ModuleNameReader moduleNameReader;


    List<Closeable> closeables = new ArrayList<>();
    private Map<String,String> fsEnv = Collections.emptyMap();

    Locations() {
        initHandlers();
    }

    File getFile(String first, String... more) {

        return FileUtils.get(first, more);
    }

    public void close() throws IOException {
        ListBuffer<IOException> list = new ListBuffer<>();
        for(Closeable closeable: closeables){
            try {
                closeable.close();
            } catch (IOException ex) {
                list.add(ex);
            }
        };
        if (list.nonEmpty()) {
            IOException ex = new IOException();
            for (IOException e: list)
                ex.addSuppressed(e);
            throw ex;
        }
    }

    void update(Log log, boolean warn, FSInfo fsInfo) {
        this.log = log;
        this.warn = warn;
        this.fsInfo = fsInfo;
    }

    /**
     * Split a search path into its elements. Empty path elements will be ignored.
     *
     * @param searchFile The search path to be split
     * @return The elements of the path
     */
    private Iterable<File> getFileEntries(String searchFile) {
        return getFileEntries(searchFile, null);
    }

    /**
     * Split a search path into its elements. If emptyFileDefault is not null, all empty elements in the
     * path, including empty elements at either end of the path, will be replaced with the value of
     * emptyFileDefault.
     *
     * @param searchFile The search path to be split
     * @param emptyFileDefault The value to substitute for empty path elements, or null, to ignore
     * empty path elements
     * @return The elements of the path
     */
    private Iterable<File> getFileEntries(String searchFile, File emptyFileDefault) {
        ListBuffer<File> entries = new ListBuffer<>();
        for (String s: searchFile.split(Pattern.quote(File.pathSeparator), -1)) {
            if (s.isEmpty()) {
                if (emptyFileDefault != null) {
                    entries.add(emptyFileDefault);
                }
            } else {
                try {
                    entries.add(getFile(s));
                } catch (IllegalArgumentException e) {
                    if (warn) {
                        log.warning(LintCategory.PATH, Warnings.InvalidPath(s));
                    }
                }
            }
        }
        return entries;
    }

    public void setMultiReleaseValue(String multiReleaseValue) {
        fsEnv = Collections.singletonMap("multi-release", multiReleaseValue);
    }

    private boolean contains(Collection<File> searchFile, File file) throws IOException {

        if (searchFile == null) {
            return false;
        }

        File nf = normalize(file);
        for (File p : searchFile) {
            File np = normalize(p);
            if (FileUtils.startsWith(nf,np)) {
                return true;
            }
        }

        return false;
    }

    /**
     * Utility class to help evaluate a path option. Duplicate entries are ignored, jar class paths
     * can be expanded.
     */
    private class SearchFile extends LinkedHashSet<File> {

        private static final long serialVersionUID = 0;

        private boolean expandJarClassFiles = false;
        private final Set<File> canonicalValues = new HashSet<>();

        public SearchFile expandJarClassFiles(boolean x) {
            expandJarClassFiles = x;
            return this;
        }

        /**
         * What to use when path element is the empty string
         */
        private File emptyFileDefault = null;

        public SearchFile emptyFileDefault(File x) {
            emptyFileDefault = x;
            return this;
        }

        public SearchFile addDirectories(String dirs, boolean warn) {
            boolean prev = expandJarClassFiles;
            expandJarClassFiles = true;
            try {
                if (dirs != null) {
                    for (File dir : getFileEntries(dirs)) {
                        addDirectory(dir, warn);
                    }
                }
                return this;
            } finally {
                expandJarClassFiles = prev;
            }
        }

        public SearchFile addDirectories(String dirs) {
            return addDirectories(dirs, warn);
        }

        private void addDirectory(File dir, boolean warn) {
            if (!dir.isDirectory()) {
                if (warn) {
                    log.warning(Lint.LintCategory.PATH,
                                Warnings.DirPathElementNotFound(dir));
                }
                return;
            }

            try  {
                Stream<File> s = Stream.of(dir.listFiles());
                s.filter(Locations.this::isArchive)
                        .forEach(dirEntry -> addFile(dirEntry, warn));
                s.close();
            } catch (Exception ignore) {
            }
        }

        public SearchFile addFiles(String files, boolean warn) {
            if (files != null) {
                addFiles(getFileEntries(files, emptyFileDefault), warn);
            }
            return this;
        }

        public SearchFile addFiles(String files) {
            return addFiles(files, warn);
        }

        public SearchFile addFiles(Iterable<? extends File> files, boolean warn) {
            if (files != null) {
                for (File file : files) {
                    addFile(file, warn);
                }
            }
            return this;
        }

        public SearchFile addFiles(Iterable<? extends File> files) {
            return addFiles(files, warn);
        }

        public void addFile(File file, boolean warn) {
            if (contains(file)) {
                // discard duplicates
                return;
            }

            if (!fsInfo.exists(file)) {
                /* No such file or directory exists */
                if (warn) {
                    log.warning(Lint.LintCategory.PATH,
                                Warnings.PathElementNotFound(file));
                }
                super.add(file);
                return;
            }

            File canonFile = fsInfo.getCanonicalFile(file);
            if (canonicalValues.contains(canonFile)) {
                /* Discard duplicates and avoid infinite recursion */
                return;
            }

            if (fsInfo.isFile(file)) {
                /* File is an ordinary file. */
                if (!"modules".equals(file.getName())) {
                    if (!isArchive(file)) {
                        /* Not a recognized extension; open it to see if
                         it looks like a valid zip file. */
                        try {
                            new ZipFile(file).close();
                            if (warn) {
                                log.warning(Lint.LintCategory.PATH,
                                        Warnings.UnexpectedArchiveFile(file));
                            }
                        } catch (IOException  e) {
                            // FIXME: include e.getLocalizedMessage in warning
                            if (warn) {
                                log.warning(Lint.LintCategory.PATH,
                                        Warnings.InvalidArchiveFile(file));
                            }
                            return;
                        }
                    }
                }
            }

            /* Now what we have left is either a directory or a file name
             conforming to archive naming convention */
            super.add(file);
            canonicalValues.add(canonFile);

            if (expandJarClassFiles && fsInfo.isFile(file) && !file.getPath().endsWith("modules")) {
                addJarClassFile(file, warn);
            }
        }

        // Adds referenced classpath elements from a jar's Class-File
        // Manifest entry.  In some future release, we may want to
        // update this code to recognize URLs rather than simple
        // filenames, but if we do, we should redo all path-related code.
        private void addJarClassFile(File jarFile, boolean warn) {
            try {
                for (File f : fsInfo.getJarClassPath(jarFile)) {
                    addFile(f, warn);
                }
            } catch (IOException e) {
                log.error(Errors.ErrorReadingFile(jarFile, JavacFileManager.getMessage(e)));
            }
        }
    }

    /**
     * Base class for handling support for the representation of Locations.
     *
     * Locations are (by design) opaque handles that can easily be implemented
     * by enums like StandardLocation. Within JavacFileManager, each Location
     * has an associated LocationHandler, which provides much of the appropriate
     * functionality for the corresponding Location.
     *
     * @see #initHandlers
     * @see #getHandler
     */
    protected static abstract class LocationHandler {

        /**
         * @see JavaFileManager#handleOption
         */
        abstract boolean handleOption(Option option, String value);

        /**
         * @see StandardJavaFileManager#hasLocation
         */
        boolean isSet() {
            return (getFiles() != null);
        }

        /**
         * @see StandardJavaFileManager#getLocation
         */
        abstract Collection<File> getFiles();

        /**
         * @see StandardJavaFileManager#setLocation
         */
        abstract void setFiles(Iterable<? extends File> paths) throws IOException;

        /**
         * @see StandardJavaFileManager#setLocationForModule
         */
        abstract void setFilesForModule(String moduleName, Iterable<? extends File> paths)
                throws IOException;

        /**
         * @see JavaFileManager#getLocationForModule(Location, String)
         */
        Location getLocationForModule(String moduleName) throws IOException {
            return null;
        }

        /**
         * @see JavaFileManager#getLocationForModule(Location, JavaFileObject)
         */
        Location getLocationForModule(File file) throws IOException  {
            return null;
        }

        /**
         * @see JavaFileManager#inferModuleName
         */
        String inferModuleName() {
            return null;
        }

        /**
         * @see JavaFileManager#listLocationsForModules
         */
        Iterable<Set<Location>> listLocationsForModules() throws IOException {
            return Collections.emptyList();
        }

        /**
         * @see JavaFileManager#contains
         */
        abstract boolean contains(File file) throws IOException;
    }

    /**
     * A LocationHandler for a given Location, and associated set of options.
     */
    private static abstract class BasicLocationHandler extends LocationHandler {

        final Location location;
        final Set<Option> options;

        /**
         * Create a handler. The location and options provide a way to map from a location or an
         * option to the corresponding handler.
         *
         * @param location the location for which this is the handler
         * @param options the options affecting this location
         * @see #initHandlers
         */
        protected BasicLocationHandler(Location location, Option... options) {
            this.location = location;
            this.options = options.length == 0
                    ? EnumSet.noneOf(Option.class)
                    : EnumSet.copyOf(Arrays.asList(options));
        }

        @Override
        void setFilesForModule(String moduleName, Iterable<? extends File> files) throws IOException {
            // should not happen: protected by check in JavacFileManager
            throw new UnsupportedOperationException("not supported for " + location);
        }

        protected File checkSingletonDirectory(Iterable<? extends File> paths) throws IOException {
            Iterator<? extends File> pathIter = paths.iterator();
            if (!pathIter.hasNext()) {
                throw new IllegalArgumentException("empty path for directory");
            }
            File path = pathIter.next();
            if (pathIter.hasNext()) {
                throw new IllegalArgumentException("path too long for directory");
            }
            checkDirectory(path);
            return path;
        }

        protected File checkDirectory(File path) throws IOException {
            Objects.requireNonNull(path);
            if (!path.exists()) {
                throw new FileNotFoundException(path + ": does not exist");
            }
            if (!path.isDirectory()) {
                throw new IOException(path + ": not a directory");
            }
            return path;
        }
    }

    /**
     * General purpose implementation for output locations, such as -d/CLASS_OUTPUT and
     * -s/SOURCE_OUTPUT. All options are treated as equivalent (i.e. aliases.)
     * The value is a single file, possibly null.
     */
    private class OutputLocationHandler extends BasicLocationHandler {

        private File outputDir;
        private ModuleTable moduleTable;

        OutputLocationHandler(Location location, Option... options) {
            super(location, options);
        }

        @Override
        boolean handleOption(Option option, String value) {
            if (!options.contains(option)) {
                return false;
            }

            // TODO: could/should validate outputDir exists and is a directory
            // need to decide how best to report issue for benefit of
            // direct API call on JavaFileManager.handleOption(specifies IAE)
            // vs. command line decoding.
            outputDir = (value == null) ? null : getFile(value);
            return true;
        }

        @Override
        Collection<File> getFiles() {
            return (outputDir == null) ? null : Collections.singleton(outputDir);
        }

        @Override
        void setFiles(Iterable<? extends File> paths) throws IOException {
            if (paths == null) {
                outputDir = null;
            } else {
                outputDir = checkSingletonDirectory(paths);
            }
            moduleTable = null;
            listed = false;
        }

        @Override
        Location getLocationForModule(String name) {
            if (moduleTable == null) {
                moduleTable = new ModuleTable();
            }
            ModuleLocationHandler l = moduleTable.get(name);
            if (l == null) {
                File out = FileUtils.resolve(outputDir,name);
                l = new ModuleLocationHandler(this, location.getName() + "[" + name + "]",
                        name, Collections.singletonList(out), true);
                moduleTable.add(l);
            }
            return l;
        }

        @Override
        void setFilesForModule(String name, Iterable<? extends File> paths) throws IOException {
            File out = checkSingletonDirectory(paths);
            if (moduleTable == null) {
                moduleTable = new ModuleTable();
            }
            ModuleLocationHandler l = moduleTable.get(name);
            if (l == null) {
                l = new ModuleLocationHandler(this, location.getName() + "[" + name + "]",
                        name, Collections.singletonList(out), true);
                moduleTable.add(l);
           } else {
                l.searchFile = Collections.singletonList(out);
                moduleTable.updateFiles(l);
            }
        }

        @Override
        Location getLocationForModule(File file) {
            return (moduleTable == null) ? null : moduleTable.get(file);
        }

        private boolean listed;

        @Override
        Iterable<Set<Location>> listLocationsForModules() throws IOException {
            if (!listed && outputDir != null) {
                for (File p : outputDir.listFiles()) {
                    getLocationForModule(p.getPath());
                }

                listed = true;
            }

            if (moduleTable == null || moduleTable.isEmpty())
                return Collections.emptySet();

            return Collections.singleton(moduleTable.locations());
        }

        @Override
        boolean contains(File file) throws IOException {
            if (moduleTable != null) {
                return moduleTable.contains(file);
            } else {
                return (outputDir) != null && normalize(file).getPath().startsWith(normalize(outputDir).getPath());
            }
        }
    }

    /**
     * General purpose implementation for search path locations,
     * such as -sourcepath/SOURCE_PATH and -processorFile/ANNOTATION_PROCESSOR_PATH.
     * All options are treated as equivalent (i.e. aliases.)
     * The value is an ordered set of files and/or directories.
     */
    private class SimpleLocationHandler extends BasicLocationHandler {

        protected Collection<File> searchFile;

        SimpleLocationHandler(Location location, Option... options) {
            super(location, options);
        }

        @Override
        boolean handleOption(Option option, String value) {
            if (!options.contains(option)) {
                return false;
            }
            searchFile = value == null ? null
                    : Collections.unmodifiableCollection(createFile().addFiles(value));
            return true;
        }

        @Override
        Collection<File> getFiles() {
            return searchFile;
        }

        @Override
        void setFiles(Iterable<? extends File> files) {
            SearchFile p;
            if (files == null) {
                p = computeFile(null);
            } else {
                p = createFile().addFiles(files);
            }
            searchFile = Collections.unmodifiableCollection(p);
        }

        protected SearchFile computeFile(String value) {
            return createFile().addFiles(value);
        }

        protected SearchFile createFile() {
            return new SearchFile();
        }

        @Override
        boolean contains(File file) throws IOException {
            return Locations.this.contains(searchFile, file);
        }
    }

    /**
     * Subtype of SimpleLocationHandler for -classpath/CLASS_PATH.
     * If no value is given, a default is provided, based on system properties and other values.
     */
    private class ClassFileLocationHandler extends SimpleLocationHandler {

        ClassFileLocationHandler() {
            super(StandardLocation.CLASS_PATH, Option.CLASS_PATH);
        }

        @Override
        Collection<File> getFiles() {
            lazy();
            return searchFile;
        }

        @Override
        protected SearchFile computeFile(String value) {
            String cp = value;



            // Default to current working directory.
            if (cp == null) {
                cp = ".";
            }

            return createFile().addFiles(cp);
        }

        @Override
        protected SearchFile createFile() {
            return new SearchFile()
                    .expandJarClassFiles(true) // Only search user jars for Class-Files
                    .emptyFileDefault(getFile("."));  // Empty path elt ==> current directory
        }

        private void lazy() {
            if (searchFile == null) {
                setFiles(null);
            }
        }
    }

    /**
     * A LocationHander to represent modules found from a module-oriented
     * location such as MODULE_SOURCE_PATH, UPGRADE_MODULE_PATH,
     * SYSTEM_MODULES and MODULE_PATH.
     *
     * The Location can be specified to accept overriding classes from the
     * {@code --patch-module <module>=<path> } parameter.
     */
    private class ModuleLocationHandler extends LocationHandler implements Location {
        private final LocationHandler parent;
        private final String name;
        private final String moduleName;
        private final boolean output;
        boolean explicit;
        Collection<File> searchFile;

        ModuleLocationHandler(LocationHandler parent, String name, String moduleName,
                Collection<File> searchFile, boolean output) {
            this.parent = parent;
            this.name = name;
            this.moduleName = moduleName;
            this.searchFile = searchFile;
            this.output = output;
        }

        @Override @DefinedBy(Api.COMPILER)
        public String getName() {
            return name;
        }

        @Override @DefinedBy(Api.COMPILER)
        public boolean isOutputLocation() {
            return output;
        }

        @Override // defined by LocationHandler
        boolean handleOption(Option option, String value) {
            throw new UnsupportedOperationException();
        }

        @Override // defined by LocationHandler
        Collection<File> getFiles() {
            return Collections.unmodifiableCollection(searchFile);
        }

        @Override // defined by LocationHandler
        void setFiles(Iterable<? extends File> paths) throws IOException {
            // defer to the parent to determine if this is acceptable
            parent.setFilesForModule(moduleName, paths);
        }

        @Override // defined by LocationHandler
        void setFilesForModule(String moduleName, Iterable<? extends File> paths) {
            throw new UnsupportedOperationException("not supported for " + name);
        }

        @Override // defined by LocationHandler
        String inferModuleName() {
            return moduleName;
        }

        @Override
        boolean contains(File file) throws IOException {
            return Locations.this.contains(searchFile, file);
        }

        @Override
        public String toString() {
            return name;
        }
    }

    /**
     * A table of module location handlers, indexed by name and path.
     */
    private class ModuleTable {
        private final Map<String, ModuleLocationHandler> nameMap = new LinkedHashMap<>();
        private final Map<File, ModuleLocationHandler> pathMap = new LinkedHashMap<>();

        void add(ModuleLocationHandler h) {
            nameMap.put(h.moduleName, h);
            for (File p : h.searchFile) {
                pathMap.put(normalize(p), h);
            }
        }

        void updateFiles(ModuleLocationHandler h) {
            // use iterator, to be able to remove old entries
            for (Iterator<Map.Entry<File, ModuleLocationHandler>> iter = pathMap.entrySet().iterator();
                    iter.hasNext(); ) {
                Map.Entry<File, ModuleLocationHandler> e = iter.next();
                if (e.getValue() == h) {
                    iter.remove();
                }
            }
            for (File p : h.searchFile) {
                pathMap.put(normalize(p), h);
            }
        }

        ModuleLocationHandler get(String name) {
            return nameMap.get(name);
        }

        ModuleLocationHandler get(File path) {
            while (path != null) {
                ModuleLocationHandler l = pathMap.get(path);

                if (l != null)
                    return l;

                path = path.getParentFile();
            }

            return null;
        }

        void clear() {
            nameMap.clear();
            pathMap.clear();
        }

        boolean isEmpty() {
            return nameMap.isEmpty();
        }

        boolean contains(File file) throws IOException {
            return Locations.this.contains(pathMap.keySet(), file);
        }

        Set<Location> locations() {
            return Collections.unmodifiableSet(StreamSupport.stream(nameMap.values()).collect(Collectors.toSet()));
        }

        Set<Location> explicitLocations() {
            return Collections.unmodifiableSet(StreamSupport.stream(nameMap.entrySet())
                                                      .filter(e -> e.getValue().explicit)
                                                      .map(e -> e.getValue())
                                                      .collect(Collectors.toSet()));
        }
    }

    /**
     * A LocationHandler for simple module-oriented search paths,
     * like UPGRADE_MODULE_PATH and MODULE_PATH.
     */
    private class ModuleFileLocationHandler extends SimpleLocationHandler {
        private ModuleTable moduleTable;

        ModuleFileLocationHandler(Location location, Option... options) {
            super(location, options);
        }

        @Override
        public boolean handleOption(Option option, String value) {
            if (!options.contains(option)) {
                return false;
            }
            setFiles(value == null ? null : getFileEntries(value));
            return true;
        }

        @Override
        public Location getLocationForModule(String moduleName) {
            initModuleLocations();
            return moduleTable.get(moduleName);
        }

        @Override
        public Location getLocationForModule(File file) {
            initModuleLocations();
            return moduleTable.get(file);
        }

        @Override
        Iterable<Set<Location>> listLocationsForModules() {
            Set<Location> explicitLocations = moduleTable != null ?
                    moduleTable.explicitLocations() : Collections.emptySet();
            Iterable<Set<Location>> explicitLocationsList = !explicitLocations.isEmpty()
                    ? Collections.singletonList(explicitLocations)
                    : Collections.emptyList();

            if (searchFile == null)
                return explicitLocationsList;

            Iterable<Set<Location>> searchFileLocations =
                    () -> new ModuleFileIterator();
            return () -> Iterators.createCompoundIterator(Arrays.asList(explicitLocationsList,
                                                                        searchFileLocations),
                                                          Iterable::iterator);
        }

        @Override
        boolean contains(File file) throws IOException {
            if (moduleTable == null) {
                initModuleLocations();
            }
            return moduleTable.contains(file);
        }

        @Override
        void setFiles(Iterable<? extends File> paths) {
            if (paths != null) {
                for (File p: paths) {
                    checkValidModuleFileEntry(p);
                }
            }
            super.setFiles(paths);
            moduleTable = null;
        }

        @Override
        void setFilesForModule(String name, Iterable<? extends File> paths) throws IOException {
            List<File> checkedFiles = checkFiles(paths);
            // how far should we go to validate the paths provide a module?
            // e.g. contain module-info with the correct name?
            initModuleLocations();
            ModuleLocationHandler l = moduleTable.get(name);
            if (l == null) {
                l = new ModuleLocationHandler(this, location.getName() + "[" + name + "]",
                        name, checkedFiles, true);
                moduleTable.add(l);
           } else {
                l.searchFile = checkedFiles;
                moduleTable.updateFiles(l);
            }
            l.explicit = true;
        }

        private List<File> checkFiles(Iterable<? extends File> paths) throws IOException {
            Objects.requireNonNull(paths);
            List<File> validFiles = new ArrayList<>();
            for (File p : paths) {
                validFiles.add(checkDirectory(p));
            }
            return validFiles;
        }

        private void initModuleLocations() {
            if (moduleTable != null) {
                return;
            }

            moduleTable = new ModuleTable();

            for (Set<Location> set : listLocationsForModules()) {
                for (Location locn : set) {
                    if (locn instanceof ModuleLocationHandler) {
                        ModuleLocationHandler l = (ModuleLocationHandler) locn;
                        if (!moduleTable.nameMap.containsKey(l.moduleName)) {
                            moduleTable.add(l);
                        }
                    }
                }
            }
        }

        private void checkValidModuleFileEntry(File p) {
            if (!p.exists()) {
                // warning may be generated later
                return;
            }

            if (p.isDirectory()) {
                // either an exploded module or a directory of modules
                return;
            }

            String name = p.getPath();
            int lastDot = name.lastIndexOf(".");
            if (lastDot > 0) {
                switch (name.substring(lastDot)) {
                    case ".jar":
                    case ".jmod":
                        return;
                }
            }
            throw new IllegalArgumentException(p.toString());
        }

        class ModuleFileIterator implements Iterator<Set<Location>> {
            Iterator<File> pathIter = searchFile.iterator();
            int pathIndex = 0;
            Set<Location> next = null;

            @Override
            public boolean hasNext() {
                if (next != null)
                    return true;

                while (next == null) {
                    if (pathIter.hasNext()) {
                        File path = pathIter.next();
                        if (path.isDirectory()) {
                            next = scanDirectory(path);
                        } else {
                            next = scanFile(path);
                        }
                        pathIndex++;
                    } else
                        return false;
                }
                return true;
            }

            @Override
            public Set<Location> next() {
                hasNext();
                if (next != null) {
                    Set<Location> result = next;
                    next = null;
                    return result;
                }
                throw new NoSuchElementException();
            }

            private Set<Location> scanDirectory(File path) {
                Set<File> paths = new LinkedHashSet<>();
                File moduleInfoClass = null;
                try  {
                    for (File entry: path.listFiles()) {
                        if (entry.getPath().endsWith("module-info.class")) {
                            moduleInfoClass = entry;
                            break;  // no need to continue scanning
                        }
                        paths.add(entry);
                    }
                } catch ( Exception ignored) {
                    log.error(Errors.LocnCantReadDirectory(path));
                    return Collections.emptySet();
                }

                if (moduleInfoClass != null) {
                    // It's an exploded module directly on the module path.
                    // We can't infer module name from the directory name, so have to
                    // read module-info.class.
                    try {
                        String moduleName = readModuleName(moduleInfoClass);
                        String name = location.getName()
                                + "[" + pathIndex + ":" + moduleName + "]";
                        ModuleLocationHandler l = new ModuleLocationHandler(
                                ModuleFileLocationHandler.this, name, moduleName,
                                Collections.singletonList(path), false);
                        return Collections.singleton(l);
                    } catch (ModuleNameReader.BadClassFile e) {
                        log.error(Errors.LocnBadModuleInfo(path));
                        return Collections.emptySet();
                    } catch (IOException e) {
                        log.error(Errors.LocnCantReadFile(path));
                        return Collections.emptySet();
                    }
                }

                // A directory of modules
                Set<Location> result = new LinkedHashSet<>();
                int index = 0;
                for (File entry : paths) {
                    Pair<String,File> module = inferModuleName(entry);
                    if (module == null) {
                        // diagnostic reported if necessary; skip to next
                        continue;
                    }
                    String moduleName = module.fst;
                    File moduleFile = module.snd;
                    String name = location.getName()
                            + "[" + pathIndex + "." + (index++) + ":" + moduleName + "]";
                    ModuleLocationHandler l = new ModuleLocationHandler(
                            ModuleFileLocationHandler.this, name, moduleName,
                            Collections.singletonList(moduleFile), false);
                    result.add(l);
                }
                return result;
            }

            private Set<Location> scanFile(File path) {
                Pair<String,File> module = inferModuleName(path);
                if (module == null) {
                    // diagnostic reported if necessary
                    return Collections.emptySet();
                }
                String moduleName = module.fst;
                File moduleFile = module.snd;
                String name = location.getName()
                        + "[" + pathIndex + ":" + moduleName + "]";
                ModuleLocationHandler l = new ModuleLocationHandler(
                        ModuleFileLocationHandler.this, name, moduleName,
                        Collections.singletonList(moduleFile), false);
                return Collections.singleton(l);
            }

            private Pair<String,File> inferModuleName(File p) {
                if (p.isDirectory()) {
                    if (FileUtils.resolve(p,"module-info.class").exists()) {
                        String name = p.getName();
                        if (SourceVersion.isName(name))
                            return new Pair<>(name, p);
                    }
                    return null;
                }

                if (p.getPath().endsWith(".jar") && fsInfo.exists(p)) {

                    try  {
                        OrderedZipFile zipFile = new OrderedZipFile(p.getPath());
                        File moduleInfoClass = zipFile.getEntryFile("module-info.class");
                        if (moduleInfoClass.exists()) {
                            String moduleName = readModuleName(moduleInfoClass);
                            return new Pair<>(moduleName, p);
                        }
                        File mf = zipFile.getEntryFile("META-INF/MANIFEST.MF");
                        if (mf.exists()) {
                            try (InputStream in = FileUtils.newInputStream(mf)) {
                                Manifest man = new Manifest(in);
                                Attributes attrs = man.getMainAttributes();
                                if (attrs != null) {
                                    String moduleName = attrs.getValue(new Attributes.Name("Automatic-Module-Name"));
                                    if (moduleName != null) {
                                        if (isModuleName(moduleName)) {
                                            return new Pair<>(moduleName, p);
                                        } else {
                                            log.error(Errors.LocnCantGetModuleNameForJar(p));
                                            return null;
                                        }
                                    }
                                }
                            }
                        }
                    } catch (ModuleNameReader.BadClassFile e) {
                        log.error(Errors.LocnBadModuleInfo(p));
                        return null;
                    } catch (IOException e) {
                        log.error(Errors.LocnCantReadFile(p));
                        return null;
                    }

                    //automatic module:
                    String fn = p.getName();
                    //from ModuleFile.deriveModuleDescriptor:

                    // drop .jar
                    String mn = fn.substring(0, fn.length()-4);

                    // find first occurrence of -${NUMBER}. or -${NUMBER}$
                    Matcher matcher = Pattern.compile("-(\\d+(\\.|$))").matcher(mn);
                    if (matcher.find()) {
                        int start = matcher.start();

                        mn = mn.substring(0, start);
                    }

                    // finally clean up the module name
                    mn =  mn.replaceAll("[^A-Za-z0-9]", ".")  // replace non-alphanumeric
                            .replaceAll("(\\.)(\\1)+", ".")   // collapse repeating dots
                            .replaceAll("^\\.", "")           // drop leading dots
                            .replaceAll("\\.$", "");          // drop trailing dots


                    if (!mn.isEmpty()) {
                        return new Pair<>(mn, p);
                    }

                    log.error(Errors.LocnCantGetModuleNameForJar(p));
                    return null;
                }

                if (warn && false) {  // temp disable, when enabled, massage examples.not-yet.txt suitably.
                    log.warning(Warnings.LocnUnknownFileOnModulePath(p));
                }
                return null;
            }

            private String readModuleName(File path) throws IOException, ModuleNameReader.BadClassFile {
                if (moduleNameReader == null)
                    moduleNameReader = new ModuleNameReader();
                return moduleNameReader.readModuleName(path);
            }
        }

        //from jdk.internal.module.Checks:
        /**
         * Returns {@code true} if the given name is a legal module name.
         */
        private boolean isModuleName(String name) {
            int next;
            int off = 0;
            while ((next = name.indexOf('.', off)) != -1) {
                String id = name.substring(off, next);
                if (!SourceVersion.isName(id))
                    return false;
                off = next+1;
            }
            String last = name.substring(off);
            return SourceVersion.isName(last);
        }
    }

    private class ModuleSourceFileLocationHandler extends BasicLocationHandler {
        private ModuleTable moduleTable;
        private List<File> paths;

        ModuleSourceFileLocationHandler() {
            super(StandardLocation.MODULE_SOURCE_PATH,
                    Option.MODULE_SOURCE_PATH);
        }

        @Override
        boolean handleOption(Option option, String value) {
            init(value);
            return true;
        }

        void init(String value) {
            Collection<String> segments = new ArrayList<>();
            for (String s: value.split(File.pathSeparator)) {
                expandBraces(s, segments);
            }

            Map<String, List<File>> map = new LinkedHashMap<>();
            List<File> noSuffixFiles = new ArrayList<>();
            boolean anySuffix = false;
            final String MARKER = "*";
            for (String seg: segments) {
                int markStart = seg.indexOf(MARKER);
                if (markStart == -1) {
                    File p = getFile(seg);
                    add(map, p, null);
                    noSuffixFiles.add(p);
                } else {
                    if (markStart == 0 || !isSeparator(seg.charAt(markStart - 1))) {
                        throw new IllegalArgumentException("illegal use of " + MARKER + " in " + seg);
                    }
                    File prefix = getFile(seg.substring(0, markStart - 1));
                    File suffix;
                    int markEnd = markStart + MARKER.length();
                    if (markEnd == seg.length()) {
                        suffix = null;
                    } else if (!isSeparator(seg.charAt(markEnd))
                            || seg.indexOf(MARKER, markEnd) != -1) {
                        throw new IllegalArgumentException("illegal use of " + MARKER + " in " + seg);
                    } else {
                        suffix = getFile(seg.substring(markEnd + 1));
                        anySuffix = true;
                    }
                    add(map, prefix, suffix);
                    if (suffix == null) {
                        noSuffixFiles.add(prefix);
                    }
                }
            }

            initModuleTable(map);
            paths = anySuffix ? null : noSuffixFiles;
        }

        private void initModuleTable(Map<String, List<File>> map) {
            moduleTable = new ModuleTable();
            for(Map.Entry<String,List<File>> entry:map.entrySet()) {
                String modName=entry.getKey();
                List<File> modFile=entry.getValue();
                boolean hasModuleInfo = StreamSupport.stream(modFile).anyMatch(checkModuleInfo);
                if (hasModuleInfo) {
                    String locnName = location.getName() + "[" + modName + "]";
                    ModuleLocationHandler l = new ModuleLocationHandler(this, locnName, modName,
                            modFile, false);
                    moduleTable.add(l);
                }
            }
        }
        //where:
            private final Predicate<File> checkModuleInfo =
                    p -> FileUtils.resolve(p,"module-info.java").exists();


        private boolean isSeparator(char ch) {
            // allow both separators on Windows
            return (ch == File.separatorChar) || (ch == '/');
        }

        void add(Map<String, List<File>> map, File prefix, File suffix) {
            if (!prefix.isDirectory()) {
                if (warn) {
                    String key = prefix.exists()
                            ? "dir.path.element.not.directory"
                            : "dir.path.element.not.found";
                    log.warning(Lint.LintCategory.PATH, key, prefix);
                }
                return;
            }

                for (File entry: prefix.listFiles()) {
                    if(!entry.isDirectory()) continue;
                    File path = (suffix == null) ? entry : FileUtils.resolve(entry,suffix.getPath());
                    if (path.isDirectory()) {
                        String name = entry.getName();
                        List<File> paths = map.get(name);
                        if (paths == null)
                            map.put(name, paths = new ArrayList<>());
                        paths.add(path);
                    }
                }
        }

        private void expandBraces(String value, Collection<String> results) {
            int depth = 0;
            int start = -1;
            String prefix = null;
            String suffix = null;
            for (int i = 0; i < value.length(); i++) {
                switch (value.charAt(i)) {
                    case '{':
                        depth++;
                        if (depth == 1) {
                            prefix = value.substring(0, i);
                            suffix = value.substring(getMatchingBrace(value, i) + 1);
                            start = i + 1;
                        }
                        break;

                    case ',':
                        if (depth == 1) {
                            String elem = value.substring(start, i);
                            expandBraces(prefix + elem + suffix, results);
                            start = i + 1;
                        }
                        break;

                    case '}':
                        switch (depth) {
                            case 0:
                                throw new IllegalArgumentException("mismatched braces");

                            case 1:
                                String elem = value.substring(start, i);
                                expandBraces(prefix + elem + suffix, results);
                                return;

                            default:
                                depth--;
                        }
                        break;
                }
            }
            if (depth > 0)
                throw new IllegalArgumentException("mismatched braces");
            results.add(value);
        }

        int getMatchingBrace(String value, int offset) {
            int depth = 1;
            for (int i = offset + 1; i < value.length(); i++) {
                switch (value.charAt(i)) {
                    case '{':
                        depth++;
                        break;

                    case '}':
                        if (--depth == 0)
                            return i;
                        break;
                }
            }
            throw new IllegalArgumentException("mismatched braces");
        }

        @Override
        boolean isSet() {
            return (moduleTable != null);
        }

        @Override
        Collection<File> getFiles() {
            if (paths == null) {
                // This may occur for a complex setting with --module-source-path option
                // i.e. one that cannot be represented by a simple series of paths.
                throw new IllegalStateException("paths not available");
            }
            return paths;
        }

        @Override
        void setFiles(Iterable<? extends File> files) throws IOException {
            Map<String, List<File>> map = new LinkedHashMap<>();
            List<File> newFiles = new ArrayList<>();
            for (File file : files) {
                add(map, file, null);
                newFiles.add(file);
            }

            initModuleTable(map);
            paths = Collections.unmodifiableList(newFiles);
        }

        @Override
        void setFilesForModule(String name, Iterable<? extends File> paths) throws IOException {
            List<File> validFiles = checkFiles(paths);

            if (moduleTable == null)
                moduleTable = new ModuleTable();

            ModuleLocationHandler l = moduleTable.get(name);
            if (l == null) {
                l = new ModuleLocationHandler(this,
                        location.getName() + "[" + name + "]",
                        name,
                        validFiles,
                        true);
                moduleTable.add(l);
           } else {
                l.searchFile = validFiles;
                moduleTable.updateFiles(l);
            }
        }

        private List<File> checkFiles(Iterable<? extends File> paths) throws IOException {
            Objects.requireNonNull(paths);
            List<File> validFiles = new ArrayList<>();
            for (File p : paths) {
                validFiles.add(checkDirectory(p));
            }
            return validFiles;
        }

        @Override
        Location getLocationForModule(String name) {
            return (moduleTable == null) ? null : moduleTable.get(name);
        }

        @Override
        Location getLocationForModule(File file) {
            return (moduleTable == null) ? null : moduleTable.get(file);
        }

        @Override
        Iterable<Set<Location>> listLocationsForModules() {
            if (moduleTable == null)
                return Collections.emptySet();

            return Collections.singleton(moduleTable.locations());
        }

        @Override
        boolean contains(File file) throws IOException {
            return (moduleTable == null) ? false : moduleTable.contains(file);
        }

    }

    private class PatchModulesLocationHandler extends BasicLocationHandler {
        private final ModuleTable moduleTable = new ModuleTable();

        PatchModulesLocationHandler() {
            super(StandardLocation.PATCH_MODULE_PATH, Option.PATCH_MODULE);
        }

        @Override
        boolean handleOption(Option option, String value) {
            if (!options.contains(option)) {
                return false;
            }

            moduleTable.clear();

            // Allow an extended syntax for --patch-module consisting of a series
            // of values separated by NULL characters. This is to facilitate
            // supporting deferred file manager options on the command line.
            // See Option.PATCH_MODULE for the code that composes these multiple
            // values.
            for (String v : value.split("\0")) {
                int eq = v.indexOf('=');
                if (eq > 0) {
                    String moduleName = v.substring(0, eq);
                    SearchFile mPatchFile = new SearchFile()
                            .addFiles(v.substring(eq + 1));
                    String name = location.getName() + "[" + moduleName + "]";
                    ModuleLocationHandler h = new ModuleLocationHandler(this, name,
                            moduleName, mPatchFile, false);
                    moduleTable.add(h);
                } else {
                    // Should not be able to get here;
                    // this should be caught and handled in Option.PATCH_MODULE
                    log.error(Errors.LocnInvalidArgForXpatch(value));
                }
            }

            return true;
        }

        @Override
        boolean isSet() {
            return !moduleTable.isEmpty();
        }

        @Override
        Collection<File> getFiles() {
            throw new UnsupportedOperationException();
        }

        @Override
        void setFiles(Iterable<? extends File> files) throws IOException {
            throw new UnsupportedOperationException();
        }

        @Override // defined by LocationHandler
        void setFilesForModule(String moduleName, Iterable<? extends File> files) throws IOException {
            throw new UnsupportedOperationException(); // not yet
        }

        @Override
        Location getLocationForModule(String name) throws IOException {
            return moduleTable.get(name);
        }

        @Override
        Location getLocationForModule(File file) throws IOException {
            return moduleTable.get(file);
        }

        @Override
        Iterable<Set<Location>> listLocationsForModules() throws IOException {
            return Collections.singleton(moduleTable.locations());
        }

        @Override
        boolean contains(File file) throws IOException {
            return moduleTable.contains(file);
        }
    }

    /**
     * Split a search path into its elements. Empty path elements will be ignored.
     *
     * @param searchPath The search path to be split
     * @return The elements of the path
     */
    private Iterable<File> getPathEntries(String searchPath) {
        return getPathEntries(searchPath, null);
    }

    /**
     * Split a search path into its elements. If emptyPathDefault is not null, all empty elements in the
     * path, including empty elements at either end of the path, will be replaced with the value of
     * emptyPathDefault.
     *
     * @param searchPath The search path to be split
     * @param emptyPathDefault The value to substitute for empty path elements, or null, to ignore
     * empty path elements
     * @return The elements of the path
     */
    private Iterable<File> getPathEntries(String searchPath, File emptyPathDefault) {
        ListBuffer<File> entries = new ListBuffer<>();
        for (String s: searchPath.split(Pattern.quote(File.pathSeparator), -1)) {
            if (s.isEmpty()) {
                if (emptyPathDefault != null) {
                    entries.add(emptyPathDefault);
                }
            } else {
                try {
                    entries.add(new File(s));
                } catch (IllegalArgumentException e) {
                    if (warn) {
                        log.warning(LintCategory.PATH, Warnings.InvalidPath(s));
                    }
                }
            }
        }
        return entries;
    }
    /**
     * Utility class to help evaluate a path option. Duplicate entries are ignored, jar class paths
     * can be expanded.
     */
    private class SearchPath extends LinkedHashSet<File> {

        private static final long serialVersionUID = 0;

        private boolean expandJarClassPaths = false;
        private final Set<File> canonicalValues = new HashSet<>();

        public SearchPath expandJarClassPaths(boolean x) {
            expandJarClassPaths = x;
            return this;
        }

        /**
         * What to use when path element is the empty string
         */
        private File emptyPathDefault = null;

        public SearchPath emptyPathDefault(File x) {
            emptyPathDefault = x;
            return this;
        }

        public SearchPath addDirectories(String dirs, boolean warn) {
            boolean prev = expandJarClassPaths;
            expandJarClassPaths = true;
            try {
                if (dirs != null) {
                    for (File dir : getPathEntries(dirs)) {
                        addDirectory(dir, warn);
                    }
                }
                return this;
            } finally {
                expandJarClassPaths = prev;
            }
        }

        public SearchPath addDirectories(String dirs) {
            return addDirectories(dirs, warn);
        }

        private void addDirectory(File dir, boolean warn) {
            if (!dir.isDirectory()) {
                if (warn) {
                    log.warning(Lint.LintCategory.PATH,
                            Warnings.DirPathElementNotFound(dir));
                }
                return;
            }

            for (File dirEntry: dir.listFiles(Locations.this::isArchive)) {
                addFile(dirEntry, warn);
            }
        }

        public SearchPath addFiles(String files, boolean warn) {
            if (files != null) {
                addFiles(getPathEntries(files, emptyPathDefault), warn);
            }
            return this;
        }

        public SearchPath addFiles(String files) {
            return addFiles(files, warn);
        }

        public SearchPath addFiles(Iterable<? extends File> files, boolean warn) {
            if (files != null) {
                for (File file : files) {
                    addFile(file, warn);
                }
            }
            return this;
        }

        public SearchPath addFiles(Iterable<? extends File> files) {
            return addFiles(files, warn);
        }

        public void addFile(File file, boolean warn) {
            if (contains(file)) {
                // discard duplicates
                return;
            }

            if (!fsInfo.exists(file)) {
                /* No such file or directory exists */
                if (warn) {
                    log.warning(Lint.LintCategory.PATH,
                            Warnings.PathElementNotFound(file));
                }
                super.add(file);
                return;
            }

            File canonFile = fsInfo.getCanonicalFile(file);
            if (canonicalValues.contains(canonFile)) {
                /* Discard duplicates and avoid infinite recursion */
                return;
            }


            /* Now what we have left is either a directory or a file name
             conforming to archive naming convention */
            super.add(file);
            canonicalValues.add(canonFile);

            if (expandJarClassPaths && fsInfo.isFile(file) ) {
                addJarClassPath(file, warn);
            }
        }

        // Adds referenced classpath elements from a jar's Class-Path
        // Manifest entry.  In some future release, we may want to
        // update this code to recognize URLs rather than simple
        // filenames, but if we do, we should redo all path-related code.
        private void addJarClassPath(File jarFile, boolean warn) {
            try {
                for (File f : fsInfo.getJarClassPath(jarFile)) {
                    addFile(f, warn);
                }
            } catch (IOException e) {
                log.error(Errors.ErrorReadingFile(jarFile, JavacFileManager.getMessage(e)));
            }
        }
    }

    /**
     * Custom subtype of LocationHandler for PLATFORM_CLASS_PATH.
     * Various options are supported for different components of the
     * platform class path.
     * Setting a value with setLocation overrides all existing option values.
     * Setting any option overrides any value set with setLocation, and
     * reverts to using default values for options that have not been set.
     * Setting -bootclasspath or -Xbootclasspath overrides any existing
     * value for -Xbootclasspath/p: and -Xbootclasspath/a:.
     */
    private class BootClassPathLocationHandler extends BasicLocationHandler implements Location{

        private Collection<File> searchPath;
        final Map<Option, String> optionValues = new EnumMap<>(Option.class);

        /**
         * Is the bootclasspath the default?
         */
        private boolean isDefault;

        BootClassPathLocationHandler(StandardLocation location) {
            super(location,
                    BOOT_CLASS_PATH, Option.XBOOTCLASSPATH,
                    XBOOTCLASSPATH_PREPEND,
                    XBOOTCLASSPATH_APPEND,
                    ENDORSEDDIRS, Option.DJAVA_ENDORSED_DIRS,
                    EXTDIRS, Option.DJAVA_EXT_DIRS);
        }

        boolean isDefault() {
            lazy();
            return isDefault;
        }

        @Override
        boolean handleOption(Option option, String value) {
            if (!options.contains(option)) {
                return false;
            }


            option = canonicalize(option);
            optionValues.put(option, value);
            if (option == BOOT_CLASS_PATH) {
                optionValues.remove(XBOOTCLASSPATH_PREPEND);
                optionValues.remove(XBOOTCLASSPATH_APPEND);
            }
            searchPath = null;  // reset to "uninitialized"
            return true;
        }
        // where
        // TODO: would be better if option aliasing was handled at a higher
        // level
        private Option canonicalize(Option option) {
            switch (option) {
                case XBOOTCLASSPATH:
                    return BOOT_CLASS_PATH;
                case DJAVA_ENDORSED_DIRS:
                    return ENDORSEDDIRS;
                case DJAVA_EXT_DIRS:
                    return EXTDIRS;
                default:
                    return option;
            }
        }

        @Override
        Collection<File> getFiles() {
            lazy();
            return searchPath;
        }

        @Override
        void setFiles(Iterable<? extends File> files) {
            if (files == null) {
                searchPath = null;  // reset to "uninitialized"
            } else {
                isDefault = false;
                SearchPath p = new SearchPath().addFiles(files, false);
                searchPath = Collections.unmodifiableCollection(p);
                optionValues.clear();
            }
        }

        @Override
        String inferModuleName() {
            return "java.base";
        }

        @Override
        Iterable<Set<Location>> listLocationsForModules() throws IOException {
            return Collections.singleton(Collections.singleton(this));
        }

        @Override
        Location getLocationForModule(String moduleName) throws IOException {
            if(moduleName.equals("java.base"))
                return this;
            return super.getLocationForModule(moduleName);
        }

        @Override
        Location getLocationForModule(File file) throws IOException {
            if(BOOT_LOCATION.equals(file))
                return this;
            return super.getLocationForModule(file);
        }

        SearchPath computePath() throws IOException {
            SearchPath path = new SearchPath();

            String bootclasspathOpt = optionValues.get(BOOT_CLASS_PATH);
            String endorseddirsOpt = optionValues.get(ENDORSEDDIRS);
            String extdirsOpt = optionValues.get(EXTDIRS);
            String xbootclasspathPrependOpt = optionValues.get(XBOOTCLASSPATH_PREPEND);
            String xbootclasspathAppendOpt = optionValues.get(XBOOTCLASSPATH_APPEND);
            path.addFiles(xbootclasspathPrependOpt);

            if (endorseddirsOpt != null) {
                path.addDirectories(endorseddirsOpt);
            } else {
                path.addDirectories(System.getProperty("java.endorsed.dirs"), false);
            }

            if (bootclasspathOpt != null) {
                path.addFiles(bootclasspathOpt);
            }else path.addFile(BOOT_LOCATION,false);


            path.addFiles(xbootclasspathAppendOpt);

            // Strictly speaking, standard extensions are not bootstrap
            // classes, but we treat them identically, so we'll pretend
            // that they are.
            if (extdirsOpt != null) {
                path.addDirectories(extdirsOpt);
            }

            isDefault =
                    (xbootclasspathPrependOpt == null)
                            && (bootclasspathOpt == null)
                            && (xbootclasspathAppendOpt == null);

            return path;
        }



        private void lazy() {
            if (searchPath == null) {
                try {
                    searchPath = Collections.unmodifiableCollection(computePath());
                } catch (IOException e) {
                    // TODO: need better handling here, e.g. javac Abort?
                    throw new RuntimeException(e);
                }
            }
        }

        @Override
        boolean contains(File file) throws IOException {
            return Locations.this.contains(searchPath, file);
        }

        @Override
        public String getName() {
            return BOOT_LOCATION.getPath();
        }

        @Override
        public boolean isOutputLocation() {
            return false;
        }
    }

    Map<Location, LocationHandler> handlersForLocation;
    Map<Option, LocationHandler> handlersForOption;

    void initHandlers() {
        handlersForLocation = new HashMap<>();
        handlersForOption = new EnumMap<>(Option.class);

        BasicLocationHandler[] handlers = {
            new BootClassPathLocationHandler(StandardLocation.PLATFORM_CLASS_PATH),
            new BootClassPathLocationHandler(StandardLocation.SYSTEM_MODULES),
            new ClassFileLocationHandler(),
            new SimpleLocationHandler(StandardLocation.SOURCE_PATH, Option.SOURCE_PATH),
            new SimpleLocationHandler(StandardLocation.ANNOTATION_PROCESSOR_PATH, Option.PROCESSOR_PATH),
            new SimpleLocationHandler(StandardLocation.ANNOTATION_PROCESSOR_MODULE_PATH, Option.PROCESSOR_MODULE_PATH),
            new OutputLocationHandler(StandardLocation.CLASS_OUTPUT, Option.D),
            new OutputLocationHandler(StandardLocation.SOURCE_OUTPUT, Option.S),
            new OutputLocationHandler(StandardLocation.NATIVE_HEADER_OUTPUT, Option.H),
            new ModuleSourceFileLocationHandler(),
            new PatchModulesLocationHandler(),
            new ModuleFileLocationHandler(StandardLocation.UPGRADE_MODULE_PATH, Option.UPGRADE_MODULE_PATH),
            new ModuleFileLocationHandler(StandardLocation.MODULE_PATH, Option.MODULE_PATH)
        };

        for (BasicLocationHandler h : handlers) {
            handlersForLocation.put(h.location, h);
            for (Option o : h.options) {
                handlersForOption.put(o, h);
            }
        }
    }

    boolean handleOption(Option option, String value) {
        LocationHandler h = handlersForOption.get(option);
        return (h == null ? false : h.handleOption(option, value));
    }

    boolean hasLocation(Location location) {
        LocationHandler h = getHandler(location);
        return (h == null ? false : h.isSet());
    }

    Collection<File> getLocation(Location location) {
        LocationHandler h = getHandler(location);
        return (h == null ? null : h.getFiles());
    }

    File getOutputLocation(Location location) {
        if (!location.isOutputLocation()) {
            throw new IllegalArgumentException();
        }
        LocationHandler h = getHandler(location);
        return ((OutputLocationHandler) h).outputDir;
    }

    void setLocation(Location location, Iterable<? extends File> files) throws IOException {
        LocationHandler h = getHandler(location);
        if (h == null) {
            if (location.isOutputLocation()) {
                h = new OutputLocationHandler(location);
            } else {
                h = new SimpleLocationHandler(location);
            }
            handlersForLocation.put(location, h);
        }
        h.setFiles(files);
    }

    Location getLocationForModule(Location location, String name) throws IOException {
        LocationHandler h = getHandler(location);
        return (h == null ? null : h.getLocationForModule(name));
    }

    Location getLocationForModule(Location location, File file) throws IOException {
        LocationHandler h = getHandler(location);
        return (h == null ? null : h.getLocationForModule(file));
    }

    void setLocationForModule(Location location, String moduleName,
            Iterable<? extends File> files) throws IOException {
        LocationHandler h = getHandler(location);
        if (h == null) {
            if (location.isOutputLocation()) {
                h = new OutputLocationHandler(location);
            } else {
                h = new ModuleFileLocationHandler(location);
            }
            handlersForLocation.put(location, h);
        }
        h.setFilesForModule(moduleName, files);
    }

    String inferModuleName(Location location) {
        LocationHandler h = getHandler(location);
        return (h == null ? null : h.inferModuleName());
    }

    Iterable<Set<Location>> listLocationsForModules(Location location) throws IOException {
        LocationHandler h = getHandler(location);
        return (h == null ? null : h.listLocationsForModules());
    }

    boolean contains(Location location, File file) throws IOException {
        LocationHandler h = getHandler(location);
        if (h == null)
            throw new IllegalArgumentException("unknown location");
        return h.contains(file);
    }

    protected LocationHandler getHandler(Location location) {
        Objects.requireNonNull(location);
        return (location instanceof LocationHandler)
                ? (LocationHandler) location
                : handlersForLocation.get(location);
    }

    /**
     * Is this the name of an archive file?
     */
    private boolean isArchive(File file) {
        String n = StringUtils.toLowerCase(file.getName());
        return fsInfo.isFile(file)
                && (n.endsWith(".jar") || n.endsWith(".zip"));
    }

    static File normalize(File p) {
        try {
            return p.getCanonicalFile();
        } catch (IOException e) {
            return p.getAbsoluteFile();
        }
    }
}
