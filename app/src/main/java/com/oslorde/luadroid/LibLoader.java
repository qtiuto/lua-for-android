package com.oslorde.luadroid;

import android.app.Application;
import android.content.pm.ApplicationInfo;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.lang.reflect.Method;
import java.nio.file.Files;
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
                    handler.postAtFrontOfQueue(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                outA[0] = (Application) CurrentApplication.invoke(null);
                                synchronized (outA) {
                                    outA.notify();
                                }
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
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
            File tmp = File.createTempFile("lib", ".so");
            if (isLibExtracted()) {
                Method method = ClassLoader.class.getDeclaredMethod("findLibrary", String.class);
                method.setAccessible(true);
                File src = new File((String) method.invoke(ScriptContext.class.getClassLoader(), "luadroid"));
                if (src.exists())
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                        Files.copy(src.toPath(), tmp.toPath());
                    } else {
                        FileInputStream in = null;
                        FileOutputStream out = null;
                        try {
                            in = new FileInputStream(src);
                            out = new FileOutputStream(tmp);
                            in.getChannel().transferTo(0, src.length(), out.getChannel());
                        } finally {
                            if (in != null) in.close();
                            if (out != null) out.close();
                        }
                    }
            } else {
                Application application = getApplication();
                ApplicationInfo info = application.getPackageManager().getApplicationInfo(BuildConfig.APPLICATION_ID, 0);
                ZipFile zipFile = new ZipFile(info.sourceDir);
                ZipEntry entry = zipFile.getEntry("lib/" + Build.CPU_ABI + "/libluadroid.so");
                InputStream stream = zipFile.getInputStream(entry);
                byte[] bytes = new byte[1024];
                int read;
                FileOutputStream out = new FileOutputStream(tmp);
                while ((read = stream.read(bytes)) > 0) {
                    out.write(bytes, 0, read);
                }
                stream.close();
                out.close();
                zipFile.close();
            }
            System.load(tmp.getAbsolutePath());
            tmp.delete();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static boolean isLibExtracted() {
        return Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP || !Build.CPU_ABI.equals(Build.SUPPORTED_32_BIT_ABIS[0]);
    }
}
