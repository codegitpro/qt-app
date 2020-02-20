function Component()
{
    installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") 
    {
        component.addOperation("CreateShortcut", "@TargetDir@/qtdtapp.exe", "@StartMenuDir@/Blomp.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/app.ico",
            "description=Open Blomp");
    }
    else if (systemInfo.productType === "osx")
    {

    }
}

Component.prototype.installationFinished = function()
{
    try 
    {
        if (installer.isInstaller() && installer.status == QInstaller.Success) 
        {
            console.log("success");
            var widget = gui.pageById(QInstaller.InstallationFinished);
            if (systemInfo.productType === "osx" && widget.RunItCheckBox.checked)
            {
                var argList = ["-a", "@TargetDir@/qtdtapp.app"];
                installer.execute("open", argList);
            }
			
			if (systemInfo.kernelType === "linux") 
			{
				console.log("start icons Linux");

				installer.performOperation("CreateDesktopEntry",
					"@HomeDir@/.local/share/applications/@Name@.desktop",
					"Version=@Version@\nType=Application\nTerminal=false\nExec=@TargetDir@/@Name@.sh\nName=@Name@\nIcon=@TargetDir@/app.png\nName[en_US]=Blomp");

				installer.performOperation("Copy",
					"@HomeDir@/.local/share/applications/@Name@.desktop",
					"@HomeDir@/Desktop/@Name@.desktop");
			}
        }

    } catch(e) {
        console.log(e);
    }
}

