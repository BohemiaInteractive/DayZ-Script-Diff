class CfgPlayerRestrictedAreaJsonData
{
	ref array<ref PlayerRestrictedAreaInstance> m_ValidatedAreas = new array<ref PlayerRestrictedAreaInstance>();
	ref array<ref PlayerRestrictedAreaInstance> m_Areas = new array<ref PlayerRestrictedAreaInstance>();
	
	bool InitializeAreas()
	{
		m_ValidatedAreas.Clear();
		
		foreach (PlayerRestrictedAreaInstance instance : m_Areas)
		{
			instance.Initialize();
			if (instance.IsValid())
				m_ValidatedAreas.Insert(instance);
		}
		
		m_Areas.Clear();
		
		return m_ValidatedAreas.Count() > 0;
	}
};

class PRAShapeDataBase
{
	void PRAShapeDataBase(array<ref array<float>> shapeDataArray)
	{
		InitPRAShapeData(shapeDataArray);
	}
	
	void InitPRAShapeData(array<ref array<float>> shapeDataArray)
	{
	}
}

class PRAShapeBoxData : PRAShapeDataBase
{
	vector m_Mins;
	vector m_Maxs;
	vector m_Mat4[4];
	
	override void InitPRAShapeData(array<ref array<float>> shapeDataArray)
	{
		vector sizes = Vector(shapeDataArray[0][0],shapeDataArray[0][1],shapeDataArray[0][2]);
		vector angle = Vector(shapeDataArray[1][0],shapeDataArray[1][1],shapeDataArray[1][2]);
		vector targetPoint = Vector(shapeDataArray[2][0],shapeDataArray[2][1],shapeDataArray[2][2]);
		
		m_Mins = Vector(-sizes[0]/2,-sizes[1]/2,-sizes[2]/2);
		m_Maxs = Vector(sizes[0]/2,sizes[1]/2,sizes[2]/2);
		
		vector mat3[3];
		Math3D.YawPitchRollMatrix(angle,mat3);
		
		m_Mat4[0] = mat3[0];
		m_Mat4[1] = mat3[1];
		m_Mat4[2] = mat3[2];
		m_Mat4[3] = targetPoint;
	}
}

class PlayerRestrictedAreaInstance
{
	//private
	private bool m_IsValid = false;
	private int m_LastUsedSafePosIdx = -1;
	private ref array<vector> m_TranslatedSafePositions3D;
	private ref array<vector> m_RandomizedSafePositions3D;
	//private ref map<vector,int> m_TimeIndexedSafePositions;
	private vector m_CenterPos2D; //averaged center position, used for initial filtering
	
	//public
	string areaName = ""; //optional
	ref array<ref array<ref array<float>>> PRABoxes = new array<ref array<ref array<float>>>; //!3D, not used directly!
	ref array<ref array<ref array<float>>> PRAPolygons = new array<ref array<ref array<float>>>; //2D
	ref array<ref array<float>> safePositions2D = new array<ref array<float>>;
	ref array<ref array<float>> safePositions3D = new array<ref array<float>>;
	
	ref array<ref PRAShapeBoxData> m_PRABoxDataTranslated = new array<ref PRAShapeBoxData>; //translated data
	
	void Initialize()
	{
		if (m_IsValid)
			return;
		
		m_IsValid = false;
		m_IsValid |= ValidatePlayerRestrictedAreaPositions();
		m_IsValid |= TranslateSafePositions();
	}
	
