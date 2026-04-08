class ObjectSpawnerHandler
{
	protected static const ref TStringArray VALID_PATHS = {
		"DZ\\plants","DZ\\plants_bliss", "DZ\\plants_sakhal",
		"DZ\\rocks", "DZ\\rocks_bliss", "DZ\\rocks_sakhal",
		"DZ/plants","DZ/plants_bliss", "DZ/plants_sakhal",
		"DZ/rocks", "DZ/rocks_bliss", "DZ/rocks_sakhal",
		};

	//---------------------------------------------------------------------------------------
	static void SpawnObjects()
	{
		if (CfgGameplayHandler.GetObjectSpawnersArr() && CfgGameplayHandler.GetObjectSpawnersArr().Count() > 0)
		{
			TStringArray arr = CfgGameplayHandler.GetObjectSpawnersArr();
			foreach (string spawnerFilePath: arr)
			{
				string path = "$mission:" + spawnerFilePath;

				string errorMessage;
				ObjectSpawnerJson spawner;
				if (JsonFileLoader<ObjectSpawnerJson>.LoadFile(path, spawner, errorMessage))
				{
					foreach (ITEM_SpawnerObject o : spawner.Objects)
						SpawnObject(o);
				}
				else
					ErrorEx(errorMessage);
			}
		}
	}
	//---------------------------------------------------------------------------------------
	static void SpawnObject(ITEM_SpawnerObject item)
	{
		Object object;
		
		float scale = item.scale;
		if (scale == 0)
			scale = 1;
		
		if (item.name.Contains("\\") || item.name.Contains("/"))
		{
			if (ValidatePath(item.name))
				object = g_Game.CreateStaticObjectUsingP3D(item.name, vector.ArrayToVec(item.pos), vector.ArrayToVec(item.ypr),scale);
		}
		else
		{
			int flags = ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS | ECE_NOLIFETIME | ECE_DYNAMIC_PERSISTENCY;
			
			if (item.enableCEPersistency)
			{
				flags &= ~ECE_DYNAMIC_PERSISTENCY;
				flags &= ~ECE_NOLIFETIME;
			}
			
			object = g_Game.CreateObjectEx(item.name, vector.ArrayToVec(item.pos), flags, RF_IGNORE);
			if (object)
			{
				object.SetOrientation( vector.ArrayToVec(item.ypr));
				if (item.scale != 1)
					object.SetScale(scale);
				
				object.OnSpawnByObjectSpawner(item);
			}
		}
		
		if (!object)
			PrintToRPT("Object spawner failed to spawn "+item.name);
	}
	//---------------------------------------------------------------------------------------
	static void OnGameplayDataHandlerLoad()
	{
		if (g_Game && g_Game.IsServer())
		{
			SpawnObjects();
			g_Game.GetWorld().ProcessMarkedObjectsForPathgraphUpdate();
		}
	}
	
	//---------------------------------------------------------------------------------------
	static bool ValidatePath(string path)
	{
		foreach (string p: VALID_PATHS)
		{
			if (path.Contains(p))
				return true;
		}
		
		PrintToRPT("Object spawner: invalid path "+ path);
		return false;
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
	bool enableCEPersistency;
	string customString;
};


//! Utility class that converts init.c format type of spawn commands to a json file, fill in the SpawnInit() with your data and call SpawnDataConverter.SpawnObjects() through script editor console, result is a json file
class SpawnDataConverter
{
	static ref array<ref ITEM_SpawnerObject> Objects = new array<ref ITEM_SpawnerObject>;
	static string m_Path = "$mission:myspawndata.json";
	
	static void SpawnObjects()
	{
		Objects.Clear();
		SpawnInit();
		ObjectSpawnerJson j = new ObjectSpawnerJson();
		j.Objects = Objects;
		
		string errorMessage;
		if (!JsonFileLoader<ObjectSpawnerJson>.SaveFile(m_Path, j, errorMessage))
			ErrorEx(errorMessage);
	}
	
	static void SpawnInit()
	{
		AddSpawnData("Land_Wall_Gate_FenR", "8406.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000");
		AddSpawnData("Land_Wall_Gate_FenR", "8410.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000");
		AddSpawnData("Land_Wall_Gate_FenR", "8416.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000");
		AddSpawnData("Land_Wall_Gate_FenR", "8422.501953 107.736824 12782.338867", "0.000000 0.000000 0.000000");
	}
	
	static void AddSpawnData(string objectName, vector position, vector orientation)
	{
		ITEM_SpawnerObject obj = new ITEM_SpawnerObject();
		obj.name = objectName;
		obj.pos[0] = position[0];
		obj.pos[1] = position[1];
		obj.pos[2] = position[2];
		
		obj.ypr[0] = orientation[0];
		obj.ypr[1] = orientation[1];
		obj.ypr[2] = orientation[2];
		
		Objects.Insert(obj);		
	}
}
