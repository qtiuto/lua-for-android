package com.oslorde.luadroid;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.os.Build;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Enumeration;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class LibInstaller {
    private static String readFile(File f){
        try (FileInputStream in=new FileInputStream(f)){
            byte[] bytes=new byte[(int) f.length()];
            in.read(bytes);
            return new String(bytes);
        }catch (IOException e){
            return null;
        }
    }

    private static String anyPrefix(String s,String... array){
        for (String str:array){
            if(s.startsWith(str))
                return str;
        }
        return null;
    }

    public static void install(Context context, int versionCode, String dest){
        install(context,versionCode,dest,"assets/lua/");
    }

    /**
     * install scripts or libraries in current application
     * @param context Application context
     * @param versionCode lib version
     * @param dest destination directory
     * @param zipDirs zip directory entries where scripts or libs are stored
     */
    public static boolean install(Context context, int versionCode, String dest,String...zipDirs){
        ApplicationInfo info = context.getApplicationInfo();
        File versionFile=new File(dest, "version.txt");
        String verStr = String.valueOf(versionCode);
        if(versionFile.exists()&& verStr.equals(readFile(versionFile))){
            return false;
        }
        try {
            File parentFile = versionFile.getParentFile();
            if(!parentFile.exists())
                parentFile.mkdirs();
            else Runtime.getRuntime().exec("rm -r "+ parentFile.getPath()).waitFor();
        }catch (Exception ignored){}
        List<String> apps=new ArrayList<>(Collections.singletonList(info.sourceDir));
        if(Build.VERSION.SDK_INT>=21&&info.splitSourceDirs!=null){
            apps.addAll(Arrays.asList(info.splitSourceDirs));
        }
        for(String app:apps){
            try (ZipFile zipFile = new ZipFile(app)){
                Enumeration<? extends ZipEntry> entries = zipFile.entries();
                while (entries.hasMoreElements()){
                    ZipEntry entry=entries.nextElement();
                    String dir;
                    if((dir=anyPrefix(entry.getName(),zipDirs))!=null){
                        if(entry.isDirectory()) continue;
                        byte[] bytes = new byte[1024];
                        int read;
                        File dst=new File(dest, entry.getName().substring(dir.length()));
                        File parentFile = dst.getParentFile();
                        if(!parentFile.exists()) parentFile.mkdirs();
                        try (InputStream stream = zipFile.getInputStream(entry);
                             FileOutputStream out = new FileOutputStream(dst);){
                            while ((read = stream.read(bytes)) > 0) {
                                out.write(bytes, 0, read);
                            }
                        }
                    }
                }
            }catch (IOException e){return false;}
        }

        try (FileOutputStream out=new FileOutputStream(versionFile)){
            out.write(verStr.getBytes());
        }catch (IOException ignored){}
        return true;
    }

    /**
     * helper function that add path to package.path and package.cpath
     * @param context runner
     * @param path string added to package.path
     * @param cpath string added to package.cpath
     */
    public static void configPath(ScriptContext context,String path,String cpath){
        context.run("package.path='"+path+"';package.path\npackage.cpath='"+cpath+"';package.cpath");
    }
}
