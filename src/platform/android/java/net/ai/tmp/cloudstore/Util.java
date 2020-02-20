package net.ai.tmp.cloudstore;

import android.Manifest;
import android.app.Activity;
import android.content.ClipData;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Build;
import android.provider.MediaStore;
import android.security.KeyPairGeneratorSpec;
import android.security.keystore.KeyGenParameterSpec;
import android.security.keystore.KeyProperties;
import android.support.v4.app.ActivityCompat;
import android.util.Base64;
import android.util.Log;

import net.ai.HttpProgressResult;
import net.ai.HttpVerb;
import net.ai.Progress;
import net.ai.tmp.cloudstore.http.HttpDownloadResult;
import net.ai.FileType;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.security.InvalidKeyException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.UnrecoverableEntryException;
import java.security.cert.CertificateException;
import java.security.interfaces.RSAPrivateKey;
import java.security.interfaces.RSAPublicKey;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Random;
import java.util.concurrent.atomic.AtomicBoolean;

import javax.crypto.Cipher;
import javax.crypto.CipherInputStream;
import javax.crypto.CipherOutputStream;
import javax.crypto.NoSuchPaddingException;
import javax.security.auth.x500.X500Principal;

public final class Util
{
    public static int KB = 1024;
    public static int MB = KB * KB;
    public static int GB = KB * MB;
    private static final char[] illegal_filename_chars = "/<>:|?*\"\\".toCharArray();
    public static final String ANDROID_KEYSTORE = "AndroidKeyStore";
    public static final String KEY_ALIAS = "blompkey";


