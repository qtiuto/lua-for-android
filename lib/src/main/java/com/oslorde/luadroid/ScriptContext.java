package com.oslorde.luadroid;


import android.os.Build;
import android.util.*;
import com.android.dx.TypeId;
import com.android.dx.stock.ProxyBuilder;
import dalvik.system.BaseDexClassLoader;
import dalvik.system.DexFile;
import org.json.JSONObject;

import java.io.File;
import java.io.OutputStream;
import java.lang.ref.WeakReference;
import java.lang.reflect.*;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

/**
 * For running a lua context
 */
public class ScriptContext implements GCTracker {
    private static final int JAVA_CHAR = 0;
    private static final int JAVA_BOOLEAN = 1;
    private static final int JAVA_INTEGER = 2;
    private static final int JAVA_DOUBLE = 3;
    private static final int JAVA_VOID = 4;
    private static final int JAVA_OBJECT = 5;

    private static final int CLASS_BYTE=0;
    private static final int CLASS_SHORT=1;
    private static final int CLASS_INT=2;
    private static final int CLASS_LONG=3;
    private static final int CLASS_FLOAT=4;
    private static final int CLASS_DOUBLE=5;
    private static final int CLASS_CHAR=6;
    private static final int CLASS_BOOLEAN=7;
    private static final int CLASS_VOID=8;
    private static final int CLASS_B_BYTE=9;
    private static final int CLASS_B_SHORT=10;
    private static final int CLASS_B_INT=11;
    private static final int CLASS_B_LONG=12;
    private static final int CLASS_B_FLOAT=13;
    private static final int CLASS_B_DOUBLE=14;
    private static final int CLASS_B_CHAR=15;
    private static final int CLASS_B_BOOLEAN=16;

    static {
    }

    private final Object importLock=new Object();
    private HashMap<Class, TableConverter> sConverters;
    private HashMap<Class,Indexer> indexers;
    private HashMap<Class,IteratorFactory> iterators;
    private long nativePtr;
    private OutputStream outLogger;
    private OutputStream errLogger;
    private WeakReference<GCListener> gcListener;
    //Too many memory usages.
    private Set<String> dexFiles;
    private Map<String,List<String>> packages;

    public ScriptContext() {
        this(true, false);
    }

    public ScriptContext(boolean importAllFunctions) {
        this(importAllFunctions, false);
    }

    /**
     * @param importAllFunctions whether to use short name
     * @param localFunction whether functions to be use only in current thread
     */
    public ScriptContext(boolean importAllFunctions, boolean localFunction) {
        nativePtr = nativeOpen(importAllFunctions, localFunction);
        gcListener = new WeakReference<>(new GCListener(new WeakReference<>(this)));
    }

    private native static synchronized void nativeClose(long ptr);

    private native static void nativeClean(long ptr);

    private static native void registerLogger(long ptr, OutputStream out, OutputStream err);

    private static native Object[] runScript(long ptr, Object s, boolean isFile,
                                             Object... args) throws RuntimeException;

    private static native long compile(long ptr, String s, boolean isFile) throws RuntimeException;

    private static native void addObject(long ptr, String s, Object ob, boolean local);

    private static native void addMethod(long ptr, String name, String method, Object instOrType, boolean local);

    private static native Object invokeLuaFunction(long ptr, boolean isInterface,
                                                   long funcRef, Object proxy, int[] types, Object[] args);

    private static native Object constructChild(long ptr, Class proxyClass, long nativeInfo);

    private native static void releaseFunc(long ptr);

    private static native int getClassType(long ptr,Class c);

    /**
     * change a lua function to a functional interface instance
     */
    @SuppressWarnings("unchecked")
    public static <T> T changeCallSite(LuaFunction function, Class<T> target) {
        if (target == LuaFunction.class)
            return (T) function;
        InvokeHandler handler= (InvokeHandler) Proxy.getInvocationHandler(function);
        handler.changeCallSite(target);
        return (T) Proxy.newProxyInstance(target.getClassLoader(), new Class[]{target}, handler);

    }

    private static Method findMethodNoExcept(Class<?> origClazz, String name,
                                              Class<?>... parameterTypes){
        for (Class<?> clazz=origClazz; clazz != null; clazz = clazz
                .getSuperclass()) {
            try {
                Method method = clazz.getDeclaredMethod(name, parameterTypes);
                if (!method.isAccessible()) {
                    method.setAccessible(true);
                }
                return method;
            } catch (NoSuchMethodException e) {
                // ignore and search next
            }
        }
        return null;
    }

    private static Method findIndexerMethod(Class<?> origClazz, String name,Class keyType,int argLen){
        for (Class<?> clazz=origClazz; clazz != null; clazz = clazz
                .getSuperclass()) {
            Method[] methods = clazz.getDeclaredMethods();
            Method best=null;
            int score=-1;
            for (Method method:
                    methods) {
                if(!method.getName().equals(name))
                    continue;
                Class<?>[] parameterTypes = method.getParameterTypes();
                if(parameterTypes.length!=argLen||!parameterTypes[0].isAssignableFrom(keyType)){
                    if(keyType!=Long.class||(parameterTypes[0]!=int.class&&parameterTypes[0]!=long.class))
                    continue;
                }
                int curScore=0;
                if(parameterTypes[0]==Object.class)
                    curScore=1;
                if(argLen==2&&parameterTypes[1]==Object.class)
                    curScore++;

                if(curScore>score){
                    best=method;
                    score=curScore;
                }
            }
            if(best!=null){
                if (!best.isAccessible()) {
                    best.setAccessible(true);
                }
                return best;
            }
        }
        return null;
    }

    private static Method getSingleInterface(Class cl) {
        if (cl.isInterface()) {
            Method[] methods = cl.getMethods();
            int count = 0;
            Method ret = null;
            for (Method method : methods) {
                if (Modifier.isAbstract(method.getModifiers())) {
                    count++;
                    ret = method;
                    if (count > 1) {
                        ret = null;
                        break;
                    }
                }
            }
            return ret;
        }
        return null;
    }

    private static Class[] getParameterTypes(Object ob) {
        if (ob instanceof Method) return ((Method) ob).getParameterTypes();
        else if (ob instanceof Constructor) return ((Constructor) ob).getParameterTypes();
        return null;
    }

