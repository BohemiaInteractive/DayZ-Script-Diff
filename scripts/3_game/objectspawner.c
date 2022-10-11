class ObjectSpawnerHandler
{
	//---------------------------------------------------------------------------------------
	static void SpawnObjects()
	{
		if( CfgGameplayHandler.GetObjectSpawnersArr() && CfgGameplayHandler.GetObjectSpawnersArr().Count() > 0 )
		{
			TStringArray arr = CfgGameplayHandler.GetObjectSpawnersArr();
			foreach(string spawner_json: arr)
			{
				string path = "$mission:"+spawner_json;
				
				if ( FileExist( path ) )
				{
					ObjectSpawnerJson spawner;
					JsonFileLoader<ObjectSpawnerJson>.JsonLoadFile( path, spawner );

					foreach (ITEM_SpawnerObject o: spawner.Objects)
					{
						SpawnObject(o);
					}
				}
			}
		}
	}
	//---------------------------------------------------------------------------------------
	static void SpawnObject(ITEM_SpawnerObject item)
	{
		Object object = GetGame().CreateObjectEx(item.name, vector.ArrayToVec(item.pos), ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS | ECE_NOLIFETIME);
		if ( object )
		{
			object.SetOrientation( vector.ArrayToVec(item.ypr));
			if (item.scale != 0)
				object.SetScale(item.scale);
		}
	}
	//---------------------------------------------------------------------------------------
	static void OnGameplayDataHandlerLoad()
	{
		SpawnObjects();
	}
	//---------------------------------------------------------------------------------------
};

class ObjectSpawnerJson
{
	ref array<ref ITEM_SpawnerObject> Objects;
};

class ITEM_SpawnerObject
{
	string name;
	float pos[3];
	float ypr[3];
	float scale;
};


//! Utility class that converts init.c format type of spawn commands to a json file, fill in the SpawnInit() with your data and call SpawnDataConverter.SpawnObjects() through script editor console, result is a json file
class SpawnDataConverter
{
	static ref array<ref ITEM_SpawnerObject> Objects = new array<ref ITEM_SpawnerObject>;
	static string m_Path = "$mission:MySpawnData.json";
	
	static void SpawnObjects()
	{
		Objects.Clear();
		SpawnInit();
		ObjectSpawnerJson j = new ObjectSpawnerJson;
		j.Objects = Objects;
		JsonFileLoader<ObjectSpawnerJson>.JsonSaveFile(m_Path, j );
	}
	
	static void SpawnInit()
	{
		AddSpawnData( "Land_Wall_Gate_FenR", "8406.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000" );
		AddSpawnData( "Land_Wall_Gate_FenR", "8410.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000" );
		AddSpawnData( "Land_Wall_Gate_FenR", "8416.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000" );
		AddSpawnData( "Land_Wall_Gate_FenR", "8422.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000" );

	}
	static void AddSpawnData(string objectName, vector position, vector orientation)
	{
		ITEM_SpawnerObject obj = new ITEM_SpawnerObject;
		obj.name = objectName;
		obj.pos[0] = position[0];
		obj.pos[1] = position[1];
		obj.pos[2] = position[2];
		
		obj.ypr[0] = orientation[0];
		obj.ypr[1] = orientation[1];
		obj.ypr[2] = orientation[2];
		
		Objects.Insert(obj);
		
	}

};