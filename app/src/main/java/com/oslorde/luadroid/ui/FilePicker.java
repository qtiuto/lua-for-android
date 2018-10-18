package com.oslorde.luadroid.ui;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Environment;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;
import com.oslorde.luadroid.R;

import java.io.File;
import java.io.FileFilter;
import java.text.Collator;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;



public class FilePicker extends LinearLayout {

    static class Item{
        String name;
        boolean isDir;

        Item(String name, boolean isDir) {
            this.name = name;
            this.isDir = isDir;
        }
    }
    static class Holder{
        ImageView image;
        TextView des;
    }
    private final File rootDir ;
    private TextView curDirView;
    private ListView curFileList;
    private FileFilter filter;
    private File curDir;
    private String selected;
    private Collator collator=Collator.getInstance();

    class FileAdapter extends BaseAdapter implements AdapterView.OnItemClickListener,Comparator<File>{
        private Drawable folder;
        private Drawable file;
        private LayoutInflater inflater;
        private Item[] mResults;
        void init(){
            updateCurDir();
            curFileList.setOnItemClickListener(this);
        }
        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            Item item=mResults[position];
            File clicked=new File(curDir,item.name);
            if(item.name.equals("..")){
                clicked=curDir.getParentFile();
            }
            if(item.isDir){
                curDir=clicked;
                updateCurDir();
            }else {
                selected=item.name;
                notifyDataSetChanged();
            }
        }
        @Override
        public int getCount() {
            return mResults==null?0:mResults.length;
        }

        @Override
        public Object getItem(int position) {
            return mResults[position];
        }

        @Override
        public long getItemId(int position) {
            return getItem(position).hashCode();
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if(convertView==null){
                if(inflater==null){
                    inflater=LayoutInflater.from(parent.getContext()) ;
                    folder=parent.getContext().getResources().getDrawable(R.drawable.ic_folder);
                    file=parent.getContext().getResources().getDrawable(R.drawable.ic_file);
                }
                convertView=inflater.inflate(R.layout.file_item,parent,false);
                Holder holder=new Holder();
                convertView.setTag(holder);
                holder.image= (ImageView) ((ViewGroup)convertView).getChildAt(0);
                holder.des= (TextView) ((ViewGroup)convertView).getChildAt(1);
            }
            Holder holder= (Holder) convertView.getTag();
            String name = mResults[position].name;
            holder.des.setText(name);
            holder.image.setImageDrawable(mResults[position].isDir?folder:file);
            if(name.equals(selected))
                convertView.setBackgroundColor(Color.DKGRAY);
            else convertView.setBackgroundColor(Color.WHITE);
            return convertView;
        }
        @Override
        public int compare(File o1, File o2) {
            if (o1.isDirectory()){
                if (o2.isDirectory()) return collator.compare(o1.getName(),o2.getName());
                return -1;
            }else {
                if (o2.isFile()) return collator.compare(o1.getName(),o2.getName());
                return 1;
            }
        }
        void updateCurDir(){
            curDirView.setText(curDir.getPath());
            File[] files= curDir.listFiles(filter);
            Arrays.sort(files, this);
            ArrayList<Item> result=new ArrayList<>(files.length+1);
            if(!curDir.equals(rootDir))
                result.add(new Item("..",true));
            for (File file:files){
                result.add(new Item(file.getName(),file.isDirectory()));
            }
            mResults=result.toArray(new Item[result.size()]);
            selected=null;
            notifyDataSetChanged();
        }
    }

    public FilePicker(Context context, FileFilter filter) {
        this(context,null,filter);
    }

    public FilePicker(Context context, File curDir, FileFilter filter) {
        super(context);
        this.curDir=resolveCurDir(context,curDir);
        this.rootDir=resolveRootDir(this.curDir);
        this.filter=filter;
        TextView textView=new TextView(context);
        textView.setSingleLine(true);
        textView.setEllipsize(TextUtils.TruncateAt.START);
        curDirView=textView;
        MarginLayoutParams params=new MarginLayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
        params.leftMargin= dpToPx(5,context.getResources());
        textView.setLayoutParams(params);
        setOrientation(VERTICAL);
        addView(curDirView);
        curFileList=new ListView(context);
        FileAdapter adapter = new FileAdapter();
        curFileList.setAdapter(adapter);
        adapter.init();
        addView(curFileList);

    }

    private static File resolveRootDir(File curDir){
        File dir=curDir;
        while (true){
            File parentFile=dir.getParentFile();
            if(parentFile!=null&&parentFile.exists()&&parentFile.canRead()&&parentFile.canExecute()){
                dir=parentFile;
            } else {
                return dir;
            }
        }
    }

    private static File resolveCurDir(Context context,File curDir){
        if(curDir==null) return getReadableDir(context);
        if(curDir.isFile()) curDir=curDir.getParentFile();
        if(!curDir.canRead()||!curDir.canExecute())
            return getReadableDir(context);
        return curDir;
    }

    private static File getReadableDir(Context context){
        File file=Environment.getExternalStorageDirectory();
        if(file.canRead()&&file.canExecute()) return file;
        file= context.getExternalFilesDir(null);
        if(file!=null&&file.canRead()&&file.canExecute()) return file;
        else return context.getFilesDir();
    }

    public static int dpToPx(float dp, Resources resources) {
        float px = dp*resources.getDisplayMetrics().density+0.5f;
        return (int) px;
    }

    @Override
    public void onWindowFocusChanged(boolean hasWindowFocus) {
        super.onWindowFocusChanged(hasWindowFocus);
        int maxHeight = getResources().getDisplayMetrics().heightPixels - dpToPx(200, getResources());
        int measuredHeight = getMeasuredHeight();
        if (measuredHeight > maxHeight){
            ViewGroup.LayoutParams params=getLayoutParams();
            params.height=maxHeight;
            setLayoutParams(params);
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

    }

    public File getSelectedFile(){
        return selected==null?null:new File(curDir,selected);
    }
}
