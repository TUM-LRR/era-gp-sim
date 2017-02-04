Component = function() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/era-sim.exe", "@StartMenuDir@/era-sim.lnk",
            "workingDirectory=@TargetDir@", "description=Launch ERASIM");
		component.addOperation("CreateShortcut", "@TargetDir@/erasim-maintenance.exe", "@StartMenuDir@/era-sim-uninstall.lnk",
            "workingDirectory=@TargetDir@", "description=Uninstall ERASIM");
    }
    if (installer.value("os") === "x11")
    {
	component.addOperation("CreateDesktopEntry", "/usr/share/applications/erasim.desktop", "Version=1.0\nType=Application\nTerminal=true\nExec=@TargetDir@/era-sim.sh\nName=ERASIM\nIcon=@TargetDir@/icon.png\nName[en_US]=ERASIM");
	component.addElevatedOperation("Copy", "/usr/share/applications/erasim.desktop", "@HomeDir@/Desktop/erasim.desktop");
    }
}
