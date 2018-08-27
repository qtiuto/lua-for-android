package com.oslorde.luadroid.ui;

import android.app.Activity;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.StateListDrawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
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

import com.oslorde.luadroid.R;
import com.oslorde.luadroid.ScriptContext;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;

/**
 * Created by Karven on 2018/2/11.
 */

public class MainActivity extends Activity {

    static {
        System.loadLibrary("luadroid");

    }

    LuaEditor editor;
    ScriptContext context = new ScriptContext();
    LinearLayout psBar;
    String[] ps = {"(", ")", "[", "]", "{", "}", "\"", "=", ":", ".", ",", "_", "+", "-", "*", "/", "\\", "%", "#", "^", "$", "?", "<", ">", "~", ";", "'"};
    CharSequence result;
    private PopupWindow popupWindow;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
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
            int pd = (int) (btn.getTextSize() / 2);
            btn.setPadding(pd, pd / 2, pd, pd / 4);
            btn.setText(text);
            btn.setBackground(sd);
            btn.setOnClickListener(v -> editor.paste(text));
            psBar.addView(btn);
        }
        context.addToLua("context", this, true);
        new Thread(() -> {
            File file = new File("/sdcard/test.lua");
            if (file.exists()) {
                try {
                    FileInputStream in = new FileInputStream(file);
                    byte[] bytes = new byte[in.available()];
                    in.read(bytes);
                    new Handler(Looper.getMainLooper()).post(() -> {
                        String text = new String(bytes);
                        editor.setText(text);
                    });
                    in.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        new MenuInflater(this).inflate(R.menu.main, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.run:

                SpannableStringBuilder string = new SpannableStringBuilder();
                final int textColor = 0xFF404040;
                try {
                    OutputStream out = new OutputStream() {
                        @Override
                        public void write(int b) {
                        }

                        @Override
                        public void write(byte[] b) {
                            int start = string.length();
                            string.append(new String(b));
                            string.setSpan(new ForegroundColorSpan(textColor), start, string.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);

                        }
                    };
                    OutputStream err = new OutputStream() {
                        @Override
                        public void write(int b) {
                        }

                        @Override
                        public void write(byte[] b) {
                            int start = string.length();
                            string.append(new String(b));
                            string.setSpan(new ForegroundColorSpan(Color.RED), start, string.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
                        }
                    };
                    context.setLogger(out, err);
                    Object[] results = context.run(editor.getText().toString());
                    context.flushLog();
                    if (string.length() > 0 && string.charAt(string.length() - 1) != '\n')
                        string.append("\n");
                    if (results != null) {
                        StringBuilder res = new StringBuilder();
                        for (int i = 0, len = results.length; i < len; ++i) {
                            res.append(i + 1).append(". ").append(results[i]).append('\n');
                        }
                        res.deleteCharAt(res.length() - 1);
                        int start = string.length();
                        string.append(res);
                        string.setSpan(new ForegroundColorSpan(textColor), start, string.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
                    }

                } catch (Throwable e) {
                    context.flushLog();
                    int start = string.length();
                    string.append(Log.getStackTraceString(e));
                    string.setSpan(new ForegroundColorSpan(Color.RED), start, string.length(), Spanned.SPAN_INCLUSIVE_EXCLUSIVE);
                }
                result = string;
                context.setLogger(null, null);
                save();
                showResult();
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
            case R.id.view_result:
                showResult();
                break;

        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        //if(hasFocus){
        //}
    }

    private void showResult() {
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
        try {
            FileOutputStream out = new FileOutputStream("/sdcard/test.lua");
            out.write(editor.getText().toString().getBytes());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
