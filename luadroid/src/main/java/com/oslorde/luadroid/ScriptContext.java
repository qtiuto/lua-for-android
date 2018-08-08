package com.oslorde.luadroid;


import android.os.Build;
import android.util.SparseArray;
import android.util.SparseBooleanArray;
import android.util.SparseIntArray;

import com.android.dx.stock.ProxyBuilder;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.File;
import java.io.OutputStream;
import java.lang.ref.WeakReference;
import java.lang.reflect.Array;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.lang.reflect.Proxy;
import java.util.AbstractCollection;
import java.util.AbstractList;
import java.util.AbstractMap;
import java.util.AbstractSet;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Deque;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.LinkedTransferQueue;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.TransferQueue;

public class ScriptContext implements GCTracker {
    private static final int JAVA_CHAR = 0;
    private static final int JAVA_BOOLEAN = 1;
    private static final int JAVA_INTEGER = 2;
    private static final int JAVA_DOUBLE = 3;
    private static final int JAVA_VOID = 4;
    private static final int JAVA_OBJECT = 5;

    static {

    }

    private HashMap<Class, TableConverter> sConverters;
    private long nativePtr;
    private OutputStream outLogger;
    private OutputStream errLogger;
    private WeakReference<GCListener> gcListener;

    public ScriptContext() {
        this(true, false);
    }

    public ScriptContext(boolean importAllFunctions) {
        this(importAllFunctions, false);
    }

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

    @SuppressWarnings("unchecked")
    public static <T> T changeCallSite(LuaFunction function, Class<T> target) {
        if (target == LuaFunction.class)
            return (T) function;
        return (T) Proxy.newProxyInstance(target.getClassLoader(), new Class[]{target}, new InvocationHandler() {
            @Override
            public Object invoke(Object proxy, Method method, Object[] args) {
                return function.invoke(args)[0];
            }
        });

    }

