package com.oslorde.luadroid;

import java.lang.ref.WeakReference;

interface GCTracker {
    void onNewGC(WeakReference<GCTracker> reference);
}
