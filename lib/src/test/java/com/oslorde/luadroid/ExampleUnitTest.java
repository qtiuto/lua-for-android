package com.oslorde.luadroid;

import org.junit.Assert;
import org.junit.Test;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
public class ExampleUnitTest {
    @Test
    public void pattern_match(){
        Assert.assertTrue("/system/d/core.jar".matches(".+(\\.jar|\\.zip|\\.apk)$"));
    }
    @Test
    public void addition_isCorrect() throws IOException {
        File file =new File("D:\\1.txt");
        byte[] buf=new byte[(int) file.length()];
        new FileInputStream(file).read(buf);
        String s=new String(buf);
        Pattern pattern=Pattern.compile("### ([ \\w\\-]+)");
        Matcher m=pattern.matcher(s);
        StringBuilder builder=new StringBuilder();
        int end=0;
        while (m.find()){
            builder.append(s,end,m.start(1));
            String key=m.group(1);
            key=key.replace("*","");
            builder.append(String.format("[%s](%s)",key,key));
            end=m.end(1);
        }
        builder.append(s,end,s.length());
        new FileOutputStream(file).write(builder.toString().getBytes());
    }
}