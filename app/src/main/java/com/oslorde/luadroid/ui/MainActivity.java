package com.oslorde.luadroid.ui;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.StateListDrawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.StrictMode;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.text.style.ForegroundColorSpan;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.PopupWindow;
import android.widget.TextView;
import android.widget.Toast;

import com.oslorde.luadroid.BuildConfig;
import com.oslorde.luadroid.DefaultScriptContext;
import com.oslorde.luadroid.LibLoader;
import com.oslorde.luadroid.Logger;
import com.oslorde.luadroid.R;
import com.oslorde.luadroid.TransmitClient;

import java.io.DataInputStream;
import java.io.File;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Random;


public class MainActivity extends Activity {

    public static final String LAST = "last";

    static {
        System.loadLibrary("luadroid");
    }

    private final Handler handler = new Handler(Looper.getMainLooper());

    LuaEditor editor;
    IScriptContext context;
    IScriptContext old;
    IClassList classList;
    LinearLayout psBar;
    String[] ps = {"(", ")", "[", "]", "{", "}", "\"", "=", ":", ".", ",", "_", "+", "-", "*", "/", "\\", "%", "#", "^", "$", "?", "<", ">", "~", ";", "'"};
    CharSequence result;
    private PopupWindow popupWindow;
    public static final String[] PRIMITIVE_TYPES = new String[]{"int", "long", "short", "byte", "double", "float","char","boolean"};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Intent intent=getIntent();
        int port=intent.getIntExtra("remotePort",0);
        if(port>0){
            try {
                context=new TransmitClient(port);
            } catch (IOException e) {
                finish();
            }
        }else context=new DefaultScriptContext();
        if(editor==null){
            setContentView(R.layout.main);
            editor = findViewById(R.id.editor);
            psBar = findViewById(R.id.ps_bar);
            for (String text : ps) {
                StateListDrawable sd = new StateListDrawable();
                sd.addState(new int[]{android.R.attr.state_pressed}, new ColorDrawable(0x88000088));
                sd.addState(new int[]{0}, new ColorDrawable(0x00ffffff));
                TextView btn = new TextView(this);
                btn.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
                btn.setTextColor(Color.WHITE);
                btn.setTextSize(20);
                int pd = 20;
                btn.setPadding(pd, pd / 2, pd, pd / 4);
                btn.setText(text);
                btn.setBackgroundDrawable(sd);
                btn.setOnClickListener(v -> {
                    editor.paste(text);
                });
                psBar.addView(btn);
            }
            editor.addNames(PRIMITIVE_TYPES);
            context.addToLua("context", this);
            configScriptContext();
        }

