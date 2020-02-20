package net.ai.tmp.cloudstore.implementation;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.ThumbnailUtils;
import android.os.AsyncTask;
import android.provider.MediaStore;
import android.util.Log;

import net.ai.BinaryResult;
import net.ai.BoolResult;
import net.ai.CancellationToken;
import net.ai.FileType;
import net.ai.Header;
import net.ai.HttpProgressResult;
import net.ai.HttpResult;
import net.ai.HttpVerb;
import net.ai.LogType;
import net.ai.LongResult;
import net.ai.OsIo;
import net.ai.StringResult;
import net.ai.StringsResult;
import net.ai.VoidResult;

import net.ai.tmp.cloudstore.Util;
import net.ai.tmp.cloudstore.http.HttpDownloadTask;
import net.ai.tmp.cloudstore.http.HttpRequestTask;
import net.ai.tmp.cloudstore.http.HttpUploadTask;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class MainOsIo extends OsIo
{
    private final ThreadPoolExecutor shortTasks = new ThreadPoolExecutor(2, 2, 1, TimeUnit.SECONDS, new LinkedBlockingDeque<Runnable>());
    private final ThreadPoolExecutor fileTasks = new ThreadPoolExecutor(1, 2, 1, TimeUnit.SECONDS, new LinkedBlockingDeque<Runnable>());
    private final ThreadPoolExecutor longTasks = new ThreadPoolExecutor(4, 4, 1, TimeUnit.SECONDS, new LinkedBlockingDeque<Runnable>());
    private final String contentPath;
    private final Context ctx;

    public MainOsIo(Context ctx, String contentPath)
    {
        this.ctx = ctx;
        this.contentPath = contentPath;
        shortTasks.allowCoreThreadTimeOut(true);
        fileTasks.allowCoreThreadTimeOut(true);
        longTasks.allowCoreThreadTimeOut(true);
    }


    private byte[] fileReadbinaryDirect(final String path)
    {
        try
        {
            return Util.readBytesFromFile(path);
        }
        catch (Exception e)
        {
        }
        return null;
    }

    public void fileReadbinary(final String path, final IBinaryResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                byte[] data = fileReadbinaryDirect(path);
                result.onResult(null == data ? new byte[0] : data);
            }
        });
    }

    @Override
    public void log(LogType type, int line, String file, String message) {
        switch (type)
        {
            case LVERBOSE: Log.v(file, "["+line+"] " + message); return;
            case LDEBUG: Log.d(file, "["+line+"] " + message); return;
            case LINFO: Log.i(file, "["+line+"] " + message); return;
            case LWARNING: Log.w(file, "["+line+"] " + message); return;
            default: Log.e(file, "["+line+"] " + message); return;
        }
    }

    @Override
    public void fileReadall(final String path, final BinaryResult result)
    {
        fileReadbinary(path, new IBinaryResult() {
            @Override
            public void onResult(byte[] data) {
                result.onResult(data);
            }
        });
    }

    private boolean fileWriteallDirect(final String path, final byte[] content)
    {
        try
        {
            Util.io_write_and_close(null != content ? content : new byte[0], new FileOutputStream(path));
            return true;
        }
        catch (IOException e)
        {
        }
        return false;
    }

    public void fileWriteall(final String path, final byte[] content, final IBoolResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                result.onResult(fileWriteallDirect(path, content));
            }
        });
    }

    @Override
    public void fileWriteall(final String path, final byte[] content, final BoolResult result)
    {
        fileWriteall(path, content, new IBoolResult() {
            @Override
            public void onResult(boolean success) {
                result.onResult(success);
            }
        });
    }

    public void fileWriteallSafely(final String path, final byte[] content, final IBoolResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                String backupPath = path + ".bak";
                try
                {
                    Util.io_write_and_close(content, new FileOutputStream(backupPath));
                }
                catch (IOException e)
                {
                    result.onResult(false);
                    return;
                }

                File originalFile = new File(path);
                originalFile.delete();

                File backupFile = new File(backupPath);
                backupFile.renameTo(originalFile);
                result.onResult(true);
            }
        });
    }

    @Override
    public void fileWriteallSafely(final String path, final byte[] content, final BoolResult result)
    {
        fileWriteallSafely(path, content, new IBoolResult() {
            @Override
            public void onResult(boolean success) {
                result.onResult(success);
            }
        });
    }

    private String getPwdPath(String email)
    {
        File file = new File(contentPath, email.replace('@', '.') + ".pwd");
        String path = file.getAbsolutePath();
        return path;
    }

    @Override
    public void fileWritePassword(final String username, final String password, final BoolResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                try
                {
                    byte[] ciphertext = Util.encrypt(ctx, password.getBytes());
                    String path = getPwdPath(username);
                    boolean success = fileWriteallDirect(path, ciphertext);
                    result.onResult(success);
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                    result.onResult(false);
                }
            }
        });
    }

    @Override
    public void fileReadPassword(final String username, final StringResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                String path = getPwdPath(username);
                byte[] ciphertext = fileReadbinaryDirect(path);
                if (null == ciphertext || 0 == ciphertext.length)
                {
                    result.onResult("");
                    return;
                }
                try
                {
                    byte[] pwd = Util.decrypt(ctx, ciphertext);
                    result.onResult(pwd != null ? new String(pwd) : "");
                }
                catch (Exception e)
                {
                    result.onResult("");
                }
            }
        });
    }

    @Override
    public void fileSize(final String path, final LongResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                File file = new File(path);
                result.onResult(file.exists() ? file.length() : 0);
            }
        });
    }

    public byte[] thumbnail(String path, final FileType type)
    {
        switch (type)
        {
            case VIDEO:
            {
                Bitmap bmp = ThumbnailUtils.createVideoThumbnail(path, MediaStore.Images.Thumbnails.MINI_KIND);
                if (null == bmp)
                    return new byte[0];

                final ByteArrayOutputStream stream = new ByteArrayOutputStream();
                bmp.compress(Bitmap.CompressFormat.PNG, 100, stream);
                byte[] r = stream.toByteArray();

                bmp.recycle();
                try
                {
                    stream.close();
                }
                catch (IOException e) {}
                return r;
            }
            case IMAGE:
            {

                try
                {
                    final FileInputStream fis = new FileInputStream(path);
                    try
                    {
                        Bitmap scaledBmp;

                        {
                            final int THUMBNAIL_SIZE = 64;
                            Bitmap bmp = BitmapFactory.decodeStream(fis);
                            scaledBmp = Bitmap.createScaledBitmap(bmp, THUMBNAIL_SIZE, THUMBNAIL_SIZE, false);
                        }

                        final ByteArrayOutputStream baos = new ByteArrayOutputStream();
                        try
                        {
                            scaledBmp.compress(Bitmap.CompressFormat.JPEG, 100, baos);
                            return baos.toByteArray();
                        }
                        finally
                        {
                            baos.close();
                        }
                    }
                    finally
                    {
                        fis.close();
                    }
                }
                catch (Exception e)
                {
                }

                break;
            }
        }

        return new byte[0];
    }

    public boolean fileThumbnail(final String path, final FileType type, final IBinaryResult result)
    {
        if (FileType.VIDEO == type || FileType.IMAGE == type)
        {
            fileTasks.execute(new Runnable()
            {
                @Override
                public void run()
                {
                    result.onResult(thumbnail(path, type));
                }
            });

            return true;
        }
        return false;
    }

    @Override
    public boolean fileThumbnail(final String path, final FileType type, final BinaryResult result)
    {
        return fileThumbnail(path, type, new IBinaryResult() {
            @Override
            public void onResult(byte[] data) {
                result.onResult(data);
            }
        });
    }

    public void copyFile(final String currentPath, final String newPath, final IStringResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                try
                {
                    Util.io_copy_and_close(new FileInputStream(currentPath), new FileOutputStream(newPath));
                    result.onResult("");
                }
                catch (FileNotFoundException e)
                {
                    File newFile = new File(newPath.substring(0, newPath.lastIndexOf('/')));

                    if (!newFile.exists() && newFile.mkdirs())
                    {
                        try
                        {
                            Util.io_copy_and_close(new FileInputStream(currentPath), new FileOutputStream(newPath));
                            result.onResult("");
                        }
                        catch (Exception e1)
                        {
                            result.onResult(e1.getMessage());
                        }
                        return;
                    }
                    result.onResult(e.getMessage());
                }
                catch (Exception e)
                {
                    result.onResult(e.getMessage());
                }
            }
        });
    }

    @Override
    public void copyFile(final String currentPath, final String newPath, final BoolResult result)
    {
        copyFile(currentPath, newPath, new IStringResult() {
            @Override
            public void onResult(String error) {
                result.onResult(error.isEmpty());
            }
        });
    }

    public void makePath(final String path, final IBoolResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                File f = new File(path);
                if (f.mkdirs())
                    result.onResult(true);
                else
                    result.onResult(false);
            }
        });
    }

    @Override
    public void makePath(final String path, final BoolResult result)
    {
        makePath(path, new IBoolResult() {
            @Override
            public void onResult(boolean success) {
                result.onResult(success);
            }
        });
    }

    public void renameFile(final String currentPath, final String newPath, final IBoolResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                File currentFile = new File(currentPath);
                File newFile = new File(newPath);
                result.onResult(currentFile.renameTo(newFile));
            }
        });
    }

    @Override
    public void renameFile(final String currentPath, final String newPath, final BoolResult result)
    {
        renameFile(currentPath, newPath, new IBoolResult() {
            @Override
            public void onResult(boolean success) {
                result.onResult(success);
            }
        });
    }

    @Override
    public CancellationToken httpRequest(HttpVerb verb, String url, ArrayList<Header> headers, String body, HttpResult result)
    {
        HttpRequestTask task = new HttpRequestTask(result, verb, url, headers, body);
        shortTasks.execute(task);
        return task;
    }

    @Override
    public CancellationToken httpUploadFile(HttpVerb verb, String url, String filePath, ArrayList<Header> headers, HttpProgressResult result)
    {
        HttpUploadTask task = new HttpUploadTask(verb, url, filePath, headers, result);
        longTasks.execute(task);
        return task;
    }

    @Override
    public CancellationToken httpDownloadFile(String url, String filePath, ArrayList<Header> headers, long size, String md5, HttpProgressResult result)
    {
        HttpDownloadTask task = new HttpDownloadTask(url, filePath, headers, size, md5, result);
        longTasks.execute(task);
        return task;
    }

    @Override
    public void wait(int millis, VoidResult result)
    {
    }

    @Override
    public void fileExpandDirectories(final ArrayList<String> paths, final StringsResult result)
    {
        fileTasks.execute(new Runnable()
        {
            @Override
            public void run()
            {
                result.onResult(paths);
            }
        });
    }

    @Override
    public void fileCopyHierarchy(String destRootPath, ArrayList<String> destRelativePaths, ArrayList<String> srcPaths)
    {
    }
}
