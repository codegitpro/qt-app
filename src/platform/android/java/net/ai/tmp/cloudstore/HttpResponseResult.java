package net.ai.tmp.cloudstore;

import net.ai.Header;

import java.util.ArrayList;

public class HttpResponseResult
{
    public final int statusCode;
    public final ArrayList<Header> headers;
    public final String data;

    public HttpResponseResult(int statusCode, ArrayList<Header> headers, String data)
    {
        this.statusCode = statusCode;
        this.headers = headers;
        this.data = data;
    }
}

