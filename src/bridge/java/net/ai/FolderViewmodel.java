// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

package net.ai;

import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicBoolean;

public abstract class FolderViewmodel {
    public abstract void onLoad(FolderView view);

    public abstract int folderId();

    public abstract byte[] listThumbnailContent(int row);

    public abstract boolean listSelected(int section, int row);

    public abstract void listAction(int section, int row, int subIndex);

    public abstract void uploadFileOnly(String filename, String path, byte[] thumbnail);

    public abstract void uploadFilesOnly(ArrayList<String> filenames, ArrayList<String> paths, ArrayList<byte[]> thumbnails);

    private static final class CppProxy extends FolderViewmodel
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
        public void onLoad(FolderView view)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_onLoad(this.nativeRef, view);
        }
        private native void native_onLoad(long _nativeRef, FolderView view);

        @Override
        public int folderId()
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            return native_folderId(this.nativeRef);
        }
        private native int native_folderId(long _nativeRef);

        @Override
        public byte[] listThumbnailContent(int row)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            return native_listThumbnailContent(this.nativeRef, row);
        }
        private native byte[] native_listThumbnailContent(long _nativeRef, int row);

        @Override
        public boolean listSelected(int section, int row)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            return native_listSelected(this.nativeRef, section, row);
        }
        private native boolean native_listSelected(long _nativeRef, int section, int row);

        @Override
        public void listAction(int section, int row, int subIndex)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_listAction(this.nativeRef, section, row, subIndex);
        }
        private native void native_listAction(long _nativeRef, int section, int row, int subIndex);

        @Override
        public void uploadFileOnly(String filename, String path, byte[] thumbnail)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_uploadFileOnly(this.nativeRef, filename, path, thumbnail);
        }
        private native void native_uploadFileOnly(long _nativeRef, String filename, String path, byte[] thumbnail);

        @Override
        public void uploadFilesOnly(ArrayList<String> filenames, ArrayList<String> paths, ArrayList<byte[]> thumbnails)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_uploadFilesOnly(this.nativeRef, filenames, paths, thumbnails);
        }
        private native void native_uploadFilesOnly(long _nativeRef, ArrayList<String> filenames, ArrayList<String> paths, ArrayList<byte[]> thumbnails);
    }
}
