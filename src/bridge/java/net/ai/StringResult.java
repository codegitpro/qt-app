// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

package net.ai;

import java.util.concurrent.atomic.AtomicBoolean;

public abstract class StringResult {
    public abstract void onResult(String content);

    private static final class CppProxy extends StringResult
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

        @Override
        public void onResult(String content)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_onResult(this.nativeRef, content);
        }
        private native void native_onResult(long _nativeRef, String content);
    }
}
