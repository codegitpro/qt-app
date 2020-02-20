function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") 
    {
        component.addOperation("Execute", "{0,3010}", "@TargetDir@/vc_redist.x64.exe", "/quiet", "/norestart");
    }
    else if (systemInfo.productType === "osx")
    {

    }
}

