package net.ai.tmp.cloudstore.http;

import android.os.Build;
import android.util.Log;

import net.ai.CancellationToken;
import net.ai.Header;
import net.ai.HttpProgressResult;
import net.ai.HttpVerb;
import net.ai.tmp.cloudstore.Util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

public class HttpUploadTask extends CancellationToken implements Runnable
{
    private final HttpVerb verb;
    private final String url;
    private final String filePath;
    private final ArrayList<Header> headers;
    private final HttpProgressResult result;
    private final AtomicBoolean cancelled = new AtomicBoolean(false);

    public HttpUploadTask(HttpVerb verb, String url, String filePath, ArrayList<Header> headers, HttpProgressResult result)
    {
        this.verb = verb;
        this.url = url;
        this.filePath = filePath;
        this.headers = headers;
        this.result = result;
    }

    private HttpDownloadResult doInBackground()
    {
        if (cancelled.get())
            return null;

        try
        {
            Log.d("upload", url);
            URL u = new URL(url);
            final HttpURLConnection connection = (HttpURLConnection) u.openConnection();

            try
            {
                if (cancelled.get())
                    return null;

                connection.setRequestMethod(Util.convert(verb));

                for (Header header : headers)
                {
                    connection.setRequestProperty(header.getName(), header.getValue());
                }


                long contentLength = Util.fileSize(filePath);
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
                {
                    connection.setFixedLengthStreamingMode(contentLength);
                }
                else
                {
                    connection.setFixedLengthStreamingMode((int)contentLength);
                }

                {
                    connection.setDoOutput(true);
                    boolean r = Util.io_copy_and_close(new FileInputStream(filePath), connection.getOutputStream(), contentLength, result, cancelled);
                    if (!r)
                        return null;
                }

                if (cancelled.get())
                    return null;

                int sc = connection.getResponseCode();

                if (cancelled.get())
                    return null;

                ArrayList<Header> responseHeaders = new ArrayList<Header>();
                for (Map.Entry<String, List<String>> entry : connection.getHeaderFields().entrySet())
                {
                    String key = entry.getKey();
                    List<String> val = entry.getValue();
                    if (null == key || 0 == val.size())
                        continue;
                    responseHeaders.add(new Header(entry.getKey(), val.get(0)));
                }

                return new HttpDownloadResult(sc, responseHeaders, contentLength);
            }
            finally
            {
                connection.disconnect();
            }

        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

        return null;
    }

    @Override
    public void run()
    {
        HttpDownloadResult res = doInBackground();
        if (null != res)
            result.onResult(res.statusCode, res.headers, res.contentLength);
        else
            result.onResult(0, new ArrayList<Header>(), 0);
    }

    @Override
    public void cancel()
    {
        cancelled.set(true);
    }
}