    private static int[] generateParamTypes(Class<?>[] paramTypes) {
        int len = paramTypes.length;
        int[] ret = new int[len];
        for (int i = len - 1; i >= 0; --i) {
            Class type = paramTypes[i];
            ret[i] = getClassLuaType(type);
        }
        return ret;
    }

    private static Object[] findMembers(Class cl, String name, boolean isField, boolean isStatic) {
        if (isField) {
            ArrayList<Object> retList = new ArrayList<>();
            Set<Class> fieldSet = new HashSet<>();
            checkAndAddFields(name, isStatic, retList, fieldSet, cl.getFields());
            do {
                checkAndAddFields(name, isStatic, retList, fieldSet, cl.getDeclaredFields());
                cl = cl.getSuperclass();
            } while (cl != null);
            return retList.toArray();
        }
        if (name.equals("<init>")&&!isStatic) {
            return convertConstructors(cl.getDeclaredConstructors());
        }
        ArrayList<Object> retList = new ArrayList<>();
        Set<MethodSetEntry> methodSet = new HashSet<>();
        checkAndAddMethods(name, isStatic, retList, methodSet, cl.getMethods());
        if (!isStatic) {
            if (cl.isInterface() || cl.isArray()) {
                checkAndAddMethods(name, false, retList, methodSet, Object.class.getMethods());
            }
        }
        do {
            checkAndAddMethods(name, isStatic, retList, methodSet, cl.getDeclaredMethods());
            cl = cl.getSuperclass();
        } while (cl != null);

        return retList.isEmpty() ? null : retList.toArray();
    }

    private static Object[] convertConstructors(Constructor[] constructors) {
        Object[] ret=new Object[constructors.length*5];
        int i=0;
        for (Constructor m : constructors) {
            ret[i++]=m;
            ret[i++]=void.class;
            ret[i++]=null;
            Class[] parameterTypes = m.getParameterTypes();
            ret[i++]= parameterTypes;
            Type[] genericParameterTypes = m.getGenericParameterTypes();
            for (int j=genericParameterTypes.length-1;j>=0;--j){
                if(genericParameterTypes[j]==parameterTypes[j])
                    genericParameterTypes[j]=null;
            }
            ret[i++]= genericParameterTypes;
        }
        return ret;
    }

    private static void checkAndAddMethods(
            String name, boolean isStatic, ArrayList<Object> retList,
            Set<MethodSetEntry> methodSet, Method[] methods) {
        for (Method m : methods) {
            if (isStatic != Modifier.isStatic(m.getModifiers()))
                continue;
            if (!name.equals(m.getName())) continue;
            if (methodSet.add(new MethodSetEntry(m))) {
                retList.add(m);
                Class<?> returnType = m.getReturnType();
                retList.add(returnType);
                Type genericReturnType = m.getGenericReturnType();
                retList.add(genericReturnType==returnType?null:returnType);
                Class<?>[] parameterTypes = m.getParameterTypes();
                retList.add(parameterTypes);
                Type[] genericParameterTypes = m.getGenericParameterTypes();
                for (int i=genericParameterTypes.length-1;i>=0;--i){
                    if(genericParameterTypes[i]==parameterTypes[i])
                        genericParameterTypes[i]=null;
                }
                retList.add(genericParameterTypes);
            }
        }
    }

    private static void checkAndAddFields(
            String name, boolean isStatic, ArrayList<Object> retList,
            Set<Class> fieldSet, Field[] fields) {
        for (Field f : fields) {
            if (isStatic != Modifier.isStatic(f.getModifiers()))
                continue;
            if (!name.equals(f.getName())) continue;
            Class<?> type = f.getType();
            if (!fieldSet.contains(type)) {
                retList.add(f);
                retList.add(type);
                Type genericType = f.getGenericType();
                retList.add(genericType==type?null:genericType);
                fieldSet.add(type);
            }
        }
    }

    private static String getPackage(String cl){
        int index=cl.lastIndexOf('.');
        if(index==-1) return "";
        return cl.substring(0,index);
    }

    /** 1-5 is left for box type,6 left for object,interfaces is always 7*/
    private static int weightObject(Class<?> target, Class<?> from) {
        if (target.isPrimitive() || from.isPrimitive()) {
            if(target==from) return 7;//left for primitive array
            else return 0;
        }
        if (target == Object.class) return 6;
        if (target.isInterface() && target.isAssignableFrom(from)) {
            return 7;
        }
        if (target.isArray()) {
            if (from.isArray())
                return weightObject(target.getComponentType(), from.getComponentType());
            else return 0;
        } else if (from.isArray()) return 0;
        int weight = 1;
        int ret = 0;
        while (from != null) {
            if (from == target) ret = weight;
            from = from.getSuperclass();
            ++weight;
        }
        ret = ret == 0 ? 0 : weight - ret + 6;
        return ret;
    }

    private static int getClassLuaType(Class type) {
        int retType;
        if (type == char.class) {
            retType = JAVA_CHAR;
        } else if (type == boolean.class) {
            retType = JAVA_BOOLEAN;
        } else if (type == int.class || type == short.class || type == long.class || type == byte.class) {
            retType = JAVA_INTEGER;
        } else if (type == double.class || type == float.class) {
            retType = JAVA_DOUBLE;
        } else if (type == void.class) {
            retType = JAVA_VOID;
        } else {
            retType = JAVA_OBJECT;
        }
        return retType;
    }

    /*
     * * to check whether all the abstract methods of this class has been included;
     */
    private static void checkAllAbstractMethod(Class cl, Map<MethodSetEntry,MethodSetEntry> methodSet,Map<Method,Long> methodList) {
        if (cl == Object.class) return;
        List<Method> methods = new ArrayList<>(Arrays.asList(cl.getMethods()));
        Class thizClass = cl;
        do {
            methods.addAll(Arrays.asList(cl.getDeclaredMethods()));
            thizClass = thizClass.getSuperclass();
        } while (thizClass != Object.class && thizClass != null);

        for (Method method : methods)
            if (Modifier.isAbstract(method.getModifiers())){
                MethodSetEntry key = new MethodSetEntry(method);
                MethodSetEntry old=methodSet.get(key);
                if (old==null)
                    throw new LuaException("The class:" + cl.getName() + " has unimplemented method:" + method);
                if(!old.m.equals(method)&& method.getDeclaringClass().isAssignableFrom(old.m.getDeclaringClass())){
                    //only on true to avoid same method in irrelevant methods
                    methodList.put(method,methodList.remove(old.m));
                    methodSet.put(key,key);
                }
            }


    }

