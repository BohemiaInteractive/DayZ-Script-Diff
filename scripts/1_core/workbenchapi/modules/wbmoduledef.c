#ifdef WORKBENCH

typedef int[] WBModuleDef;
sealed class WBModuleDef
{
	proto external bool SetOpenedResource(string filename);
	proto external BaseContainer GetContainer(int index = 0);
	proto external int GetNumContainers();
	proto external bool Save();
	proto external bool GetCmdLine(string name, out string value);
	proto external bool Close();
	proto external bool ExecuteAction(notnull array<string> menuPath, bool bKeepFocus = true);
	proto external WorkbenchPlugin GetPlugin(typename pluginType);
}

#endif