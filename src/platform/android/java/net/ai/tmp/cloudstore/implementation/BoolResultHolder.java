package net.ai.tmp.cloudstore.implementation;

import net.ai.BoolResult;

public class BoolResultHolder extends BoolResult
{
    private final IBoolResult result;

    public BoolResultHolder(IBoolResult result)
    {
        this.result = result;
    }
    @Override
    public void onResult(boolean success)
    {
        result.onResult(success);
    }
}