    private static Class resolveType(Type type){
        if(type instanceof Class)
            return (Class) type;
        else if(type instanceof ParameterizedType)
            return (Class) ((ParameterizedType) type).getRawType();
        else if(type instanceof WildcardType)
            return  resolveType(((WildcardType) type).getUpperBounds()[0]);
        else if(type instanceof TypeVariable)
            return resolveType(((TypeVariable) type).getBounds()[0]);
        else  if(type instanceof GenericArrayType){
            Type component=((GenericArrayType) type).getGenericComponentType();
            Class c=resolveType(component);
            return Array.newInstance(c,0).getClass();
        }
       throw new RuntimeException("Unexpected type:"+type);
    }

    static Func getFunc(LuaFunction function,TypeId[] argTypes,TypeId returnType){
        InvokeHandler handler= (InvokeHandler) Proxy.getInvocationHandler(function);
        MethodInfo info=handler.methodMap.values().iterator().next();
        Func func=handler.context().new Func(info.luaFuncInfo,argTypes,returnType);
        info.luaFuncInfo=0;
        return func;
    }

    private static int getTypeLuaType(TypeId<?> type) {
        int retType;
        if (type == TypeId.CHAR) {
            retType = JAVA_CHAR;
        } else if (type == TypeId.BOOLEAN) {
            retType = JAVA_BOOLEAN;
        } else if (type == TypeId.INT || type ==  TypeId.LONG || type ==  TypeId.SHORT|| type ==  TypeId.BYTE) {
            retType = JAVA_INTEGER;
        } else if (type == TypeId.DOUBLE || type == TypeId.FLOAT) {
            retType = JAVA_DOUBLE;
        } else if (type == TypeId.VOID) {
            retType = JAVA_VOID;
        } else {
            retType = JAVA_OBJECT;
        }
        return retType;
    }

    /**
     * add an indexer for index or new index or length call
     * @param target target class
     * @param indexer indexer
     * @param <T> relative type
     */
    public <T> void putIndexer(Class<T> target,Indexer<? super T> indexer){
        lazyIndexers().put(target,indexer);
    }

    /**
     * add an iterator factory to generate a iterator for pairs call
     * @param target target class
     * @param factory factory
     * @param <T> relative type
     */
    public <T> void putIteratorFactory(Class<T> target,IteratorFactory<? super T> factory){
        lazyIteratorFactories().put(target,factory);
    }

    private Indexer resolveIndexer(Object v,Class keyType){
        Class c=v.getClass();
        HashMap<Class,Indexer> indexers=lazyIndexers();
        Indexer indexer = indexers.get(v.getClass());
        if(indexer!=null) return indexer;
        Out:
        if(v instanceof List){
            indexer= new ListIndexer<List>(){
                @Override
                public int size(List obj) {
                    return obj.size();
                }
                @Override
                public Object at(List obj, int key) {
                    return obj.get(key);
                }
                @Override
                public void set(List obj, int key, Object v) {
                    obj.set(key,v);
                }
            };
        }else if(v instanceof Map)
            indexer= new Indexer<Map>(){
                @Override
                public int size(Map obj) {
                    return obj.size();
                }

                @Override
                public Object at(Map obj, Object key) {
                    return obj.get(key);
                }

                @Override
                public void set(Map obj, Object key, Object v) {
                    obj.put(key,v);
                }
            };
        else if(keyType!=null){
            Method sizeMethod=findMethodNoExcept(c,"size");
            if(sizeMethod==null) sizeMethod=findMethodNoExcept(c,"length");
            Method getMethod=findIndexerMethod(c,"get",keyType,1);
            if(getMethod==null) getMethod=findIndexerMethod(c,"at",keyType,1);
            if(getMethod==null) break Out;
            Method setMethod=findIndexerMethod(c,"set",keyType,2);
            if(setMethod==null) setMethod=findIndexerMethod(c,"put",keyType,2);
            Method finalGetMethod = getMethod;
            Method finalSetMethod = setMethod;
            Method finalSizeMethod = sizeMethod;
            indexer=new Indexer() {
                Class rawKeyType=finalGetMethod.getParameterTypes()[0];
                int keyTypeID=getClassLuaType(rawKeyType);
                Class rawValueType;
                int valueTypeID;
                Type realValueType;
                {
                    if(finalSetMethod!=null){
                        rawValueType=finalSetMethod.getParameterTypes()[1];
                        valueTypeID=getClassLuaType(rawValueType);
                        realValueType=finalSetMethod.getGenericParameterTypes()[1];
                    }
                }
                @Override
                public Object at(Object obj, Object key) throws Throwable{
                    return finalGetMethod.invoke(obj,fixValue(key,keyTypeID,rawKeyType,rawKeyType,false));
                }

                @Override
                public void set(Object obj, Object key, Object v)throws Throwable {
                    if(finalSetMethod ==null) throw new UnsupportedOperationException("set");
                    finalSetMethod.invoke(obj,fixValue(key,valueTypeID,rawKeyType,rawKeyType,false),
                            fixValue(v,valueTypeID,rawValueType,realValueType,true));
                }

                @Override
                public int size(Object obj) throws Exception{
                    if(finalSizeMethod ==null) throw new UnsupportedOperationException("size");
                    return (int) finalSizeMethod.invoke(obj);
                }
            };
        }
        if(indexer!=null)indexers.put(v.getClass(),indexer);
        return indexer;
    }

    private int length(Object object) throws Exception{
        if (object instanceof Collection)
            return ((Collection) object).size();
        else if (object instanceof Map)
            return ((Map) object).size();
        Indexer indexer=resolveIndexer(object,null);
        if(indexer==null){
            Class c=object.getClass();
            Method sizeMethod=findMethodNoExcept(c,"length");
            if(sizeMethod==null) sizeMethod=findMethodNoExcept(c,"size");
            if(sizeMethod!=null)
                return ((Number) sizeMethod.invoke(object)).intValue();
            throw new LuaException("Can't get length of "+object);
        }
        return indexer.size(object);
    }

