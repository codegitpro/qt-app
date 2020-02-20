package net.ai.tmp.cloudstore.http;

import net.ai.Header;
import net.ai.HttpResult;
import net.ai.HttpVerb;

import java.util.ArrayList;

public class HttpRequestParams
{
    public final HttpResult receiver;
    public final HttpVerb verb;
    public final String url;
    public final ArrayList<Header> headers;
    public final String body;

    public HttpRequestParams(HttpResult receiver, HttpVerb verb, String url, ArrayList<Header> headers, String body)
    {
        this.receiver = receiver;
        this.verb = verb;
        this.url = url;
        this.headers = headers;
        this.body = body;
    }
}