	private bool ValidatePlayerRestrictedAreaPositions()
	{
		if (PRABoxes.Count() == 0 && PRAPolygons.Count() == 0)
		{
			Debug.Log("No valid coordinates in 'PRABoxes' or 'PRAPolygons' for area: " + areaName,"n/a","n/a","","PlayerRestrictedAreaInstance");
			return false;
		}
		
		float xMin,zMin = float.MAX;
		float xMax,zMax = float.MIN;
		
		foreach (array<ref array<float>> boxFloatsData : PRABoxes)
		{
			if (boxFloatsData.Count() != 3)
			{
				Debug.Log("Invalid box defined in " + areaName + ". Box needs to have sizes, rotation, and position defined!","n/a","n/a","","PlayerRestrictedAreaInstance");
				return false;
			}
			
			array<float> boxVectors;
			//sizes
			boxVectors = boxFloatsData[0];
			if (boxVectors.Count() != 3)
			{
				Debug.Log("Invalid box defined in " + areaName + ". Box size needs to be in an XYZ format!","n/a","n/a","","PlayerRestrictedAreaInstance");
				return false;
			}
			
			//rotation
			boxVectors = boxFloatsData[1];
			if (boxVectors.Count() != 3)
			{
				Debug.Log("Invalid box defined in " + areaName + ". Box rotation needs to be in an XYZ format!","n/a","n/a","","PlayerRestrictedAreaInstance");
				return false;
			}
			
			//position
			boxVectors = boxFloatsData[2];
			if (boxVectors.Count() != 3)
			{
				Debug.Log("Invalid box defined in " + areaName + ". Box position needs to be in an XYZ format!","n/a","n/a","","PlayerRestrictedAreaInstance");
				return false;
			}
			
			//translate to data
			PRAShapeBoxData dta = new PRAShapeBoxData(boxFloatsData);
			if (dta)
			{
				m_PRABoxDataTranslated.Insert(dta);
				
				xMin = Math.Min(xMin,dta.m_Mins[0]);
				zMin = Math.Min(zMin,dta.m_Mins[2]);
				xMax = Math.Max(xMax,dta.m_Maxs[0]);
				zMax = Math.Max(zMax,dta.m_Maxs[2]);
			}
		}
		
		foreach (array<ref array<float>> polygonData : PRAPolygons)
		{
			if (polygonData.Count() < 3)
			{
				Debug.Log("Invalid polygon defined in " + areaName + ". At least 3 points necessary!","n/a","n/a","","PlayerRestrictedAreaInstance");
				return false;
			}
			
			foreach (array<float> polygonCoords : polygonData)
			{
				if (polygonCoords.Count() != 2)
				{
					Debug.Log("Invalid polygon defined in " + areaName + ". Polygon coordinates need to be in a XZ format!","n/a","n/a","","PlayerRestrictedAreaInstance");
					return false;
				}
				
				xMin = Math.Min(xMin,polygonCoords[0]);
				zMin = Math.Min(zMin,polygonCoords[1]);
				xMax = Math.Max(xMax,polygonCoords[0]);
				zMax = Math.Max(zMax,polygonCoords[1]);
			}
		}
		
		m_CenterPos2D = Vector(((xMin + xMax) / 2), 0, ((zMin + zMax) / 2));
		
		return true;
	}
	
	private bool TranslateSafePositions()
	{
		if (safePositions2D.Count() == 0 && safePositions3D.Count() == 0)
		{
			Debug.Log("Undefined safe positions for area: " + areaName,"n/a","n/a","","PlayerRestrictedAreaInstance");
			return false;
		}
		
		m_TranslatedSafePositions3D = new array<vector>;
		
		foreach (array<float> arr2d : safePositions2D)
		{
			m_TranslatedSafePositions3D.Insert(Vector(arr2d[0],g_Game.SurfaceY(arr2d[0],arr2d[1]),arr2d[1]));
		}
		
		foreach (array<float> arr3d : safePositions3D)
		{
			m_TranslatedSafePositions3D.Insert(Vector(arr3d[0],arr3d[1],arr3d[2]));
		}
		
		return true;
	}
	
	bool IsValid()
	{
		return m_IsValid;
	}
	
	vector GetCenterPos2D()
	{
		return m_CenterPos2D;
	}
	
	//----------------------------------------------------------
	//getters
	
	vector GetClosestSafePos3D(vector targetPos)
	{
		vector closestPos = targetPos;
		float smallestDist = float.MAX;
		float dist;
		
		foreach (vector pos : m_TranslatedSafePositions3D)
		{
			dist = vector.DistanceSq(targetPos, pos);
			if (dist < smallestDist)
			{
				smallestDist = dist;
				closestPos = pos;
			}
		}
		return closestPos;
	}
	
	vector GetRandomSafePos3D(vector targetPos)
	{
		if (!m_RandomizedSafePositions3D || m_RandomizedSafePositions3D.Count() == 0)
		{
			m_RandomizedSafePositions3D = new array<vector>;
			m_RandomizedSafePositions3D.Copy(m_TranslatedSafePositions3D);
			m_LastUsedSafePosIdx = -1;
		}
		
		int count = m_RandomizedSafePositions3D.Count();
		m_LastUsedSafePosIdx++;
		
		if (m_LastUsedSafePosIdx == count)
		{
			m_RandomizedSafePositions3D.ShuffleArray();
			m_LastUsedSafePosIdx = 0;
		}
		
		return m_RandomizedSafePositions3D[m_LastUsedSafePosIdx];
	}
}