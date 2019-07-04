package com.oslorde.luadroid.ui;

import android.content.Context;
import android.content.Intent;
import android.content.res.TypedArray;
import android.graphics.Typeface;
import android.net.Uri;
import android.text.Editable;
import android.text.Selection;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.ActionMode;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.AutoCompleteTextView;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.Filter;
import android.widget.Filterable;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.myopicmobile.textwarrior.android.FreeScrollingTextField;
import com.myopicmobile.textwarrior.android.YoyoNavigationMethod;
import com.myopicmobile.textwarrior.common.ColorScheme;
import com.myopicmobile.textwarrior.common.ColorSchemeDark;
import com.myopicmobile.textwarrior.common.ColorSchemeLight;
import com.myopicmobile.textwarrior.common.Document;
import com.myopicmobile.textwarrior.common.DocumentProvider;
import com.myopicmobile.textwarrior.common.LanguageLua;
import com.myopicmobile.textwarrior.common.Lexer;
import com.myopicmobile.textwarrior.common.LinearSearchStrategy;
import com.myopicmobile.textwarrior.common.ReadTask;
import com.oslorde.luadroid.R;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class LuaEditor extends FreeScrollingTextField {

    private boolean _isWordWrap;

    private String mLastSelectedFile;

    private int _index;

    private ActionMode.Callback lastLibraryMode;

    public LuaEditor(Context context) {
        super(context);
        init(context);
    }


    public LuaEditor(Context context, AttributeSet set) {
        super(context, set);
        init(context);
    }

    private void init(Context context) {
        setTypeface(Typeface.MONOSPACE);
        DisplayMetrics dm = context.getResources().getDisplayMetrics();

        float size = TypedValue.applyDimension(2, BASE_TEXT_SIZE_PIXELS, dm);
        setTextSize((int) size);
        setShowLineNumbers(true);
        setHighlightCurrentRow(true);
        setWordWrap(false);
        setAutoIndentWidth(2);
        Lexer.setLanguage(LanguageLua.getInstance());
        setNavigationMethod(new YoyoNavigationMethod(this));
        TypedArray array = getContext().getTheme().obtainStyledAttributes(new int[]{
                android.R.attr.colorBackground,
                android.R.attr.textColorPrimary,
                android.R.attr.textColorHighlight,
        });
        int backgroundColor = array.getColor(0, 0xFF00FF);
        int textColor = array.getColor(1, 0xFF00FF);
        int textColorHighlight = array.getColor(2, 0xFF00FF);
        array.recycle();
        setTextColor(textColor);
        setTextHighlightColor(textColorHighlight);
    }

    @Override
    protected void onLayout(boolean changed, int left, int top, int right, int bottom) {
       
        super.onLayout(changed, left, top, right, bottom);
        if (_index != 0 && right > 0) {
            moveCaret(_index);
            _index = 0;
        }
    }

    public void setDark(boolean isDark) {
        if (isDark)
            setColorScheme(new ColorSchemeDark());
        else
            setColorScheme(new ColorSchemeLight());
    }

    public void addNames(String[] names) {
        LanguageLua lang = (LanguageLua) Lexer.getLanguage();
        String[] old = lang.getNames();
        String[] news = new String[old.length + names.length];
        System.arraycopy(old, 0, news, 0, old.length);
        System.arraycopy(names, 0, news, old.length, names.length);
        lang.setNames(news);
        Lexer.setLanguage(lang);
        respan();
        invalidate();

    }

    public void setPanelBackgroundColor(int color) {
       
        _autoCompletePanel.setBackgroundColor(color);
    }

    public void setPanelTextColor(int color) {
       
        _autoCompletePanel.setTextColor(color);
    }

    public void setKeywordColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.KEYWORD, color);
    }

    public void setUserwordColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.LITERAL, color);
    }

    public void setBasewordColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.NAME, color);
    }

    public void setStringColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.STRING, color);
    }

    public void setCommentColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.COMMENT, color);
    }

    public void setBackgroundColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.BACKGROUND, color);
    }

    public void setTextColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.FOREGROUND, color);
    }

    public void setTextHighlightColor(int color) {
        getColorScheme().setColor(ColorScheme.Colorable.SELECTION_BACKGROUND, color);
    }

    public String getSelectedText() {
        return _hDoc.subSequence(getSelectionStart(), getSelectionEnd() - getSelectionStart()).toString();
    }

    @Override
    public boolean onKeyShortcut(int keyCode, KeyEvent event) {
        final int filteredMetaState = event.getMetaState() & ~KeyEvent.META_CTRL_MASK;
        if (KeyEvent.metaStateHasNoModifiers(filteredMetaState)) {
            switch (keyCode) {
                case KeyEvent.KEYCODE_A:
                    selectAll();
                    return true;
                case KeyEvent.KEYCODE_X:
                    cut();
                    return true;
                case KeyEvent.KEYCODE_C:
                    copy();
                    return true;
                case KeyEvent.KEYCODE_V:
                    paste();
                    return true;
            }
        }
        return super.onKeyShortcut(keyCode, event);
    }

    public void startGotoMode() {
        startActionMode(new ActionMode.Callback() {

            private int idx;

            private EditText edit;

            @Override
            public boolean onCreateActionMode(ActionMode mode, Menu menu) {

                mode.setTitle(getContext().getString(R.string.gotoLine));
                mode.setSubtitle(null);

                edit = new EditText(getContext()) {
                    @Override
                    public void onTextChanged(CharSequence s, int start, int before, int count) {
                        if (s.length() > 0) {
                            idx = 0;
                            _gotoLine();
                        }
                    }

                };

                edit.setSingleLine(true);
                edit.setInputType(2);
                edit.setImeOptions(2);
                edit.setOnEditorActionListener((p1, p2, p3) -> {
                    _gotoLine();
                    return true;
                });
                edit.setLayoutParams(new LayoutParams(getWidth() /3, -1));
                menu.add(0, 1, 0, "").setActionView(edit);
                menu.add(0, 2, 0, getContext().getString(R.string.ok));
                edit.requestFocus();
                return true;
            }

            private void _gotoLine() {
                String s = edit.getText().toString();
                if (s.isEmpty())
                    return;

                int l = Integer.parseInt(s);
                if (l > _hDoc.getRowCount()) {
                    l = _hDoc.getRowCount();
                }
                gotoLine(l);
            }

            @Override
            public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
                return false;
            }

            @Override
            public boolean onActionItemClicked(ActionMode mode, MenuItem item) {

                switch (item.getItemId()) {
                    case 1:
                        break;
                    case 2:
                        _gotoLine();
                        return true;

                }
                return false;
            }

            @Override
            public void onDestroyActionMode(ActionMode p1) {

            }
        });
    }


    public void startLibrarySearchMode(IClassList classList){
        lastLibraryMode = new ActionMode.Callback() {
            AutoCompleteTextView mText;
            List<String> classes=Collections.emptyList();

            class Adp extends BaseAdapter implements Filterable {
                LayoutInflater inflater;
                @Override
                public int getCount() {
                    return classes.size();
                }

                @Override
                public Object getItem(int position) {
                    String s = classes.get(position);
                    return s.substring(s.lastIndexOf('.')+1);
                }

                @Override
                public long getItemId(int position) {
                    return classes.get(position).hashCode();
                }

                @Override
                public View getView(int position, View convertView, ViewGroup parent) {

                    if(convertView==null){
                        if(inflater==null)
                            inflater=LayoutInflater.from(parent.getContext());
                        convertView=inflater.inflate(R.layout.class_item,null);
                        convertView.findViewById(R.id.go).setOnClickListener(v -> {
                            String clazz= (String) v.getTag();
                            try {
                                JClass cl=classList.resolveClass(clazz);
                                startClassMode(cl);
                            } catch (Exception e) {
                                Toast.makeText(getContext(),R.string.not_found,Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                    TextView text =  convertView.findViewById(R.id.text);
                    String clazz = classes.get(position);
                    text.setText(clazz);
                    ImageView view=convertView.findViewById(R.id.go);
                    view.setTag(clazz);
                    return convertView;
                }

                @Override
                public Filter getFilter() {
                    return new Filter() {
                        @Override
                        protected FilterResults performFiltering(CharSequence constraint) {
                            FilterResults results=new FilterResults();
                            if(TextUtils.isEmpty(constraint)){
                                results.count=0;
                                results.values=Collections.emptyList();
                            }else {
                                List<String> classes=classList.findClassWithPrefix(constraint.toString());
                                results.count=classes.size();
                                results.values=classes;
                            }
                            return results;
                        }

                        @Override
                        protected void publishResults(CharSequence constraint, FilterResults results) {
                            classes= (List<String>) results.values;
                            if(results.count>0){
                                notifyDataSetChanged();
                            }else notifyDataSetInvalidated();
                        }
                    };
                }
            }

            @Override
            public boolean onCreateActionMode(ActionMode mode, Menu menu) {
                CharSequence lastText=null;
                if(mText!=null)
                    lastText=mText.getText();
                mText = new AutoCompleteTextView(getContext());
                mText.setAdapter(new Adp());
                mText.setOnItemClickListener((parent, view, position, id) -> {
                    paste(classes.get(position));
                    mText.setText(parent.getAdapter().getItem(position).toString());
                });
                mText.setHint(R.string.library_hint);
                ViewGroup.MarginLayoutParams params=new ViewGroup.MarginLayoutParams(getWidth()/3*2,ViewGroup.LayoutParams.MATCH_PARENT);
                params.rightMargin=40;
                mText.setSingleLine();
                mText.setLayoutParams(params);
                if(lastText!=null){
                    mText.clearComposingText();
                    mText.setText(lastText);
                    Editable spannable = mText.getText();
                    Selection.setSelection(spannable, spannable.length());
                    post(()->mText.showDropDown());
                }
                menu.add(0,0,0,"").setActionView(mText);
                menu.add(0,1,0,android.R.string.search_go).setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
                return true;
            }

            @Override
            public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
                return false;
            }

            @Override
            public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
                switch (item.getItemId()){
                    case 1:
                        mText.showDropDown();
                }
                return false;
            }

            @Override
            public void onDestroyActionMode(ActionMode mode) {

            }
        };
        startActionMode(lastLibraryMode);
    }



    private void startClassMode(JClass c){
        final boolean isPublicClass = Modifier.isPublic(c.getModifiers());
        startActionMode(new ActionMode.Callback() {
            AutoCompleteTextView mText;

            List<JMember> members =Collections.emptyList();

            class Adp extends BaseAdapter implements Filterable {
                LayoutInflater inflater;
                List<String> memberTextList;
                @Override
                public int getCount() {
                    return memberTextList.size();
                }

                @Override
                public Object getItem(int position) {
                    return  members.get(position).getName();
                }

                @Override
                public long getItemId(int position) {
                    return memberTextList.get(position).hashCode();
                }

                @Override
                public View getView(int position, View convertView, ViewGroup parent) {
                    if(convertView==null){
                        if(inflater==null)
                            inflater=LayoutInflater.from(parent.getContext());
                        convertView=inflater.inflate(R.layout.member_item,null);
                    }
                    TextView tx=convertView.findViewById(R.id.text);
                    tx.setText(memberTextList.get(position));
                    return convertView;
                }



                @Override
                public Filter getFilter() {
                    return new Filter() {
                        @Override
                        protected FilterResults performFiltering(CharSequence constraint) {
                            FilterResults results = new FilterResults();
                            if (TextUtils.isEmpty(constraint)) {
                                results.count = 0;
                                results.values = Collections.emptyList();
                            } else {
                                try {
                                    List<JMember> members =c.filterMembers(constraint.toString());
                                    results.values = members;
                                    results.count = members.size();
                                }catch (Exception e){
                                    e.printStackTrace();
                                }

                            }
                            return results;
                        }

                        @Override
                        protected void publishResults(CharSequence constraint, FilterResults results) {
                            if(results.values==null) results.values=Collections.emptyList();
                            members= (List<JMember>) results.values;
                            ArrayList<String> memberTexts=new ArrayList<>(members.size());
                            for (JMember member : members) {
                                memberTexts.add(member.getDes());
                            }
                            memberTextList=memberTexts;
                            if(results.count>0){
                                notifyDataSetChanged();
                            }else notifyDataSetInvalidated();
                        }
                    };
                }
            }

            @Override
            public boolean onCreateActionMode(ActionMode mode, Menu menu) {
                mText = new AutoCompleteTextView(getContext());
                mText.setAdapter(new Adp());
                mText.setThreshold(1);
                mText.setOnItemClickListener((parent, view, position, id) ->{
                    JMember member=members.get(position);
                    if(member .isField()){
                        paste(member.getName());
                    }else if(member .isConstructor()){
                        paste("()");
                        moveCaretLeft();
                    }else{
                        paste(member.getName()+"()");
                        moveCaretLeft();
                    }
                });
                mText.setHint(R.string.class_hint);
                mText.setSingleLine();
                mText.setLayoutParams(new LayoutParams(getWidth()*7/10,LayoutParams.MATCH_PARENT));
                menu.add(0,0,0,"").setActionView(mText);
                if(isPublicClass){
                    menu.add(0,1,0,R.string.doc).setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
                }
                return true;
            }

            @Override
            public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
                return false;
            }

            @Override
            public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
                switch (item.getItemId()){
                    case 1:
                        getContext().startActivity(new Intent().setAction(Intent.ACTION_VIEW).setData(Uri.parse(
                                "https://developer.android.com/reference/"+c.getName().replace('.','/').replace('$','.'))));
                }
                return false;
            }

            @Override
            public void onDestroyActionMode(ActionMode mode) {
                post(()->startActionMode(lastLibraryMode));
            }
        });
    }

    public void startFindMode() {
       
        startActionMode(new ActionMode.Callback() {

            private LinearSearchStrategy finder;

            private int idx;

            private EditText edit;

            @Override
            public boolean onCreateActionMode(ActionMode mode, Menu menu) {
                mode.setTitle(getContext().getString(R.string.search));
                mode.setSubtitle(null);

                edit = new EditText(getContext()) {
                    @Override
                    public void onTextChanged(CharSequence s, int start, int before, int count) {
                        if (s.length() > 0) {
                            idx = 0;
                            findNext();
                        }else selectText(false);
                    }
                };
                edit.setSingleLine(true);
                edit.setImeOptions(3);
                edit.setOnEditorActionListener((p1, p2, p3) -> {
                    findNext();
                    return true;
                });
                edit.setLayoutParams(new LayoutParams(getWidth() / 3*2, -1));
                menu.add(0, 1, 0, "").setActionView(edit);
                menu.add(0, 2, 0, getContext().getString(R.string.next));
                edit.requestFocus();
                return true;
            }

            @Override
            public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
                 return false;
            }

            @Override
            public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
               
                switch (item.getItemId()) {
                    case 1:
                        break;
                    case 2:
                        findNext();
                        break;

                }
                return false;
            }

            private void findNext() {
                finder = new LinearSearchStrategy();
                String kw = edit.getText().toString();
                if (kw.isEmpty()) {
                    selectText(false);
                    return;
                }
                idx = finder.find(getText(), kw, idx, getText().length(), false, false);
                if (idx == -1) {
                    selectText(false);
                    Toast.makeText(getContext(), R.string.not_found, Toast.LENGTH_SHORT).show();
                    idx = 0;
                    return;
                }
                setSelection(idx, edit.getText().length());
                idx += edit.getText().length();
                moveCaret(idx);
            }

            @Override
            public void onDestroyActionMode(ActionMode p1) {
               
            }
        });

    }

    @Override
    public void setWordWrap(boolean enable) {
       
        _isWordWrap = enable;
        super.setWordWrap(enable);
    }

    public DocumentProvider getText() {
        return createDocumentProvider();
    }

    public void setText(CharSequence c) {
        //TextBuffer text=new TextBuffer();
        Document doc = new Document(this);
        doc.setWordWrap(_isWordWrap);
        doc.setText(c);
        setDocumentProvider(new DocumentProvider(doc));
        //doc.analyzeWordWrap();
    }

    public void insert(int idx, String text) {
        selectText(false);
        moveCaret(idx);
        paste(text);
    }

    public void setSelection(int index) {
        selectText(false);
        if (!hasLayout())
            moveCaret(index);
        else
            _index = index;
    }

    public void gotoLine(int line) {
        if (line > _hDoc.getRowCount()) {
            line = _hDoc.getRowCount();
        }
        int i = getText().getLineOffset(line - 1);
        setSelection(i);
    }

    public void undo() {
        DocumentProvider doc = createDocumentProvider();
        int newPosition = doc.undo();

        if (newPosition >= 0) {
            setEdited(true);

            respan();
            selectText(false);
            moveCaret(newPosition);
            invalidate();
        }

    }

    public void redo() {
        DocumentProvider doc = createDocumentProvider();
        int newPosition = doc.redo();

        if (newPosition >= 0) {
            setEdited(true);

            respan();
            selectText(false);
            moveCaret(newPosition);
            invalidate();
        }

    }

    public String lastFile(){
        return mLastSelectedFile;
    }

    public void open(String filename) {
        mLastSelectedFile = filename;
        File inputFile = new File(filename);
        if(!inputFile.exists()) return;
        new ReadTask(this, inputFile).start();
    }

    public void save(String filename) {
        File outputFile = new File(filename);

        if (outputFile.exists()) {
            if (!outputFile.canWrite()) {
                return;
            }
        }
        try (FileOutputStream stream = new FileOutputStream(filename)){
            stream.write(getText().toString().getBytes());
            stream.flush();
            stream.getFD().sync();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

}
