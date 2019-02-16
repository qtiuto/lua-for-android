package com.myopicmobile.textwarrior.common;

import android.app.ProgressDialog;
import android.os.AsyncTask;

import com.oslorde.luadroid.R;
import com.oslorde.luadroid.ui.LuaEditor;

import java.io.File;
import java.io.FileInputStream;
import java.lang.ref.WeakReference;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;

public class ReadTask extends AsyncTask<File,Integer,CharSequence> {
    private ProgressDialog _dlg;
    private WeakReference<LuaEditor> _edit;
    private File _file;
    private long _len;

    public ReadTask(LuaEditor edit, File file) {
        _file = file;
        _len = _file.length();
        _edit = new WeakReference<>(edit);
        _dlg = new ProgressDialog(edit.getContext());
        _dlg.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
        _dlg.setTitle(edit.getContext().getString(R.string.Opening));
        _dlg.setIcon(android.R.drawable.ic_dialog_info);
        _dlg.setIndeterminate(true);
    }

    public void start() {
        execute();
        _dlg.show();
    }

    @Override
    protected CharSequence doInBackground(File... p1) {
        try (FileChannel channel=new FileInputStream(_file).getChannel()){
            return Charset.defaultCharset().decode(
                    channel.map(FileChannel.MapMode.READ_ONLY,0,_len));
        } catch (Exception e) {
            _dlg.setMessage(e.getMessage());
        }
        return "";
    }

    @Override
    protected void onPostExecute(CharSequence result) {
        LuaEditor editor = _edit.get();
        if(editor!=null)
            editor.setText(result);
        _dlg.dismiss();
    }

    @Override
    protected void onProgressUpdate(Integer[] values) {
        _dlg.setProgress(values[0]);
    }




}
