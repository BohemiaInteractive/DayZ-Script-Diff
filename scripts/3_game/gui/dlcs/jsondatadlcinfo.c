class JsonDataDLCList : Managed
{
	ref array<ref JsonDataDLCInfo> DLCs;
};

class JsonDataDLCInfo : Managed
{
	string Name;
	string HeaderText;
	string DescriptionText;
	string VideoFileName;
};