        Uri uri=intent.getData();
        String file=null;
        if(uri!=null){
            if("file".equals(uri.getScheme())){
                String path = uri.getPath();
                if(path!=null){
                    file=new File(path).getAbsolutePath();
                }
            }
        }else if(editor.lastFile()==null){
            file=new File(getPreferences(0).getString(LAST,
                    new File(Environment.getExternalStorageDirectory().getPath(),"test.lua").getPath())).getAbsolutePath();
        }
        if (Build.VERSION.SDK_INT>=23){
            requestPermissions(new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.READ_EXTERNAL_STORAGE},0);
        }
        loadFile(file);
        new Thread(()->{
            LibLoader.extractLibs(this, BuildConfig.VERSION_CODE);
        }).start();
    }

    private void configScriptContext() {
        StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder(StrictMode.getThreadPolicy()).permitNetwork().build());
        StringBuilder scriptBuilder = new StringBuilder();
        for (String type : PRIMITIVE_TYPES) {
            scriptBuilder.append(type).append('=')
                    .append(" class '").append(type).append("'\n");
        }
        context.run(scriptBuilder.toString());
        context.run("package.path='./?.lua;./?/init.lua;/sdcard/?.lua;/sdcard/?/init.lua;" +
                LibLoader.getLibDir(this) + "/?.lua" + "'\npackage.cpath='./?.so;./lib?.so;lib?.so;?.so'");


    }

    @SuppressLint("ApplySharedPref")
    private void loadFile(String path) {
        if(path==null) return;
        if(!new File(path).exists()){
            editor.setText("using 'java.lang'\n");
        }
        editor.open(path);
        getPreferences(0).edit().putString(LAST,path).commit();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        new MenuInflater(this).inflate(R.menu.main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,  String[] permissions,  int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        for(int i=permissions.length;i--!=0;){
            String permission =permissions[i];
            if(permission.equals(Manifest.permission.WRITE_EXTERNAL_STORAGE)&&grantResults[i]!= PackageManager.PERMISSION_GRANTED)
                finish();
            else loadFile(editor.lastFile());
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.run:
                runCode();
                break;
            case R.id.redo:
                editor.redo();
                break;
            case R.id.undo:
                editor.undo();
                break;
            case R.id.gotoLine:
                editor.startGotoMode();
                break;
            case R.id.search:
                editor.startFindMode();
                break;
            case R.id.search_library:
                if(classList==null)
                    classList=context.getClasses();
                editor.startLibrarySearchMode(classList);
                break;
            case R.id.view_result:
                showResult();
                break;
            case R.id.save:
                save();
                break;
            case R.id.open:{
                save();
                final FilePicker filePicker =new FilePicker(this, new File(editor.lastFile()),
                        pathname -> pathname.isDirectory() || pathname.getName().matches(".+(.lua)$"));
                filePicker.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 800));
                new AlertDialog.Builder(this,android.R.style.Theme_DeviceDefault_Light_Dialog).setView(filePicker).setNegativeButton(R.string.cancel,null).
                                setPositiveButton(R.string.confirm, (dialog2, which) -> {
                                    File f =filePicker.getSelectedFile();
                                    if(f!=null){
                                        loadFile(f.getAbsolutePath());
                                    }
                                }).show();
            }
            break;
            case R.id.help:
                startActivity(new Intent().setAction(Intent.ACTION_VIEW).setData(Uri.parse("https://github.com/qtiuto/lua-for-android/wiki")));
                break;
            case R.id.lua_doc:
                startActivity(new Intent().setAction(Intent.ACTION_VIEW).setData(Uri.parse("http://www.lua.org/manual/5.3/manual.html")));
                break;
            case R.id.root_mode:{
                if(item.isChecked()){
                    ((TransmitClient)(context)).close();
                    context=old;
                    item.setChecked(false);
                    Toast.makeText(this,
                            R.string.root_mode_exited,Toast.LENGTH_SHORT).show();
                }else {
                    new Thread(()->{
                        boolean switched=startRootMode();
                        handler.post(()->{
                            if(switched) item.setChecked(true);
                            Toast.makeText(this,switched?R.string.root_mode_ok:
                                    R.string.root_mode_failed,Toast.LENGTH_SHORT).show();
                        });
                    }).start();

                }

            }

        }
        return super.onOptionsItemSelected(item);
    }

    private static void appendLog(SpannableStringBuilder builder,CharSequence log,boolean isError){
        synchronized (builder){
            int start = builder.length();
            builder.append(log);
            builder.setSpan(new ForegroundColorSpan(isError?Color.RED:0xFF404040), start, builder.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
        }
    }

    private void runCode() {
        SpannableStringBuilder string = new SpannableStringBuilder();
        try {
            Logger out = (log, r) -> appendLog(string,log,false);
            Logger err = (log,r) -> appendLog(string,log,true);
            context.setLogger(out, err);
            String result= context.run(editor.getText().toString());
            context.flushLog();
            if (result != null) {
                StringBuilder res = new StringBuilder();
                res.append(getString(R.string.return_values)).append(":\n").append(result);
                appendLog(string,res,false);
            }

        } catch (Throwable e) {
            context.flushLog();
            appendLog(string,Log.getStackTraceString(e),true);
        }
        result = string;
        save();
        showResult();
        context.setLogger(null, null);
    }

    private void showResult() {
        handler.post(()->{
            if (popupWindow != null)
                popupWindow.dismiss();
            View v = View.inflate(this, R.layout.result, null);
            TextView resultText = v.findViewById(R.id.result);
            resultText.setText(result);
            popupWindow = new PopupWindow(v, ViewGroup.LayoutParams.MATCH_PARENT, getResources().getDisplayMetrics().heightPixels >> 1);
            popupWindow.setTouchable(true);
            popupWindow.setFocusable(false);
            popupWindow.setOutsideTouchable(true);
            popupWindow.setBackgroundDrawable(new ColorDrawable(Color.WHITE));
            popupWindow.showAtLocation(editor, Gravity.BOTTOM, 0, 0);
            popupWindow.setOnDismissListener(() -> popupWindow = null);
        });

    }

    @Override
    public void onBackPressed() {
        if (popupWindow != null) {
            popupWindow.dismiss();
        } else {
            save();
            super.onBackPressed();
        }
    }

    private void save() {
        editor.save(editor.lastFile());
    }

    private boolean startRootMode(){
        try (ServerSocket serverListener= getServerListener()){
            String apkPath=getApplicationInfo().sourceDir;
            final String cmd="app_process -Djava.class.path="+apkPath+" -Djava.library.path="+System.getProperty("java.library.path")+
                    ":"+getApplicationInfo().nativeLibraryDir +" "
                    +new File(apkPath).getParent()+" com.oslorde.luadroid.RootServer "+serverListener.getLocalPort();
            Runtime.getRuntime().exec("su -c "+cmd);
            try(Socket socket=serverListener.accept();
                DataInputStream in=new DataInputStream(socket.getInputStream())){
                int remotePort=in.readInt();
                old=context;
                context=new TransmitClient(remotePort);
            }
            return true;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

    }

    private ServerSocket getServerListener()  {
        int st=7899;
        Random random=new Random();
        for (;;){
            int port=Math.abs((random.nextInt()&0xffff)-st)+st;
            try {
                return new ServerSocket(port);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }
}
