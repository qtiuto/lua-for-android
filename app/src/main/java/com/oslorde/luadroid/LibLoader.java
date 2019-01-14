package com.oslorde.luadroid;

import android.app.Application;
import android.content.pm.ApplicationInfo;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import dalvik.system.BaseDexClassLoader;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class LibLoader {
    private static Application getApplication() {
        try {
            final Method CurrentApplication = Class.forName("android.app.ActivityThread").
                    getDeclaredMethod("currentApplication");
            Application application = null;
            if (Build.VERSION.SDK_INT < 18) {
                Handler handler = new Handler(Looper.getMainLooper());
                final Application[] outA = new Application[1];
                while (outA[0] == null) {
                    handler.postAtFrontOfQueue(() -> {
                        try {
                            outA[0] = (Application) CurrentApplication.invoke(null);
                            synchronized (outA) {
                                outA.notify();
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    });
                    synchronized (outA) {
                        outA.wait();
                    }
                }
                application = outA[0];
            } else while (application == null) {
                application = (Application) CurrentApplication.invoke(null);
            }
            return application;
        } catch (Throwable e) {
            throw new RuntimeException("Failed to Find Application", e);
        }
    }

    public static void load() {
        try {
            loadLib( "luadroid",true);
            loadLib("ffi",false);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static void loadLib(String libName,boolean load) throws Exception {
        File dst =new File(getApplication().getCacheDir(), "lib" + libName + ".so");
        if (isLibExtracted()) {
            String libPath = ((BaseDexClassLoader) ScriptContext.class.getClassLoader()).findLibrary(libName);
            File src ;
            if (libPath!=null&&(src= new File(libPath)).exists())
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                    Files.copy(src.toPath(), dst.toPath(), StandardCopyOption.REPLACE_EXISTING);
                } else {
                    try( FileInputStream in = new FileInputStream(src);
                            FileOutputStream out = new FileOutputStream(dst)) {
                        in.getChannel().transferTo(0, src.length(), out.getChannel());
                    } catch (Exception ignored){ }
                }
        } else {
            Application application = getApplication();
            ApplicationInfo info = application.getPackageManager().getApplicationInfo(BuildConfig.APPLICATION_ID, 0);
            ZipFile zipFile = new ZipFile(info.sourceDir);
            ZipEntry entry = zipFile.getEntry("lib/" + Build.CPU_ABI + "/lib" + libName + ".so");
            InputStream stream = zipFile.getInputStream(entry);
            byte[] bytes = new byte[1024];
            int read;
            FileOutputStream out = new FileOutputStream(dst);
            while ((read = stream.read(bytes)) > 0) {
                out.write(bytes, 0, read);
            }
            stream.close();
            out.close();
            zipFile.close();
        }
        dst.deleteOnExit();
        if(load){
            createNewNativeDir(dst.getParentFile());
            System.loadLibrary(libName);
        }
    }


    private static boolean isLibExtracted() {
        return Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP || !Build.CPU_ABI.equals(Build.SUPPORTED_32_BIT_ABIS[0]);
    }
    private static void createNewNativeDir(File path) throws Exception{
        BaseDexClassLoader pathClassLoader = (BaseDexClassLoader) LibLoader.class.getClassLoader();
        Object pathList = getPathList(pathClassLoader);
        if(Build.VERSION.SDK_INT<23){
            Field nativeLibraryDirectories = pathList.getClass().getDeclaredField("nativeLibraryDirectories");
            nativeLibraryDirectories.setAccessible(true);
            File[] files = (File[]) nativeLibraryDirectories.get(pathList);
            File[] newPathList = new File[files.length + 1];
            newPathList[0] = path;
            System.arraycopy(files, 0, newPathList, 1, files.length);
            nativeLibraryDirectories.set(pathList, newPathList);
        }else {
            Field nativeLibraryPathElements = pathList.getClass().getDeclaredField("nativeLibraryPathElements");
            nativeLibraryPathElements.setAccessible(true);
            Method m=pathList.getClass().getDeclaredMethod("makePathElements",Build.VERSION.SDK_INT>25?new Class[]{List.class}:
            new Class[]{List.class,File.class,List.class});
            m.setAccessible(true);
            List<File> in=new ArrayList<>();
            in.add(path);
            Object merge= m.invoke(null,Build.VERSION.SDK_INT>25?new Object[]{in}:new Object[]{in,null,new ArrayList()});
            Object orig=nativeLibraryPathElements.get(pathList);
            Object out=Array.newInstance(orig.getClass().getComponentType(),Array.getLength(orig)+1);
            System.arraycopy(merge,0,out,0,1);
            System.arraycopy(orig,0,out,1,Array.getLength(orig));
            nativeLibraryPathElements.set(pathList,out);
        }

        /*if(pathClassLoader.findLibrary("lua")==null){
            File file = new File(path, System.mapLibraryName("lua"));
        }*/
    }
    private static Object getPathList(Object obj) throws ClassNotFoundException, NoSuchFieldException, IllegalAccessException {
        return getField(obj, Class.forName("dalvik.system.BaseDexClassLoader"), "pathList");
    }
    private static Object getField(Object obj, Class cls, String str) throws NoSuchFieldException, IllegalAccessException {
        Field declaredField = cls.getDeclaredField(str);
        declaredField.setAccessible(true);
        return declaredField.get(obj);
    }
}
