package net.ai.tmp.cloudstore.http;

import android.os.AsyncTask;
import android.util.Log;

import net.ai.CancellationToken;
import net.ai.Header;
import net.ai.HttpProgressResult;
import net.ai.HttpVerb;
import net.ai.Progress;
import net.ai.tmp.cloudstore.HttpResponseResult;
import net.ai.tmp.cloudstore.Util;

import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.Buffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

import static net.ai.HttpVerb.GET;
import static net.ai.HttpVerb.OPTIONS;
import static net.ai.HttpVerb.REMOVE;

public class HttpDownloadTask extends CancellationToken implements Runnable
{
    private final String url;
    private final String filePath;
    private final ArrayList<Header> headers;
    private final long size;
    private final String md5;
    private final HttpProgressResult result;
    private final AtomicBoolean cancelled = new AtomicBoolean(false);

    public HttpDownloadTask(String url, String filePath, ArrayList<Header> headers, long size, String md5, HttpProgressResult result)
    {
        this.url = url;
        this.filePath = filePath;
        this.headers = headers;
        this.size = size;
        this.md5 = md5;
        this.result = result;
    }

    private HttpDownloadResult doInBackground()
    {
        if (cancelled.get())
            return null;

        try
        {
            Log.d("download", url);
            URL u = new URL(url);
            final HttpURLConnection connection = (HttpURLConnection) u.openConnection();
            try
            {
                if (cancelled.get())
                    return null;

                connection.setRequestMethod("GET");

                for (Header header : headers)
                {
                    connection.setRequestProperty(header.getName(), header.getValue());
                }

                connection.setDoInput(true);

                int sc = connection.getResponseCode();
                if (cancelled.get())
                    return null;

                ArrayList<Header> responseHeaders = new ArrayList<Header>();
                long contentLength = 0;
                for (Map.Entry<String, List<String>> entry : connection.getHeaderFields().entrySet())
                {
                    String key = entry.getKey();
                    List<String> val = entry.getValue();
                    if (null == key || 0 == val.size())
                        continue;
                    if ("content-length".equals(key.toLowerCase()))
                        contentLength = Long.valueOf(val.get(0));
                    responseHeaders.add(new Header(entry.getKey(), val.get(0)));
                }

                if (!Util.io_copy_and_close(connection.getInputStream(), new FileOutputStream(filePath), contentLength, result, cancelled))
                    return null;
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
