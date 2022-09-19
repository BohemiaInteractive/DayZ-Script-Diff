[WorkbenchPluginAttribute("Open Dir", "Just for testing", "ctrl+-", "", {"ScriptEditor"})]
class OpenDirPlugin: WorkbenchPlugin
{
	override void Run()
	{
		ScriptEditor mod = Workbench.GetModule("ScriptEditor");
		if (mod)
		{
			string file;
			string absPath;
			if (mod.GetCurrentFile(file) && Workbench.GetAbsolutePath(file, absPath))
			{
				if (absPath.Length() < 2) return;
				
				absPath.Replace("\\", "/");
				
				if (absPath[1] != ":")
				{
					string cwd;
					Workbench.GetCwd(cwd);
					absPath = cwd + "/" + absPath;
				}
				
				int index = absPath.IndexOf("/");
				int last_index = index;
				
				while(index != -1)
				{
					last_index = index;
					index = absPath.IndexOfFrom(last_index + 1, "/");
				}
				
				if (last_index == -1) return;
				
				string path = absPath.Substring(0, last_index);
				string command;
				command.Replace("$path", path);
				//Print(path);
				//Workbench.RunCmd(command);
				Workbench.RunCmd("cmd /c \"start " + path +"\"");
			}
		}
	}
	
	override void Configure()
	{
		Workbench.ScriptDialog("Configure OpenDir", "Usage: \n$path - will be replaced with file name", this);
	}
	
	[ButtonAttribute("OK")]
	void OkButton() {}
}

