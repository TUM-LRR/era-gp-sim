function Component()
{
}

Component.prototype.createOperationsForArchive = function(archive)
{
  try {
    //First we extract the archive content into the home directory
    component.addOperation("Extract", archive, "@HomeDir@");

    //As we installed with root/admin/sudo/... rights, the owner of .erasim in Home
    // is root/Admin/...
    //we want to change it to the executing user
    if (systemInfo.kernelType === "winnt") {
        //WINDOWS
    } else if (systemInfo.kernelType === "darwin") {
        //MACOS/OSX
    } else {
	//OTHER, hopefully a unix
	
	//component.addOperation("Execute", "chown", "$USER", "-R", "@HomeDir@/.erasim");
    }
  }catch(e) {
    print(e);
  }
}
