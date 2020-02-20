package net.ai.tmp.cloudstore.http;

import net.ai.Header;

import java.util.ArrayList;

public class HttpDownloadResult
{
    public final int statusCode;
    public final ArrayList<Header> headers;
    public final long contentLength;

    public HttpDownloadResult(int statusCode, ArrayList<Header> headers, long contentLength)
    {
        this.statusCode = statusCode;
        this.headers = headers;
        this.contentLength = contentLength;
    }
}
