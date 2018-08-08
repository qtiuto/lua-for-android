package com.oslorde.luadroid;

import java.lang.ref.WeakReference;

class GCListener {
    private WeakReference<GCTracker> context;

    GCListener(WeakReference<GCTracker> context) {
        this.context = context;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        GCTracker tracker = context.get();
        if (tracker != null) {
            tracker.onNewGC(context);
        }
    }
}