    private HashMap<Class,Indexer> lazyIndexers(){
        if(indexers==null){
            indexers=new HashMap<>();
        }
        return indexers;
    }

    private void set(Object obj,Object key,Object v) throws Throwable{
        Indexer indexer=resolveIndexer(obj,key.getClass());
        if(indexer==null||!indexer.supportsKey(key))
            throw new LuaException(obj+" doesn't have a member with key:"+key);
        indexer.set(obj,key,v);
    }

    private  Object at(Object v,Object key) throws Throwable{
        Indexer indexer=resolveIndexer(v,key.getClass());
        if(indexer==null&&key instanceof String){
            throw new LuaException("No non-static member is named "+key+" in class "+v.getClass().getName());
        }
        if(indexer==null||!indexer.supportsKey(key))
            throw new LuaException(v+" doesn't have a member with key:"+key);
        return indexer.at(v,key);
    }

    private HashMap<Class,IteratorFactory> lazyIteratorFactories(){
        if(iterators==null){
            iterators=new HashMap<>();
            IteratorFactory factory= v -> {
                int length;
                length = length(v);
                Method keyAt=v.getClass().getMethod("keyAt",int.class);
                Method valueAt=v.getClass().getMethod("valueAt",int.class);
                return new MapIterator() {
                    int i;
                    @Override
                    public Object[] nextEntry() throws Throwable{
                        return new Object[]{keyAt.invoke(v,i),valueAt.invoke(v,i++)};
                    }

                    @Override
                    public boolean hasNext() {
                        return i<length;
                    }
                };
            };
            iterators.put(SparseArray.class, factory);
            iterators.put(SparseBooleanArray.class, factory);
            iterators.put(SparseIntArray.class, factory);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2) {
                iterators.put(SparseLongArray.class, factory);
            }
            iterators.put(JSONObject.class, (IteratorFactory<JSONObject>) v -> {
                Iterator<String> iterator=v.keys();
                return new MapIterator() {
                    @Override
                    public boolean hasNext() {
                        return iterator.hasNext();
                    }

                    @Override
                    public Object[] nextEntry() throws Throwable {
                        String key=iterator.next();
                        return new Object[]{key,v.get(key)};
                    }
                };
            });

        }
        return iterators;
    }

    private MapIterator iterate(Object v) throws Throwable{
        HashMap<Class,IteratorFactory> iterators=lazyIteratorFactories();
        IteratorFactory factory=iterators.get(v.getClass());
        if(factory==null){
            if(v instanceof Collection){
                factory= v1 -> {
                    Iterator iter=((Collection) v1).iterator();
                    return new ListIterator() {
                        @Override
                        public Object next() {
                            return iter.next();
                        }

                        @Override
                        public boolean hasNext() {
                            return iter.hasNext();
                        }
                    };
                };
            }else if(v instanceof Map)
                factory= v12 -> {
                    Iterator<Map.Entry> iter=((Map) v12).entrySet().iterator();
                    return new MapIterator() {
                        @Override
                        public boolean hasNext() {
                            return iter.hasNext();
                        }

                        @Override
                        public Object[] nextEntry() {
                            Map.Entry entry=iter.next();
                            return new Object[]{entry.getKey(),entry.getValue()};
                        }
                    };
                };
            else if(v.getClass().isArray()){
                factory= v13 -> {
                    int len=Array.getLength(v13);
                    return new ListIterator() {
                        int i;
                        @Override
                        public Object next() {
                            return Array.get(v13,i++);
                        }

                        @Override
                        public boolean hasNext() {
                            return i<len;
                        }
                    };
                };
            }else {
                Indexer indexer= resolveIndexer(v, int.class);
                if(indexer!=null&&!indexer.supportsKey(Object.class)){
                    factory= v14 -> {
                        int length = length(v14);
                        return new ListIterator() {
                            int i;
                            @Override
                            public Object next() throws Throwable{
                                return at(v14,i++);
                            }

                            @Override
                            public boolean hasNext() {
                                return i<length;
                            }
                        };
                    };
                }
            }
            if(factory!=null)iterators.put(v.getClass(),factory);

        }
        if(factory==null) return null;
        return factory.generate(v);
    }

    private String[] importAll(String pack) throws Exception {
        synchronized (importLock){
            initLoader();
            List<String> nameList = packages.get(pack);
            String[] empty = new String[0];
            if(nameList==null)
                return empty;
            return nameList.toArray(empty);
        }
    }

    private void loadClassLoader(ClassLoader loader) throws Exception {
        synchronized (importLock){
            initLoader();
            Field fPathList = BaseDexClassLoader.class.getDeclaredField("pathList");
            Field fDexElements=null;
            Field fDexFile=null;
            while (loader!=null){
                if(loader instanceof BaseDexClassLoader){
                    fPathList.setAccessible(true);
                    Object pathList= fPathList.get(loader);
                    if(pathList==null) continue;
                    if(fDexElements==null){
                        fDexElements = pathList.getClass().getDeclaredField("dexElements");
                        fDexElements.setAccessible(true);
                    }
                    Object[] elements= (Object[]) fDexElements.get(pathList);
                    if(elements==null) continue;
                    for (Object ele : elements) {
                        if(fDexFile==null){
                            fDexFile = ele.getClass().getDeclaredField("dexFile");
                            fDexFile.setAccessible(true);
                        }
                        DexFile dexFile= (DexFile) fDexFile.get(ele);
                        if(dexFile==null) continue;
                        if(!dexFiles.contains(dexFile.getName())){
                            addDexFile(dexFile);
                        }
                    }
                }
                loader=loader.getParent();
            }
        }

    }

    private void initLoader() throws Exception {
        if(dexFiles==null){
            dexFiles=new HashSet<>();
            packages=new HashMap<>();
            String[] bootJars=System.getenv("BOOTCLASSPATH").split(":");
            for (String path : bootJars) {
                DexFile dexFile = new DexFile(path);
                addDexFile(dexFile);
            }
            loadClassLoader(ScriptContext.class.getClassLoader());
        }
    }

    private void addDexFile(DexFile dexFile) {
        Enumeration<String> entries = dexFile.entries();
        while (entries.hasMoreElements()){
            String cl=entries.nextElement();
            cl=cl.replace('/','.');
            String pack=getPackage(cl);
            List<String> names=packages.get(pack);
            if(names==null){
                names=new ArrayList<>();
                names.add(cl);
                packages.put(pack,names);
            }else names.add(cl);
        }
        dexFiles.add(dexFile.getName());
    }

    @Override
    public void onNewGC(WeakReference<GCTracker> reference) {
        nativeClean(nativePtr);
        gcListener = new WeakReference<>(new GCListener(reference));
    }

    private native long nativeOpen(boolean importAll, boolean localFunction);

    private HashMap<Class, TableConverter> lazyConverts() {
        if (sConverters != null)
            return sConverters;
        sConverters = new HashMap<>();
        TableConverter<HashMap<?, ?>> mapConverter = table -> (HashMap<?, ?>) table;
        sConverters.put(HashMap.class, mapConverter);
        sConverters.put(Map.class, mapConverter);
        sConverters.put(LinkedHashMap.class, mapConverter);
        sConverters.put(JSONObject.class, JSONObject::new);

        TableConverter<ConcurrentHashMap<?, ?>> mapConverter3 = ConcurrentHashMap::new;
        sConverters.put(ConcurrentHashMap.class, mapConverter3);
        TableConverter<ArrayList<?>> listConverter = table -> new ArrayList<>(table.values());
        sConverters.put(ArrayList.class, listConverter);
        sConverters.put(Collection.class, listConverter);
        sConverters.put(List.class, listConverter);
        TableConverter<ArrayDeque<?>> dequeueConverter = table -> new ArrayDeque<>(table.values());
        sConverters.put(ArrayDeque.class, dequeueConverter);
        sConverters.put(Queue.class, dequeueConverter);
        sConverters.put(Deque.class, dequeueConverter);


        TableConverter<HashSet<?>> setConverter = table -> new HashSet<>(table.values());
        sConverters.put(HashSet.class, setConverter);
        sConverters.put(Set.class, setConverter);
        return sConverters;
    }

    /**
     *Take care if a converter exists
     * @param type type to convert
     * @param converter table converter
     * @param <T> type to convert
     * @param <F> sub type of T
     */
    public <T, F extends T> TableConverter putTableConverter(Class<T> type, TableConverter<F> converter) {
        return lazyConverts().put(type, converter);
    }

    private boolean isTableType(Class<?> cls) {
        if (lazyConverts().containsKey(cls)) return true;
        final Constructor constructor;
        try {
            boolean isAbstract=Modifier.isAbstract(cls.getModifiers())|| cls.isInterface();
            if (Map.class.isAssignableFrom(cls)&&!isAbstract) {
                if ((constructor = cls.getConstructor()) == null)
                    return false;
                sConverters.put(cls, table -> {
                    Map<Object, Object> map = (Map) constructor.newInstance();
                    map.putAll(table);
                    return map;
                });
                return true;
            }
            if (Collection.class.isAssignableFrom(cls)&&!isAbstract) {
                if ((constructor = cls.getConstructor()) == null)
                    return false;
                sConverters.put(cls, table -> {
                    Collection list = (Collection) constructor.newInstance();
                    list.addAll(table.values());
                    return list;
                });
                return true;
            }
            Class<?> componentType = cls.getComponentType();
            if (componentType != null) {
                if (componentType.isPrimitive()) {
                    if (componentType == int.class) {
                        sConverters.put(cls, table -> {
                            int[] array = new int[table.size()];
                            int i = 0;
                            for (Object v : table.values()) {
                                array[i++] = ((Long) v).intValue();
                            }
                            return array;
                        });
                    } else if (componentType == byte.class) {
                        sConverters.put(cls, table -> {
                            byte[] array = new byte[table.size()];
                            int i = 0;
                            for (Object v : table.values()) {
                                array[i++] = ((Long) v).byteValue();
                            }
                            return array;
                        });
                    } else if (componentType == long.class) {
                        sConverters.put(cls, table -> {
                            long[] array = new long[table.size()];
                            int i = 0;
                            for (Object v : table.values()) {
                                array[i++] = (Long) v;
                            }
                            return array;
                        });
                    } else if (componentType == short.class) {
                        sConverters.put(cls, table -> {
                            short[] array = new short[table.size()];
                            int i = 0;
                            for (Object v : table.values()) {
                                array[i++] = ((Long) v).shortValue();
                            }
                            return array;
                        });
                    } else if (componentType == float.class) {
                        sConverters.put(cls, table -> {
                            float[] array = new float[table.size()];
                            int i = 0;
                            for (Object v : table.values()) {
                                array[i++] = ((Double) v).floatValue();
                            }
                            return array;
                        });
                    } else if (componentType == double.class) {
                        sConverters.put(cls, table -> {
                            double[] array = new double[table.size()];
                            int i = 0;
                            for (Object v : table.values()) {
                                array[i++] = (Double) v;
                            }
                            return array;
                        });
                    } else if (componentType == char.class) {
                        sConverters.put(cls, table -> {
                            char[] array = new char[table.size()];
                            int i = 0;
                            for (Object v : table.values()) {
                                array[i++] = ((String) v).charAt(0);
                            }
                            return array;
                        });
                    } else return false;
                } else {
                    sConverters.put(cls, table -> table.values().toArray((Object[])
                            Array.newInstance(componentType, table.size())));
                }
                return true;
            }
        } catch (NoSuchMethodException ignored) {
        }
        return false;
    }

    private Object convertTable(Map<?, ?> table, Class target) throws Throwable {
        TableConverter<?> converter = sConverters.get(target);
        if (converter != null)
            return converter.convert(table);
        return null;
    }

    private Type checkTableType(Type type){
        return isTableType(resolveType(type))?type:null;
    }

    private Type resolveKeyTableType(Type type){
        if(type instanceof ParameterizedType){
            Type[] actualTypeArguments = ((ParameterizedType) type).getActualTypeArguments();
            if(actualTypeArguments.length>1)
                return actualTypeArguments[0];
            return null;
        }
        else if(type instanceof WildcardType)
            return  resolveKeyTableType(((WildcardType) type).getUpperBounds()[0]);
        else if(type instanceof TypeVariable)
            return resolveKeyTableType(((TypeVariable) type).getBounds()[0]);
        else return null;
    }

    private Type resolveValueTableType(Type type){
        if(type instanceof ParameterizedType){
            Type[] actualTypeArguments = ((ParameterizedType) type).getActualTypeArguments();
            if(actualTypeArguments.length>1)
                return actualTypeArguments[1];
            return actualTypeArguments[0];
        }else if(type instanceof Class)
            return ((Class) type).getComponentType();
        else  if(type instanceof GenericArrayType)
            return ((GenericArrayType) type).getGenericComponentType();
        else if(type instanceof WildcardType)
            return  resolveValueTableType(((WildcardType) type).getUpperBounds()[0]);
        else if(type instanceof TypeVariable)
            return resolveValueTableType(((TypeVariable) type).getBounds()[0]);
        else return null;
    }

    private Object fixValue(Object from,int type,Class raw,Type real) throws Throwable{
        return fixValue(from,type,raw,real,isTableType(raw));
    }

    private Object fixValue2(Object from,Class raw,Type real,boolean shouldFixTable) throws Throwable{
        if(from==null||raw==void.class) return null;
        if((raw==Integer.class||raw==Integer.TYPE)&&from instanceof Long)
            return ((Long) from).intValue();
        if((raw==Short.class||raw==Short.TYPE)&&from instanceof Long)
            return ((Long) from).shortValue();
        if((raw==Byte.class||raw==Byte.TYPE)&&from instanceof Long)
            return ((Long) from).byteValue();
        if((raw==Float.class||raw==Float.TYPE)&&from instanceof Double)
            return ((Double) from).floatValue();
        if(raw==Character.class&&from instanceof String)
            return ((String) from).charAt(0);
        if(from instanceof Map&&shouldFixTable)
            return convertTable((Map<Object, Object>) from
                    , raw, real);
        if(!raw.isInstance(from))
            throw new LuaException("Incompatible Object passed:expected:"+real+",got:"+from);
        return from;
    }

    private Object fixValue(Object from,int classType,Class raw,Type real,boolean shouldFixTable) throws Throwable{
        if(from==null) return null;
        switch (classType) {
            case CLASS_INT:
            case CLASS_B_INT:
                if(from instanceof Long)
                    return ((Long) from).intValue();
                break;
            case CLASS_SHORT:
            case CLASS_B_SHORT:
                if(from instanceof Long)
                    return ((Long) from).shortValue();
                break;
            case CLASS_BYTE:
            case CLASS_B_BYTE:
                if(from instanceof Long)
                    return ((Long) from).byteValue();
                break;

            case CLASS_FLOAT:
            case CLASS_B_FLOAT:
                if(from instanceof Double)
                    return ((Double) from).floatValue();
                break;
            case CLASS_CHAR:
            case CLASS_B_CHAR:
                if(from instanceof String)
                    return ((String) from).charAt(0);
                break;
            case CLASS_VOID:
                return null;
            default:
                if(from instanceof Map&&shouldFixTable)
                    return convertTable((Map<Object, Object>) from
                            , raw, real);
                if(!raw.isInstance(from))
                    throw new LuaException("Incompatible Object passed:expected:"+real+",got:"+from);
                break;
        }
        return from;
    }

    private Object convertTable(Map<Object, Object> table,Class raw, Type target) throws Throwable {
        TableConverter<?> converter = sConverters.get(raw);
        if (converter != null){
            if(target!=null){
                Type valueType= resolveValueTableType(target);
                if(valueType==null) valueType=converter.expectedValueTableType();
                Type keyType= resolveKeyTableType(target);
                if(keyType==null) keyType=converter.expectedKeyTableType();
                if(valueType!=null){
                    Class rawType = resolveType(valueType);
                    boolean shouldFixTable=isTableType(rawType);
                    for (Map.Entry<Object, Object> entry :
                            table.entrySet()) {
                        Object old = entry.getValue();
                        Object value = fixValue2(old,rawType,valueType,shouldFixTable);
                        if(value!=old)
                            entry.setValue(value);
                    }
                }
                if(keyType!=null){
                    List<Pair<Object, Object>> toChanged=new ArrayList<>();
                    Class rawType = resolveType(keyType);
                    boolean shouldFixTable=isTableType(rawType);
                    for (Map.Entry<Object, Object> entry :
                            table.entrySet()) {
                        Object key = entry.getKey();
                        Object newKey=fixValue2(key,rawType,keyType,shouldFixTable);
                        if(newKey!=key){
                            toChanged.add(new Pair<>(key,newKey));
                        }
                    }
                    for (Pair<Object, Object> pair : toChanged) {
                        table.put(pair.second,table.remove(pair.first));
                    }
                }
            }
            return converter.convert(table);
        }
        return null;
    }

    private Object proxy(final Class<?> main,
                         Class<?>[] leftInterfaces, Method[] methods,
                         long[] values, boolean shared, long nativeInfo,Object superObject) throws Exception {
        if (main == null) throw new IllegalArgumentException("No proxy class");
        if (values == null||methods==null) throw new IllegalArgumentException("No need to proxy");
        if (leftInterfaces != null) {
            for (Class cl : leftInterfaces) {
                if (!cl.isInterface())
                    throw new IllegalArgumentException("Only one class can be extent");
            }
        }
        Map<MethodSetEntry,MethodSetEntry> methodSet = new HashMap<>(values.length);
        Map<Method,Long> methodList=new HashMap<>(values.length);
        if (methods.length != values.length) {
            throw new IllegalArgumentException("Java method count doesn't equal lua method count");
        }
        for (int i = 0, len = methods.length; i < len; ++i) {
            Method m = methods[i];
            if (m == null) throw new IllegalArgumentException("Method can't be null");
            if(methodList.containsKey(m))  throw new IllegalArgumentException("Duplicate method passed in");
            MethodSetEntry entry = new MethodSetEntry(m);
            methodSet.put(entry,entry);
            methodList.put(m, values[i]);
        }
        final boolean isInterface = main.isInterface();
        checkAllAbstractMethod(main, methodSet,methodList);
        if (leftInterfaces != null) {
            for (Class cl : leftInterfaces) {
                checkAllAbstractMethod(cl, methodSet,methodList);
            }
        }
        InvokeHandler handler = new InvokeHandler(methodList, nativePtr, isInterface);
        try {
            if (main.isInterface()) {
                Class[] interfaces = new Class[leftInterfaces == null ? 1 : leftInterfaces.length + 1];
                interfaces[0] = main;
                if (leftInterfaces != null)
                    System.arraycopy(leftInterfaces, 0, interfaces, 1, leftInterfaces.length);
                return Proxy.newProxyInstance(ScriptContext.class.getClassLoader(), interfaces, handler);
            } else {
                ProxyBuilder<?> builder = ProxyBuilder.forClass(main);
                if (leftInterfaces != null) builder.implementing(leftInterfaces);
                if (shared) builder.withSharedClassLoader();
                Class proxyClass = builder.onlyMethods(methodList.keySet().toArray(new Method[0])).buildProxyClass();
                Object ret =superObject==null?constructChild(nativePtr, proxyClass, nativeInfo)
                        :ClassBuilder.cloneFromSuper(proxyClass,superObject);
                ProxyBuilder.setInvocationHandler(ret, handler);
                return ret;
            }
        } catch (Throwable e) {
            handler.deprecate();
            throw e;
        }
    }

    /**
     *
     * @param outLogger
     * @param errLogger
     */
    public void setLogger(OutputStream outLogger, OutputStream errLogger) {
        this.errLogger = errLogger;
        this.outLogger = outLogger;
        registerLogger(nativePtr, outLogger, errLogger);
    }

    /**
     *
     * @param errLogger
     */
    public void setErrLogger(OutputStream errLogger) {
        this.errLogger = errLogger;
        registerLogger(nativePtr, outLogger, errLogger);
    }

    /**
     *
     * @param outLogger
     */
    public void setOutLogger(OutputStream outLogger) {
        this.outLogger = outLogger;
        registerLogger(nativePtr, outLogger, errLogger);
    }

    /**
     * flush log
     */
    public void flushLog() {
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     *
     * @param script script string
     * @return Compiled Script
     */
    public CompiledScript compile(String script) {
        long compile = compile(nativePtr, script, false);
        return new CompiledScript(compile);

    }

    /**
     *
     * @param file script file
     * @return Compiled Script
     */
    public CompiledScript compile(File file) {
        return new CompiledScript(compile(nativePtr, file.getPath(), true));

    }

    /**
     *
     * @param script script
     * @param args script arguments
     * @return script result,conversion see README
     */
    public Object[] run(CompiledScript script, Object... args) {
        return runScript(nativePtr, script, false, args);
    }

    /**
     *
     * @param script script
     * @param args script arguments
     * @return script result,conversion see README
     */
    public Object[] run(String script, Object... args) {
        return runScript(nativePtr, script, false, args);
    }

    /**
     *
     * @param scriptFile script
     * @param args script arguments
     * @return script result,conversion see README
     */
    public Object[] run(File scriptFile, Object... args) {
        return runScript(nativePtr, scriptFile.getPath(), true, args);
    }

    /**
     * remove a global value from lua state
     * @param name lua global name
     * @return this
     */
    public final ScriptContext removeFromLua(String name) {
        return addToLua(name, null, false);
    }

    /**
     * @see ScriptContext#addToLua(String, Object, boolean)
     */
    public final ScriptContext addToLua(String name, Object obj) {
        return addToLua(name, obj, false);
    }

    /**
     *
     * @param name lua global name
     * @param obj value,null to remove the global value
     * @param local false:the method to be added to all lua state,and will be added to new lua states.
     *              true:the method to be added to only current state.If no running state, no operation.
     * @return this
     */
    public final ScriptContext addToLua(String name, Object obj, boolean local) {
        addObject(nativePtr, name, obj, local);
        return this;
    }

    /**
     * @see ScriptContext#addToLua(String, String, Object, boolean)
     */
    public ScriptContext addToLua(String luaName, String methodName, Object instOrType) throws NoSuchMethodException {
        return addToLua(luaName, methodName, instOrType, false);
    }

    /**
     *
     * @param luaName lua global name
     * @param methodName method name in the class
     * @param instOrType o bject for method or the declaring class
     * @param local false:the method to be added to all lua state,and will be added to new lua states,
     *              true:the method to be added to only current state.If no running state, no operation.
     * @return this
     * @throws NoSuchMethodException if the method not found
     */
    public ScriptContext addToLua(String luaName, String methodName, Object instOrType, boolean local) throws NoSuchMethodException {
        if (instOrType == null) throw new IllegalArgumentException("No permitted:null");
        boolean isStatic = instOrType instanceof Class;
        if (findMembers(isStatic ? (Class) instOrType : instOrType.getClass(), methodName, false, isStatic) == null) {
            throw new NoSuchMethodException(methodName);
        }
        addMethod(nativePtr, luaName, methodName, instOrType, local);
        return this;
    }

    public Object get(String name) {
        Object[] result = run("return " + name);
        return result == null ? null : result[0];
    }

    private void close() {
        long nativePtr = this.nativePtr;
        this.nativePtr = 0;
        if (nativePtr != 0)
            nativeClose(nativePtr);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        close();
    }

    private static class ObjectMethods {
        private static Method sToString;
        private static Method sHashCode;
        private static Method sEquals;

        static {
            try {
                sEquals = Object.class.getMethod("equals", Object.class);
                sHashCode = Object.class.getMethod("hashCode");
                sToString = Object.class.getMethod("toString");
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            }
        }
    }

    private static class MethodInfo {
        long luaFuncInfo;
        int[] paramsTypes;
        Class<?> returnType;
        Type genericReturnType;
        int returnClassType;

        MethodInfo(long luaFuncInfo, int[] paramsTypes, Class<?> returnType,Type genericReturnType,int returnClassType) {
            this.luaFuncInfo = luaFuncInfo;
            this.paramsTypes = paramsTypes;
            this.returnType = returnType;
            this.genericReturnType=genericReturnType;
            this.returnClassType=returnClassType;
        }

        @Override
        protected void finalize() {
            if (luaFuncInfo != 0)
                releaseFunc(luaFuncInfo);
        }
    }

    static class MethodSetEntry {
        final Method m;
        private final String name;
        private final Class<?>[] paramTypes;
        private final Class<?> returnType;


        MethodSetEntry(Method m) {
            this.m = m;
            name = m.getName();
            paramTypes = m.getParameterTypes();
            returnType = m.getReturnType();
        }

        @Override
        public int hashCode() {
            return name.hashCode() ^ returnType.hashCode() ^ Arrays.hashCode(paramTypes);
        }

        @Override
        public boolean equals(Object obj) {
            if (obj != null && obj instanceof MethodSetEntry) {

                MethodSetEntry other = ((MethodSetEntry) obj);
                if (name.equals(other.name)) {
                    if (!returnType.equals(other.returnType))
                        return false;
                    /* Avoid unnecessary cloning */
                    Class<?>[] params1 = paramTypes;
                    Class<?>[] params2 = other.paramTypes;
                    if (params1.length == params2.length) {
                        for (int i = 0; i < params1.length; i++) {
                            if (params1[i] != params2[i])
                                return false;
                        }
                        return true;
                    }
                }
            }
            return false;
        }
    }

    /**
     * A compiled script for future call
     */
    public static class CompiledScript extends Number {
        private long address;

        CompiledScript(long address) {
            this.address = address;
        }

        @Override
        protected void finalize() {
            if (address != 0)//anti android bug on object allocated but not initialize when a jni exception raised
                releaseFunc(address);
        }

        /**
         *
         * @throws UnsupportedOperationException
         */
        @Override
        public int intValue() {
            throw new UnsupportedOperationException();
        }

        @Override
        public long longValue() {
            return address;
        }
        /**
         *
         * @throws UnsupportedOperationException
         */
        @Override
        public float floatValue() {
            throw new UnsupportedOperationException();
        }
        /**
         *
         * @throws UnsupportedOperationException
         */
        @Override
        public double doubleValue() {
            throw new UnsupportedOperationException();
        }
    }

    class InvokeHandler implements InvocationHandler {
        private Map<Method, MethodInfo> methodMap = new HashMap<>();
        private boolean isInterface;

        InvokeHandler(Map<Method, Long> methodMap, long nativePtr, boolean isInterface) {
            for (Map.Entry<Method, Long> entry : methodMap.entrySet()) {
                Method key = entry.getKey();
                this.methodMap.put(key, new MethodInfo(entry.getValue(), generateParamTypes(key.getParameterTypes())
                        , key.getReturnType(),key.getGenericReturnType(),getClassType(nativePtr,key.getReturnType())));
            }
            this.isInterface = isInterface;

        }

        void changeCallSite(Class c){
            Method m=getSingleInterface(c);
            if(m==null)
                throw new IllegalArgumentException("Not a single interfaces");
            Method entry= methodMap.keySet().iterator().next();
            if(methodMap.size()!=1||entry.getDeclaringClass()!=LuaFunction.class)
                throw new IllegalStateException("Unchangeable handler");
            MethodInfo old=methodMap.remove(entry);
            MethodInfo info=new MethodInfo(old.luaFuncInfo,generateParamTypes(m.getParameterTypes())
                    ,m.getReturnType(),m.getGenericReturnType(),getClassType(nativePtr,m.getReturnType()));
            old.luaFuncInfo=0;
            methodMap.put(m,info);
        }

        @Override
        public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
            MethodInfo info = methodMap.get(method);//avoid interfaces with the same name;
            if (info == null) {
                if (method.equals(ObjectMethods.sEquals))
                    return proxy == args[0];
                else if (method.equals(ObjectMethods.sToString))
                    return proxy.getClass().getName() + "@" + Integer.toHexString(System.identityHashCode(proxy));
                else if (method.equals(ObjectMethods.sHashCode))
                    return System.identityHashCode(proxy);
                else throw new NoSuchMethodException(method.toGenericString());
            }
            long funcRef = info.luaFuncInfo;
            int[] paramsTypes = info.paramsTypes;
            Object ret = invokeLuaFunction(nativePtr, isInterface, funcRef, proxy, paramsTypes,
                    args);
            return fixValue(ret,info.returnClassType,info.returnType,info.genericReturnType);
        }
        private ScriptContext context(){
            return ScriptContext.this;
        }

        private void deprecate() {
            for (MethodInfo info :
                    methodMap.values()) {
                info.luaFuncInfo = 0;
            }
        }

    }

    public class Func{
        long funcRef;
        int[] argTypes;
        TypeId returnType;
        Class returnClass;
        int classType;
        private Func(long funcRef,TypeId[] argTypes,TypeId returnType){
            this.funcRef=funcRef;
            this.returnType=returnType;
            this.argTypes=new int[argTypes.length];
            for (int i = 0; i < argTypes.length; i++) {
                this.argTypes[i]=getTypeLuaType(argTypes[i]);
            }
        }

        public Object call(Object thiz,Object... args) throws Throwable{
            if(returnClass==null){
                String name = returnType.getName();
                switch (name.charAt(0)) {
                    case 'I':
                        returnClass = int.class;
                        break;
                    case 'J':
                        returnClass = long.class;
                        break;
                    case 'B':
                        returnClass = byte.class;
                        break;
                    case 'Z':
                        returnClass = boolean.class;
                        break;
                    case 'C':
                        returnClass = char.class;
                        break;
                    case 'S':
                        returnClass = short.class;
                        break;
                    case 'F':
                        returnClass = float.class;
                        break;
                    case 'D':
                        returnClass = double.class;
                        break;
                    case 'V':
                        returnClass = void.class;
                        break;
                    default: {
                        break;
                    }
                }
                name = (name.charAt(0) == '[' ? name :
                        name.substring(1, name.length() - 1)).replace('/', '.');
                returnClass = thiz.getClass().getClassLoader().loadClass(name);
                classType=getClassType(nativePtr,returnClass);
            }
            return fixValue(invokeLuaFunction(nativePtr,false,funcRef,thiz,argTypes,args),classType,returnClass,returnClass);
        }

        @Override
        protected void finalize() {
            releaseFunc(funcRef);
        }
    }
}