    private static int length(Object object) {
        if (object.getClass().isArray())
            return Array.getLength(object);
        else if (object instanceof Collection)
            return ((Collection) object).size();
        else if (object instanceof Map)
            return ((Map) object).size();
        else if (object instanceof JSONArray)
            return ((JSONArray) object).length();
        else if (object instanceof SparseArray)
            return ((SparseArray) object).size();
        else if (object instanceof SparseBooleanArray)
            return ((SparseBooleanArray) object).size();
        else if (object instanceof SparseIntArray)
            return ((SparseIntArray) object).size();
        throw new UnsupportedOperationException();
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
            Map<Class, Field> fieldMap = new HashMap<>();
            checkAndAddFields(name, isStatic, retList, fieldMap, cl.getFields());
            do {
                checkAndAddFields(name, isStatic, retList, fieldMap, cl.getDeclaredFields());
                cl = cl.getSuperclass();
            } while (cl != null);
            return retList.toArray();
        }
        if (name.equals("<init>")) {
            return cl.getDeclaredConstructors();
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

    private static void checkAndAddMethods(
            String name, boolean isStatic, ArrayList<Object> retList,
            Set<MethodSetEntry> methodSet, Method[] methods) {
        for (Method m : methods) {
            if (isStatic != Modifier.isStatic(m.getModifiers()))
                continue;
            if (!name.equals(m.getName())) continue;
            if (methodSet.add(new MethodSetEntry(m))) {
                retList.add(m);
                retList.add(m.getReturnType());
            }
        }
    }

    private static void checkAndAddFields(
            String name, boolean isStatic, ArrayList<Object> retList,
            Map<Class, Field> fieldMap, Field[] fields) {
        for (Field f : fields) {
            if (isStatic != Modifier.isStatic(f.getModifiers()))
                continue;
            if (!name.equals(f.getName())) continue;
            if (!fieldMap.containsKey(f.getType())) {
                retList.add(f);
                retList.add(f.getType());
                fieldMap.put(f.getType(), f);
            }
        }
    }

    private static int weightObject(Class<?> target, Class<?> from) {
        if (target.isPrimitive() || from.isPrimitive()) return 0;
        if (target == Object.class) return 1;
        if (target.isInterface() && target.isAssignableFrom(from)) {
            return 2;
        }
        if (target.isArray()) {
            if (from.isArray())
                return weightObject(from.getComponentType(), target.getComponentType());
            else return 0;
        } else if (from.isArray()) return 0;
        int weight = 1;
        int ret = 0;
        while (from != null) {
            if (from == target) ret = weight;
            from = from.getSuperclass();
            ++weight;
        }
        ret = ret == 0 ? 0 : weight - ret + 1;
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
    private static void checkAllAbstractMethod(Class cl, Set<MethodSetEntry> methodSet) {
        if (cl == Object.class) return;
        List<Method> methods = new ArrayList<>(Arrays.asList(cl.getMethods()));
        Class thizClass = cl;
        do {
            methods.addAll(Arrays.asList(cl.getDeclaredMethods()));
            thizClass = thizClass.getSuperclass();
        } while (thizClass != Object.class && thizClass != null);

        for (Method method : methods)
            if (Modifier.isAbstract(method.getModifiers()))
                if (!methodSet.contains(new MethodSetEntry(method)))
                    throw new LuaException("The class:" + cl.getName() + " has unimplemented method:" + method);

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
        TableConverter<HashMap<?, ?>> mapConverter = HashMap::new;
        sConverters.put(HashMap.class, mapConverter);
        sConverters.put(Map.class, mapConverter);
        sConverters.put(AbstractMap.class, mapConverter);
        TableConverter<LinkedHashMap<?, ?>> mapConverter2 = LinkedHashMap::new;
        sConverters.put(LinkedHashMap.class, mapConverter2);
        sConverters.put(JSONObject.class, JSONObject::new);

        TableConverter<ConcurrentHashMap<?, ?>> mapConverter3 = ConcurrentHashMap::new;
        sConverters.put(ConcurrentHashMap.class, mapConverter3);
        TableConverter<ArrayList<?>> listConverter = table -> new ArrayList<>(table.values());
        sConverters.put(ArrayList.class, listConverter);
        sConverters.put(Collection.class, listConverter);
        sConverters.put(AbstractCollection.class, listConverter);
        sConverters.put(List.class, listConverter);
        sConverters.put(AbstractList.class, listConverter);
        TableConverter<ArrayDeque<?>> dequeueConverter = table -> new ArrayDeque<>(table.values());
        sConverters.put(ArrayDeque.class, dequeueConverter);
        sConverters.put(Queue.class, dequeueConverter);
        sConverters.put(Deque.class, dequeueConverter);
        TableConverter<SynchronousQueue<?>> blockingQueueConverter = table -> {
            SynchronousQueue<Object> objects = new SynchronousQueue<>();
            objects.addAll(table.values());
            return objects;
        };
        sConverters.put(SynchronousQueue.class, blockingQueueConverter);
        sConverters.put(BlockingQueue.class, blockingQueueConverter);
        TableConverter<LinkedBlockingDeque<?>> blockingDequeConverter = table -> new LinkedBlockingDeque<>(table.values());
        sConverters.put(BlockingDeque.class, blockingDequeConverter);
        sConverters.put(LinkedBlockingDeque.class, blockingDequeConverter);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            TableConverter<LinkedTransferQueue<?>> transferQueueConverter = table -> new LinkedTransferQueue<>(table.values());
            sConverters.put(TransferQueue.class, transferQueueConverter);
            sConverters.put(LinkedTransferQueue.class, transferQueueConverter);
        }
        TableConverter<HashSet<?>> setConverter = table -> new HashSet<>(table.values());
        sConverters.put(HashSet.class, setConverter);
        sConverters.put(Set.class, setConverter);
        sConverters.put(AbstractSet.class, setConverter);
        return sConverters;
    }

    public <T, F extends T> void putTableConverter(Class<T> type, TableConverter<F> converter) {
        lazyConverts().put(type, converter);
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
            /*if(Set.class.isAssignableFrom(cls)){
                if((constructor=cls.getConstructor())==null)
                    return false;
                sConverters.put(cls, table -> {
                    Collection list= (Collection) constructor.newInstance();
                    list.addAll(table.keySet());
                    return list;
                });
                return true;
            }*/
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
        else if (target.isInterface() && table.size() > 0) {
            Method[] methods = target.getMethods();
            for (Method m : methods) {
                Object object = table.get(m.getName());
                if (!(object instanceof LuaFunction))
                    throw new LuaException("Method " + m + " is not implemented");
            }

            Proxy.newProxyInstance(target.getClassLoader(), new Class[]{target}, new InvocationHandler() {
                @Override
                public Object invoke(Object proxy, Method method, Object[] args) {
                    LuaFunction func = (LuaFunction) table.get(method.getName());
                    return func.invoke(args)[0];
                }
            });
        }
        return null;
    }

    private Object proxy(final long nativePtr, final Class<?> main,
                         Class<?>[] leftInterfaces, Method[] methods,
                         long[] values, boolean shared, long nativeInfo) throws Exception {
        if (main == null) throw new IllegalArgumentException("No proxy class");
        if (values == null) throw new IllegalArgumentException("No lua functions");
        if (leftInterfaces != null) {
            for (Class cl : leftInterfaces) {
                if (!cl.isInterface())
                    throw new IllegalArgumentException("Only one class can be extent");
            }
        }
        Map<MethodSetEntry, Long> methodMap = new HashMap<>(values.length);
        if (methods.length != values.length) {
            throw new IllegalArgumentException("Java method count doesn't equal lua method count");
        }
        for (int i = 0, len = methods.length; i < len; ++i) {
            Method m = methods[i];
            if (m == null) throw new IllegalArgumentException("Method can't be null");
            methodMap.put(new MethodSetEntry(m), values[i]);
        }
        if (methodMap.size() == 1) {
            Map.Entry<MethodSetEntry, Long> entry = methodMap.entrySet().iterator().next();
            methodMap = Collections.singletonMap(entry.getKey(), entry.getValue());
        }
        final boolean isInterface = main.isInterface();
        checkAllAbstractMethod(main, methodMap.keySet());
        if (leftInterfaces != null) {
            for (Class cl : leftInterfaces) {
                checkAllAbstractMethod(cl, methodMap.keySet());
            }
        }
        InvokeHandler handler = new InvokeHandler(methodMap, nativePtr, isInterface);
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
                Class proxyClass = builder.onlyMethods(methods).buildProxyClass();
                Object ret = constructChild(nativePtr, proxyClass, nativeInfo);
                ProxyBuilder.setInvocationHandler(ret, handler);
                return ret;
            }
        } catch (Throwable e) {
            handler.deprecate();
            throw e;
        }
    }

    public void setLogger(OutputStream outLogger, OutputStream errLogger) {
        this.errLogger = errLogger;
        this.outLogger = outLogger;
        registerLogger(nativePtr, outLogger, errLogger);
    }

    public void setErrLogger(OutputStream errLogger) {
        this.errLogger = errLogger;
        registerLogger(nativePtr, outLogger, errLogger);
    }

    public void setOutLogger(OutputStream outLogger) {
        this.outLogger = outLogger;
        registerLogger(nativePtr, outLogger, errLogger);
    }

    public void flushLog() {
        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public CompiledScript compile(String script) {
        long compile = 0;
        compile = compile(nativePtr, script, false);
        return new CompiledScript(compile);

    }

    public CompiledScript compile(File file) {
        return new CompiledScript(compile(nativePtr, file.getPath(), true));

    }

    public Object[] run(String script) {
        return runScript(nativePtr, script, false);
    }

    private Object[] run(Object script) {//for reflection
        return runScript(nativePtr, script, false);
    }

    public Object[] run(File scriptFile) {//for reflection
        return runScript(nativePtr, scriptFile.getPath(), true);
    }

    public Object[] run(CompiledScript script, Object... args) {
        return runScript(nativePtr, script, false, args);
    }

    public Object[] run(String script, Object... args) {
        return runScript(nativePtr, script, false, args);
    }

    public Object[] run(File scriptFile, Object... args) {
        return runScript(nativePtr, scriptFile.getPath(), true, args);
    }

    public final ScriptContext removeFromLua(String name) {
        return addToLua(name, null, false);
    }

    public final ScriptContext addToLua(String name, Object obj) {
        return addToLua(name, obj, false);
    }

    public final ScriptContext addToLua(String name, Object obj, boolean local) {
        addObject(nativePtr, name, obj, local);
        return this;
    }

    public ScriptContext addToLua(String luaName, String methodName, Object instOrType) throws NoSuchMethodException {
        return addToLua(luaName, methodName, instOrType, false);
    }

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

        MethodInfo(long luaFuncInfo, int[] paramsTypes, Class<?> returnType) {
            this.luaFuncInfo = luaFuncInfo;
            this.paramsTypes = paramsTypes;
            this.returnType = returnType;
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
            return name.hashCode() + returnType.hashCode() + Arrays.hashCode(paramTypes);
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

    public static class CompiledScript extends Number {
        private long address;

        public CompiledScript(long address) {
            this.address = address;
        }

        @Override
        protected void finalize() {
            if (address != 0)//anti android bug on object allocated but not initialize when a jni exception raised
                releaseFunc(address);
        }

        @Override
        public int intValue() {
            throw new UnsupportedOperationException();
        }

        @Override
        public long longValue() {
            return address;
        }

        @Override
        public float floatValue() {
            throw new UnsupportedOperationException();
        }

        @Override
        public double doubleValue() {
            throw new UnsupportedOperationException();
        }
    }

    class InvokeHandler implements InvocationHandler {
        private Map<MethodSetEntry, MethodInfo> methodMap = new HashMap<>();
        private boolean isInterface;

        InvokeHandler(Map<MethodSetEntry, Long> methodMap, long nativePtr, boolean isInterface) {
            for (Map.Entry<MethodSetEntry, Long> entry : methodMap.entrySet()) {
                MethodSetEntry key = entry.getKey();
                this.methodMap.put(key, new MethodInfo(entry.getValue(), generateParamTypes(key.paramTypes), key.returnType));
            }
            this.isInterface = isInterface;

        }

        @Override
        public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
            MethodInfo info = methodMap.get(new MethodSetEntry(method));//avoid interfaces with the same name;
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
            Class<?> returnType = info.returnType;
            Object ret = invokeLuaFunction(nativePtr, isInterface, funcRef, proxy, paramsTypes,
                    args);
            if (returnType == int.class)
                return ((Long) ret).intValue();
            else if (returnType == char.class)
                return ret == null ? null : ret.toString().charAt(0);
            else if (returnType == short.class)
                return ((Long) ret).shortValue();
            else if (returnType == byte.class)
                return ((Long) ret).byteValue();
            else if (returnType == float.class)
                return ((Double) ret).floatValue();
            return ret;//void case
        }

        void changeCallSite(Method orig, Method other) {
            MethodInfo only = methodMap.get(new MethodSetEntry(orig));
            only.paramsTypes = generateParamTypes(other.getParameterTypes());
            only.returnType = other.getReturnType();
        }

        @Override
        protected void finalize() throws Throwable {
            super.finalize();
        }

        private void deprecate() {
            methodMap.clear();
        }


    }
}
