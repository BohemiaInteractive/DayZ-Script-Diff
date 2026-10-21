#ifdef WORKBENCH

typedef int[] ProcessHandle;

sealed class Workbench
{
	private void Workbench();
	private void ~Workbench();
    
	static proto WBModuleDef GetModule(typename type);
	static proto bool OpenModule(typename type);
	static proto bool CloseModule(typename type);
	static proto void Dialog(string caption, string text, string detailedText = string.Empty);
	static proto int ScriptDialog(string caption, string text, Class data);
	static proto bool SearchResources(string filter, func callback);
	static proto int RunCmd(string command, bool wait = false);
    /*!
	Open ScriptEditor and try to find given symbol.

	\code
		Workbench.JumpToScriptSymbol("IEntity.GetID");
	\endcode
	*/
	static proto bool JumpToScriptSymbol(string symbol);
	/*!
	Attempts to start up a process with given CLI command. Returns process handle.
	*/
	static proto ProcessHandle RunProcess(string command);
	/*!
	Attempts to kill the process of given handle. Closes the handle. Returns true
	when the process really transitioned from running to closed state.
	*/
	static proto bool KillProcess(ProcessHandle handle);
	/*!
	Queries the active flag on the process handle.
	*/
	static proto bool IsRunningProcess(ProcessHandle handle);
	/*!
	Waits for the process to complete. Takes timeout value in milliseconds.
	*/
	static proto int WaitProcess(ProcessHandle handle, int timeout = -1);
	/*!
	Closes the process handle.
	*/
	static proto void FinishProcess(ProcessHandle handle);
	static proto void GetCwd(out string currentDir);
	static proto void GetUserName(out string userName);
	static proto bool GetAbsolutePath(string relativePath, out string absPath, bool mustExist = true);
	static proto void GetExecutableFilePath(out string filePath);
	//! Returns absolute path to game project settings.
	static proto string GetCurrentGameProjectFile();
	//! Returns game project settings.
	static proto BaseContainer GetGameProjectSettings();
	static proto string GenerateGloballyUniqueID64();
	static proto void Exit(int exitCode);
	static proto DateTimeUtcAsInt GetPackedUtcTime();
	static proto ResourceName GetResourceName(string path);
	static proto bool OpenResource(string filename);
	static proto ref ScriptModule GetWBScriptModule();
	static proto ref ScriptModule GetWBGameScriptModule();
};

#endif
