package com.oslorde.luadroid;

import android.content.Context;
import android.support.test.InstrumentationRegistry;
import android.support.test.runner.AndroidJUnit4;
import org.junit.Test;
import org.junit.runner.RunWith;

import static org.junit.Assert.assertEquals;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    @Test
    public void useAppContext() {
        // Context of the app under test.
        Context appContext = InstrumentationRegistry.getTargetContext();

        assertEquals("com.oslorde.luadroid.test", appContext.getPackageName());
       /* try {
            for (String name : getClasses()) {
                Method[] methods = Class.forName(name).getDeclaredMethods();
                for (Method m : methods) {
                    if (binarySearchMethod(methods, m.getName(), isDirect(m.getModifiers())) < 0) {
                        System.err.println(m);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }*/
    }
}
