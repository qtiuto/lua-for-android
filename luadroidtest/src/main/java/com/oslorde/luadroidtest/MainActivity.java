package com.oslorde.luadroidtest;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;

import androidx.appcompat.app.AppCompatActivity;

import com.android.dx.cf.direct.DirectClassFile;
import com.android.dx.cf.direct.StdAttributeFactory;
import com.android.dx.command.dexer.DxContext;
import com.android.dx.dex.DexOptions;
import com.android.dx.dex.cf.CfOptions;
import com.android.dx.dex.cf.CfTranslator;
import com.android.dx.dex.file.DexFile;
import com.oslorde.luadroid.ScriptContext;
import com.sun.source.util.JavacTask;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.RandomAccessFile;
import java.io.Writer;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.net.URI;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import javax.tools.Diagnostic;
import javax.tools.DiagnosticListener;
import javax.tools.FileObject;
import javax.tools.ForwardingJavaFileManager;
import javax.tools.JavaCompiler;
import javax.tools.JavaFileObject;
import javax.tools.SimpleJavaFileObject;
import javax.tools.StandardJavaFileManager;
import javax.tools.ToolProvider;

import dalvik.system.DexClassLoader;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.test).setOnClickListener((v)->{
            test();
        });
        test();
    }
    static {
        System.loadLibrary("luadroid");
    }

    void test(){
        compilerTest();
    }

    private static byte[] readAllBytes(InputStream in) throws IOException {
        byte[] buf=new byte[1024];
        int read;
        ByteArrayOutputStream outputStream=new ByteArrayOutputStream();
        while ((read=in.read(buf))>0){
            outputStream.write(buf,0,read);
        }
        return outputStream.toByteArray();
    }

    private static String readAll(InputStream in) throws IOException{
        byte[] buf=new byte[1024];
        int read;
        ByteArrayOutputStream outputStream=new ByteArrayOutputStream();
        while ((read=in.read(buf))>0){
            outputStream.write(buf,0,read);
        }
        return outputStream.toString();
    }

    static class MemoryJavaContent extends SimpleJavaFileObject
    {
        ByteArrayOutputStream bout=new ByteArrayOutputStream();

        protected MemoryJavaContent(String name) {
            super(URI.create(name), Kind.CLASS);
        }

        @Override
        public OutputStream openOutputStream() throws IOException {
            return bout;
        }
    }

    static class MemoryJavaFileManager extends ForwardingJavaFileManager<StandardJavaFileManager>{

        /**
         * Creates a new instance of ForwardingJavaFileManager.
         *
         * @param fileManager delegate to this file manager
         */
        protected MemoryJavaFileManager(StandardJavaFileManager fileManager) {
            super(fileManager);
        }

        public JavaFileObject getJavaFileForOutput(Location location,
                                            String className,
                                            JavaFileObject.Kind kind,
                                            FileObject sibling) throws IOException{
            if(kind== JavaFileObject.Kind.CLASS)
                return new MemoryJavaContent(className);
            else return super.getJavaFileForOutput(location,className,kind,sibling);
        }

        @Override
        public FileObject getFileForOutput(Location location, String packageName, String relativeName, FileObject sibling) throws IOException {
            return super.getFileForOutput(location, packageName, relativeName, sibling);
        }
    }

    public DexFile generateDexFile(List<byte[]> classByteCodes){
        DxContext dxContext=new DxContext();
        DexOptions dexOptions=new DexOptions();
        CfOptions cfOptions=new CfOptions();
        DexFile dexFile=new DexFile(dexOptions);
        for (byte[] cls:classByteCodes){
            DirectClassFile directClassFile=new DirectClassFile(cls,"",false);
            directClassFile.setAttributeFactory(StdAttributeFactory.THE_ONE);
            directClassFile.getMagic();
            dexFile.add(CfTranslator.translate(dxContext,directClassFile,cls,cfOptions,dexOptions,dexFile));
        }
        return dexFile;
    }

    public void compilerTest(){
        JavaCompiler compiler= ToolProvider.getSystemJavaCompiler();
        StandardJavaFileManager fileManager = compiler.getStandardFileManager(null,                 null, null);
        DiagnosticListener<JavaFileObject> diagnosticListener=new DiagnosticListener<JavaFileObject>() {
            @Override
            public void report(Diagnostic<? extends JavaFileObject> diagnostic) {
                System.err.println(diagnostic);
            }
        };
        Writer writer=new PrintWriter(System.err){
            @Override
            public void write(int c) {
                super.write(c);
            }
        };

        StringJavaFileObject testScript=new StringJavaFileObject("Main.java",
                "package test;\n" +
                        "import java.util.*;\n" +
                        "public final class Main {\n" +
                        "private Main(List<String> arg){Runnable v=()->System.out.println(arg);v.run();}\n" +
                        "public static String greeting(String name) {\n" +
                        "\treturn \"Hello \" + name;\n" + "}\n}\n");
        List<String> options= Arrays.asList(
                //"-source","1.8"
        );

        JavacTask task= (JavacTask) compiler.getTask(writer,new MemoryJavaFileManager(fileManager),diagnosticListener,options,null, Collections.singletonList(testScript));
        try {
            java.util.List<byte[]> bytes= new ArrayList<>();
            for (JavaFileObject javaContent:task.generate()) {
                bytes.add(((ByteArrayOutputStream)javaContent.openOutputStream()).toByteArray());
            }
            DexFile dexFile= generateDexFile(bytes);
            File path = File.createTempFile("dk_", ".dex");
            RandomAccessFile file=new RandomAccessFile(path,"rw");
            byte[] dex=dexFile.toDex(null,false);
            file.setLength(dex.length);
            FileChannel channel=file.getChannel();
            channel.map(FileChannel.MapMode.READ_WRITE,0,dex.length).put(ByteBuffer.wrap(dex));
            channel.close();
            file.close();
            DexClassLoader loader=new DexClassLoader(path.getPath(),null,null,getClass().getClassLoader());
            Class c=loader.loadClass("test.Main");
            System.out.println(c);
            for (Constructor constructor:c.getDeclaredConstructors()){
                System.out.println(constructor);
                constructor.setAccessible(true);
                constructor.newInstance(Arrays.asList("gdfgdf","gdghdfh","juyujg"));
            }
            for (Method method:c.getDeclaredMethods()){
                System.out.println(method);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    public void ffitest() {
        // Context of the app under test.
        ScriptContext context=new ScriptContext();
        AssetManager manager=getAssets();
        try(InputStream stream=manager.open("luaffitest.lua")) {
            context.run(readAll(stream));
        }catch (Exception e){
            context.flushLog();
            Log.e("ffi","Test failed",e);
        }
        context.flushLog();
        Log.d("ffi","Test passed");
    }
}