    public static void createKeys(Context ctx) throws Exception
    {
        KeyPairGenerator generator = KeyPairGenerator.getInstance("RSA", ANDROID_KEYSTORE);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
        {
            generator.initialize
            (
                new KeyGenParameterSpec.Builder(KEY_ALIAS, KeyProperties.PURPOSE_DECRYPT)
                    .setDigests(KeyProperties.DIGEST_SHA256, KeyProperties.DIGEST_SHA512)
                    .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_RSA_PKCS1)
                    .build()
            );
        }
        else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2)
        {
            Calendar end = Calendar.getInstance();
            end.add(Calendar.YEAR, 10);

            generator.initialize
            (
                new KeyPairGeneratorSpec.Builder(ctx)
                    .setAlias(KEY_ALIAS)
                    .setSubject(new X500Principal("CN=" + KEY_ALIAS))
                    .setSerialNumber(BigInteger.valueOf(1337))
                    .setStartDate(Calendar.getInstance().getTime())
                    .setEndDate(end.getTime())
                    .build()
            );
        }
        KeyPair kp = generator.generateKeyPair();
        Log.d("hans", "PUBLIC KEY = " + kp.getPublic());
        Log.d("hans", "PRIVATE KEY = " + kp.getPrivate());
    }

    public static KeyStore getKeyStore(Context ctx)
    {
        try
        {
            final KeyStore ks = KeyStore.getInstance(ANDROID_KEYSTORE);
            if (null == ks)
                return null;

            ks.load(null);
            if (!ks.containsAlias(KEY_ALIAS))
                createKeys(ctx);
            return ks;
        }
        catch (Exception e)
        {
        }
        return null;
    }

    public static byte[] encrypt(Context ctx, byte[] plaintext)
    {
        if (null == plaintext)
            return null;
        final KeyStore ks = getKeyStore(ctx);
        if (null == ks)
            return plaintext;
        try
        {
            Cipher cc = Cipher.getInstance("RSA/ECB/PKCS1Padding");
            KeyStore.PrivateKeyEntry privateKeyEntry = (KeyStore.PrivateKeyEntry) ks.getEntry(KEY_ALIAS, null);
            cc.init(Cipher.ENCRYPT_MODE, privateKeyEntry.getCertificate().getPublicKey());
            return cc.doFinal(plaintext);
        }
        catch (Exception e)
        {
            try
            {
                ks.deleteEntry(KEY_ALIAS);
            }
            catch (Exception e1)
            {
            }
        }
        return plaintext;
    }

    public static byte[] decrypt(Context ctx, byte[] ciphertext)
    {
        if (null == ciphertext)
            return null;
        final KeyStore ks = getKeyStore(ctx);
        if (null == ks)
            return ciphertext;
        try
        {
            Cipher cc1 = Cipher.getInstance("RSA/ECB/PKCS1Padding");
            KeyStore.PrivateKeyEntry privateKeyEntry = (KeyStore.PrivateKeyEntry)ks.getEntry(KEY_ALIAS, null);
            cc1.init(Cipher.DECRYPT_MODE, privateKeyEntry.getPrivateKey());
            byte[] decoded = cc1.doFinal(ciphertext);
            return decoded;
        }
        catch (Exception e)
        {
            try
            {
                ks.deleteEntry(KEY_ALIAS);
            }
            catch (Exception e1)
            {
            }
        }
        return ciphertext;
    }

    public static String compactSize(long size)
    {
        if (size >= 1000000000000L)
            return String.format("%s TB", Math.round(((double)size)/100000000000.0)/10.0);
        if (size >= 10000000000L)
            return String.format("%d GB", (long)Math.round(((double)size)/1000000000.0));
        if (size >= 1000000000L)
            return String.format("%s GB", Math.round(((double)size)/100000000.0)/10.0);
        if (size >= 10000000)
            return String.format("%d MB", (long)Math.round(((double)size)/1000000.0));
        if (size >= 1000000)
            return String.format("%s MB", Math.round(((double)size)/100000.0)/10.0);
        if (size >= 10000)
            return String.format("%d KB", (long)Math.round(((double)size)/1000.0));
        if (size >= 1000)
            return String.format("%s KB", Math.round(((double)size)/100.0)/10.0);
        if (size > 0)
            return String.format("%d", size);
        return "";
    }
    public static String convert(HttpVerb verb)
    {
        switch (verb)
        {
            case GET: return "GET";
            case POST: return "POST";
            case PUT: return "PUT";
            case PATCH: return "PATCH";
            case REMOVE: return "DELETE";
            case OPTIONS: return "OPTIONS";
        }
        return "";
    }

    public static long fileSize(String filePath)
    {
        return new File(filePath).length();
    }

    public static void io_write_and_close(byte[] data, final OutputStream os) throws IOException
    {
        try
        {
            os.write(data, 0, data.length);
            os.flush();
        }
        finally
        {
            os.close();
        }
    }

    public static void io_copy_and_close(final InputStream is, final OutputStream os) throws IOException
    {
        try
        {
            try
            {
                byte[] buffer = new byte[64* Util.KB];
                while (true)
                {
                    int readSize = is.read(buffer);
                    if (-1 == readSize)
                        break;
                    os.write(buffer, 0, readSize);
                }

                os.flush();
            }
            finally
            {
                os.close();
            }
        }
        finally
        {
            is.close();
        }
    }

    public static boolean io_copy_and_close(final InputStream is, final OutputStream os, long contentLength, HttpProgressResult progress, AtomicBoolean cancelled) throws Exception
    {
        try
        {
            try
            {
                if (null != cancelled && cancelled.get())
                    return false;

                byte[] buffer = new byte[64* Util.KB];
                long total = 0;
                long progressTarget = buffer.length;
                while (true)
                {
                    if (null != cancelled && cancelled.get())
                        return false;

                    int readSize = is.read(buffer, 0, buffer.length);
                    if (-1 == readSize)
                        break;
                    os.write(buffer, 0, readSize);
                    total += readSize;
                    if (total > progressTarget)
                    {
                        progress.onProgress(new Progress(total, contentLength));
                        progressTarget = total + buffer.length;
                    }
                }

                os.flush();

            }
            finally
            {
                os.close();
            }
        }
        finally
        {
            is.close();
        }
        return true;
    }

    public static Uri[] contentUris(Intent data)
    {
        if (null == data)
            return new Uri[0];
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN)
        {
            ClipData clipData = data.getClipData();
            if (null != clipData)
            {
                Uri[] r = new Uri[clipData.getItemCount()];
                for (int i = 0; i < r.length; ++i)
                    r[i] = clipData.getItemAt(i).getUri();
                return r;
            }
        }
        Uri uri = data.getData();
        if (null == uri)
            return new Uri[0];
        return new Uri[]{uri};
    }

    public static String friendlyName(ContentResolver cr, Uri uri)
    {
        if (null == uri)
            return "";
        String[] projection = {MediaStore.MediaColumns.TITLE, MediaStore.MediaColumns.DISPLAY_NAME};
        final Cursor metaCursor = cr.query(uri, projection, null, null, null);
        if (metaCursor != null)
        {
            try
            {
                if (metaCursor.moveToFirst())
                {
                    for (int i = 0; i < metaCursor.getColumnCount(); ++i)
                    {
                        String title = metaCursor.getString(i);
                        if (null != title && title.length() > 0)
                            return title;
                    }
                }
            }
            finally
            {
                metaCursor.close();
            }
        }
        String ext = extension(cr.getType(uri));
        return sanitiseFilename(uri.getLastPathSegment()) + (null != ext ? ext : "");
    }

    public static String mimeFromFilename(String filename)
    {
        if (null == filename)
            return "";
        String fn = filename.toLowerCase();
        if (fn.endsWith(".png"))
            return "image/png";
        if (fn.endsWith(".jpg") || fn.endsWith(".jpeg"))
            return "image/jpeg";
        if (fn.endsWith(".mp4"))
            return "video/mpeg";
        return "";
    }

    public static Uri addImageToGallery(ContentResolver cr, String filename, String filepath)
    {
        ContentValues values = new ContentValues();
        values.put(MediaStore.Images.Media.TITLE, filename);
        values.put(MediaStore.Images.Media.DISPLAY_NAME, filename);
        values.put(MediaStore.Images.Media.DESCRIPTION, "");
        values.put(MediaStore.Images.Media.MIME_TYPE, mimeFromFilename(filename));
        values.put(MediaStore.Images.Media.DATE_ADDED, System.currentTimeMillis());
        values.put(MediaStore.Images.Media.DATE_TAKEN, System.currentTimeMillis());
        values.put(MediaStore.Images.Media.DATA, filepath);

        return cr.insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
    }

    public static String fileExtension(String path)
    {
        if (null == path)
            return "";
        return path.substring(path.lastIndexOf('.'));
    }

    public static String extension(String mime)
    {
        if (null == mime)
            return "";
        if ("image/jpeg".equals(mime))
            return ".jpg";
        if ("image/png".equals(mime))
            return ".png";

        return "." + mime.substring(mime.lastIndexOf('/')+1);
    }

    public static FileType fileTypeFromMime(String mime)
    {
        if (null == mime)
            return FileType.OTHER;

        if (mime.startsWith("image/"))
            return FileType.IMAGE;
        if (mime.startsWith("video/"))
            return FileType.VIDEO;
        if (mime.endsWith("/text"))
            return FileType.TEXT;
        return FileType.OTHER;
    }
    public static String sanitiseFilename(String filename)
    {
        StringBuilder sb = new StringBuilder();

        for (char c : filename.toCharArray())
        {
            boolean found = false;
            for (char il : illegal_filename_chars)
            {
                if (c == il)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                sb.append(c);
        }

        return sb.toString();
    }

    public static boolean isNetworkReady(Activity activity)
    {
        ConnectivityManager connectivityManager = (ConnectivityManager) activity.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
        if (networkInfo == null
                || !networkInfo.isConnected()
                || (networkInfo.getType() != ConnectivityManager.TYPE_WIFI && networkInfo.getType() != ConnectivityManager.TYPE_MOBILE))
            return false;
        return true;
    }

    public static String readStringAndClose(InputStream stream, AtomicBoolean cancelled) throws Exception
    {
        try
        {
            if (null != cancelled && cancelled.get())
                return null;

            final InputStreamReader reader = new InputStreamReader(stream, "UTF-8");
            try
            {
                char[] buffer = new char[1024];
                StringBuffer sbuf = new StringBuffer();
                while (true)
                {
                    if (null != cancelled && cancelled.get())
                        return null;
                    int readSize = reader.read(buffer, 0, buffer.length);
                    if (-1 == readSize)
                        break;
                    sbuf.append(buffer, 0, readSize);
                }

                return sbuf.toString();
            }
            finally
            {
                reader.close();
            }
        }
        finally
        {
            stream.close();
        }
    }

    public static String readStringFromFile(String filePath) throws Exception
    {
        return readStringAndClose(new FileInputStream(filePath), null);
    }

    public static byte[] readBytesAndClose(InputStream stream) throws Exception
    {
        try
        {
            byte[] buffer = new byte[1024];
            final ByteArrayOutputStream baos = new ByteArrayOutputStream();
            try
            {
                while (true)
                {
                    int readSize = stream.read(buffer, 0, buffer.length);
                    if (-1 == readSize)
                        break;
                    baos.write(buffer, 0, readSize);
                }

                baos.flush();
                return baos.toByteArray();
            }
            finally
            {
                baos.close();
            }
        }
        finally
        {
            stream.close();
        }
    }

    public static byte[] readBytesFromFile(String path) throws Exception
    {
        return readBytesAndClose(new FileInputStream(path));
    }
    // Storage Permissions
    private static String[] PERMISSIONS_STORAGE = {Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};

    public static boolean verifyStoragePermissions(Activity activity, int requestCode)
    {
        int permission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);

        if (permission != PackageManager.PERMISSION_GRANTED)
        {
            ActivityCompat.requestPermissions(activity, PERMISSIONS_STORAGE, requestCode);
            return false;
        }
        return true;
    }
}
