package com.oslorde.luadroid.set;

public class SetUtils {
    public static  int binaryCeil(int i) {
        i |= (i >> 1);
        i |= (i >> 2);
        i |= (i >> 4);
        i |= (i >> 8);
        i |= (i >> 16);

        return i + 1;
    }
    public static  boolean notPowerOfTwo(int n) {
        return ((n & (n - 1)) != 0);
    }

}
