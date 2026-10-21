#ifdef WORKBENCH

typedef int[] MetaFile;
sealed class MetaFile: BaseContainer
{
	proto external ResourceName GetResourceID();
	//! Returns exact path to source file
	proto external string GetSourceFilePath();
	proto external void Save();
}

#endif
