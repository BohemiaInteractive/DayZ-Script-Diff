[WorkbenchPluginAttribute("SVN Diff", "Just for testing", "ctrl+`", "", {"ScriptEditor"})]
class SVNDiffPlugin: WorkbenchPlugin
{
	[Attribute("TortoiseProc /command:diff /path:$path", "editbox")]
	string CommandLine;
	
	override void Run()
	{
		ScriptEditor mod = Workbench.GetModule("ScriptEditor");
		if (mod)
		{
			string file;
			string absPath;
			if (mod.GetCurrentFile(file) && Workbench.GetAbsolutePath(file, absPath))
			{
				string command = CommandLine;
				command.Replace("$path", absPath);
				Workbench.RunCmd(command);
				Print( command );
				Print( absPath );
			}
		}
	}
	
	override void Configure()
	{
		Workbench.ScriptDialog("Configure SVN Diff", "Usage: \n$path - will be replaced with file name", this);
	}
	
	[ButtonAttribute("OK")]
	void OkButton() {}
}

