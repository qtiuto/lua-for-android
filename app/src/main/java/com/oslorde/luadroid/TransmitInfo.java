package com.oslorde.luadroid;

import java.lang.reflect.Method;
import java.util.Comparator;

public interface TransmitInfo {
    int CONNECT_MAGIC = 0x888;
    int PRINT_MAGIC = 0x666;
    int METHOD_MAGIC = 0x1234;
    int CLEAN_MAGIC=0x1245;
    int HANDLE_RUN = 1;
    int HANDLE_RUN_FILE = 2;
    int HANDLE_CLASSES = 3;
    int HANDLE_FLUSH_LOG = 4;
    String TAG="Osl_transmit";

    Comparator<Method> METHOD_COMPARATOR = (m1, m2) -> {
        int ret = m1.getName().compareTo(m2.getName());
        if (ret == 0) {
            Class[] paras = m1.getParameterTypes();
            Class[] pars = m2.getParameterTypes();
            ret = paras.length - pars.length;
            if (ret == 0) {
                for (int i = paras.length; i-- != 0 && ret == 0; ) {
                    ret = paras[i].getName().compareTo(pars[i].getName());
                }
            }
        }
        return ret;
    };
}
