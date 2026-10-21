/*
===========================================
Do not modify, this script is generated
===========================================
*/

#ifdef WORKBENCH

/*!
\addtogroup WorkbenchAPI_Plugins
\{
*/

class ResourceManagerPlugin: WorkbenchPlugin
{
	event void OnRegisterResource(string absFileName, BaseContainer metaFile);
	event BuildResourceContext OnBuildResource(string absSrcFileName, string absDestFileName, BaseContainer metaFile, BaseContainer configuration, GeneratedResources generatedResources);
	event void OnRenameResource(string absFileNameOld, string absFileNameNew, BaseContainer metaFile);
	//! Returns a directory where new default materials may be generated for given mesh object model (`absModelPath`).
	event string OnGetMaterialGenerateDir(string absModelPath);
	//! Returns suggested MaterialClass name for given material path.
	event string OnGetMaterialClassName(string absMaterialPath, GeneratedResources generatedResources);
	event void OnMaterialCreated(string absMaterialPath, BaseContainer materialSrc, GeneratedResources generatedResources);
}

/*!
\}
*/

#endif // WORKBENCH
