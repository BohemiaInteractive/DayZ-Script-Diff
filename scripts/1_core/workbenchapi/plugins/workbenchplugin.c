
class WorkbenchPlugin : Managed
{
	private void WorkbenchPlugin();
	private void ~WorkbenchPlugin();

	event void Run();
	event void RunCommandline();
	event void Configure();
	event void OnResourceContextMenu(notnull array<ResourceName> resources);
};