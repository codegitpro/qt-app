package net.ai.tmp.cloudstore.http;

import android.icu.util.Output;
import android.os.AsyncTask;
import android.util.Log;

import net.ai.CancellationToken;
import net.ai.Header;
import net.ai.HttpResult;
import net.ai.HttpVerb;
import net.ai.tmp.cloudstore.HttpResponseResult;
import net.ai.tmp.cloudstore.Util;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

import javax.net.ssl.HttpsURLConnection;

import static net.ai.HttpVerb.GET;
import static net.ai.HttpVerb.OPTIONS;
import static net.ai.HttpVerb.REMOVE;

public class HttpRequestTask extends CancellationToken implements Runnable
{
    public final HttpResult receiver;
    public final HttpVerb verb;
    public final String url;
    public final ArrayList<Header> headers;
    public final String body;
    private final AtomicBoolean cancelled = new AtomicBoolean(false);

    public HttpRequestTask(HttpResult receiver, HttpVerb verb, String url, ArrayList<Header> headers, String body)
    {
        this.receiver = receiver;
        this.verb = verb;
        this.url = url;
        this.headers = headers;
        this.body = body;
    }


    @Override
    public void run()
    {
        HttpResponseResult result = doRequest();
        if (null != result)
            receiver.onResult(result.statusCode, result.headers, result.data);
        else
            receiver.onResult(0, new ArrayList<Header>(), "");
    }

    private HttpResponseResult doRequest()
    {
        if (cancelled.get())
            return null;
        try
        {
            Log.d("http", url);
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

                connection.setDoInput(true);

                if (null != body && 0 < body.length() && GET != verb && REMOVE != verb && OPTIONS != verb)
                {
                    connection.setDoOutput(true);
                    final OutputStream outputStream = connection.getOutputStream();
                    try
                    {
                        if (cancelled.get())
                            return null;

                        final BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(outputStream));
                        try
                        {
                            writer.write(body);
                            writer.flush();
                        }
                        finally
                        {
                            writer.close();
                        }
                    }
                    finally
                    {
                        outputStream.close();
                    }
                }

                if (cancelled.get())
                    return null;

                int sc  = connection.getResponseCode();

                ArrayList<Header> responseHeaders = new ArrayList<Header>();
                for (Map.Entry<String, List<String>> entry : connection.getHeaderFields().entrySet())
                {
                    String key = entry.getKey();
                    List<String> val = entry.getValue();
                    if (null == key || 0 == val.size())
                        continue;
                    responseHeaders.add(new Header(entry.getKey(), val.get(0)));
                }

                String result = Util.readStringAndClose(connection.getInputStream(), cancelled);
                if (null == result)
                    return null;
                return new HttpResponseResult(sc, responseHeaders, result);
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
    public void cancel()
    {
        cancelled.set(true);
    }
}
