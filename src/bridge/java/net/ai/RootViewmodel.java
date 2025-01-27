// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

package net.ai;

import java.util.concurrent.atomic.AtomicBoolean;

public abstract class RootViewmodel {
    public static RootViewmodel create(Worker scheduler, String appDataLocation, int thumbPx, OsIo io, View view, ScreenType type, UiCallback callback, Diagnostic logger)
    {
        return CppProxy.create(scheduler,
                               appDataLocation,
                               thumbPx,
                               io,
                               view,
                               type,
                               callback,
                               logger);
    }

    private static final class CppProxy extends RootViewmodel
    {
        private final long nativeRef;
        private final AtomicBoolean destroyed = new AtomicBoolean(false);

        private CppProxy(long nativeRef)
        {
            if (nativeRef == 0) throw new RuntimeException("nativeRef is zero");
            this.nativeRef = nativeRef;
        }

        private native void nativeDestroy(long nativeRef);
        public void _djinni_private_destroy()
        {
            boolean destroyed = this.destroyed.getAndSet(true);
            if (!destroyed) nativeDestroy(this.nativeRef);
        }
        protected void finalize() throws java.lang.Throwable
        {
            _djinni_private_destroy();
            super.finalize();
        }

        public static native RootViewmodel create(Worker scheduler, String appDataLocation, int thumbPx, OsIo io, View view, ScreenType type, UiCallback callback, Diagnostic logger);
    }
}
