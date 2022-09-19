// JSON data structure
class JsonDataContaminatedAreas : Managed
{
	ref array<ref JsonDataContaminatedArea> Areas;
	ref array<ref array<float>> SafePositions;
}

class JsonDataContaminatedArea : Managed
{
	string AreaName;
	string Type;
	string TriggerType;
	JsonDataAreaData Data;
	JsonDataPlayerData PlayerData;
};

class JsonDataAreaData : Managed
{
	ref array<float> Pos;
	float Radius;
	float PosHeight;
	float NegHeight;
	int InnerRingCount;
	int InnerPartDist;
	bool OuterRingToggle;
	int OuterPartDist;
	int OuterOffset;
	int VerticalLayers;
	int VerticalOffset;
	string ParticleName; 
};

class JsonDataPlayerData : Managed
{
	string AroundPartName;
	string TinyPartName;
	string PPERequesterType;
}