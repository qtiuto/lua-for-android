package com.oslorde.luadroidtest;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;

import com.oslorde.luadroid.ScriptContext;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.test).setOnClickListener((v)->{
            ffitest();
        });
        ffitest();
    }
    static {
        System.loadLibrary("luadroid");
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
