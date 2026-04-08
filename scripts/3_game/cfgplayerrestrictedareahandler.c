class CfgPlayerRestrictedAreaHandler
{
	private static bool m_Initialized;
	
	static ref TStringArray m_PRAFiles = new TStringArray();
	static ref CfgPlayerRestrictedAreaJsonData m_Data = new CfgPlayerRestrictedAreaJsonData();
	
	//----------------------------------------------------------
	//load, inits, validation
	
	static bool LoadData()
	{
		if (m_Initialized)
			return true;
		
		m_Initialized = false;
		
		if (g_Game.ServerConfigGetInt( "enableCfgGameplayFile" )) //only reach into the CfgGameplayHandler if properly loaded!
			m_PRAFiles = CfgGameplayHandler.GetPlayerRestrictedAreaFiles();
		else
			m_PRAFiles = g_Game.GetMission().GetWorldData().GetDefaultPRAPaths();
		
		if (!m_PRAFiles)
			return true;

		string worldName;
		g_Game.GetWorldName(worldName);
	
		foreach (string filename : m_PRAFiles)
		{
			PlayerRestrictedAreaInstance area = new PlayerRestrictedAreaInstance();
			
			string errorMessage, path;

			path = string.Format("$mission:%1", filename);
			if (!JsonFileLoader<PlayerRestrictedAreaInstance>.LoadFile(path, area, errorMessage))
			{
				if (!FileExist(filename))
					path = string.Format("dz/worlds/%1/ce/%2", worldName, filename);
				
				if (!JsonFileLoader<PlayerRestrictedAreaInstance>.LoadFile(path, area, errorMessage))
				{
					ErrorEx(errorMessage);
					continue;
				}
			}
			
			if (area != null)
				m_Data.m_Areas.Insert(area);
		}
		
		m_Initialized = m_Data.InitializeAreas();
		
		return true;
	}
	
	static bool IsInitialized()
	{
		return m_Initialized;
	}
	
	//----------------------------------------------------------
	//synchronization
	
	static void SyncDataSend(notnull PlayerIdentity identity)
	{
		g_Game.RPCSingleParam(null, ERPCs.RPC_PLAYERRESTRICTEDAREAS_SYNC, new Param1<CfgPlayerRestrictedAreaJsonData>(m_Data), true, identity);
	}
	
	static void OnRPC(ParamsReadContext ctx)
	{
		Param1<CfgPlayerRestrictedAreaJsonData> data = new Param1<CfgPlayerRestrictedAreaJsonData>(null);
		
		if (ctx.Read(data))
		{
			m_Data = data.param1;
			m_Initialized = m_Data.m_ValidatedAreas.Count() > 0;
		}
		else
		{
			ErrorEx("CfgPlayerRestrictedAreaHandler - client failed to read incoming data");
		}
	}
	
	//----------------------------------------------------------
	//main area checking methods
	
	static bool IsPointInPlayerRestrictedAreaClosest(vector point, out PlayerRestrictedAreaInstance hitArea)
	{
		if (!IsInitialized())
			return false;
		
		PlayerRestrictedAreaInstance area = GetClosestArea(point);
		if (area && IsCylinderInAreaBox(area,point) || IsPointInAreaPolygon(area,point))
		{
			hitArea = area;
			return true;
		}
		return false;
	}
	
	static bool IsPointInPlayerRestrictedArea(vector point, out PlayerRestrictedAreaInstance hitArea)
	{
		if (!IsInitialized())
			return false;
		
		foreach (PlayerRestrictedAreaInstance area : m_Data.m_ValidatedAreas)
		{
			if (IsCylinderInAreaBox(area,point) || IsPointInAreaPolygon(area,point))
			{
				hitArea = area;
				return true;
			}
		}
		
		return false;
	}
	
	//----------------------------------------------------------
	//support methods
	
	private static PlayerRestrictedAreaInstance GetClosestArea(vector point)
	{
		PlayerRestrictedAreaInstance ret;
		float closestDist = float.MAX;
		float currentDist;
		
		foreach (PlayerRestrictedAreaInstance area : m_Data.m_ValidatedAreas)
		{
			currentDist = vector.DistanceSq(point,area.GetCenterPos2D());
			if (currentDist < closestDist)
			{
				ret = area;
				closestDist = currentDist;
			}
		}
		
		return ret;
	}
	
	//! default cylinder values sufficient for player detection
	private static bool IsCylinderInAreaBox(notnull PlayerRestrictedAreaInstance area, vector point, float cylinderRadius = 0.25, float cylinderHeight = 1)
	{
		foreach (PRAShapeBoxData boxData : area.m_PRABoxDataTranslated)
		{
			vector matPlayer4[4];
			Math3D.MatrixIdentity4(matPlayer4);
			matPlayer4[3] = point;
			if (Math3D.IntersectCylinderOBB(boxData.m_Mins,boxData.m_Maxs,boxData.m_Mat4,matPlayer4,cylinderRadius,cylinderHeight))
				return true;
		}
		
		return false;
	}
	
	private static bool IsPointInAreaPolygon(notnull PlayerRestrictedAreaInstance area, vector point)
	{
		array<float> translatedDta = new array<float>;
		foreach (array<ref array<float>> polygonData : area.PRAPolygons)
		{
			translatedDta.Clear();
			
			foreach (array<float> vertexPos : polygonData)
			{
				translatedDta.Insert(vertexPos[0]);
				translatedDta.Insert(vertexPos[1]);
			}
			
			if (Math2D.IsPointInPolygon(translatedDta,point[0],point[2]))
				return true;
		}
		
		return false;
	}
	
	//----------------------------------------------------------
	//debugs
	
	static private ref array<Shape> m_DbgShapesBoxes;
	static private ref array<Shape> m_DbgShapesPolygonLines;
	
	static void DrawBoxesDebug(bool draw)
	{
		if (!IsInitialized())
			return;
		
		if (!m_DbgShapesBoxes)
			m_DbgShapesBoxes = {};
		
		if (draw && m_DbgShapesBoxes.Count() == 0)
		{
			foreach (PlayerRestrictedAreaInstance area : m_Data.m_ValidatedAreas)
			{
				foreach (PRAShapeBoxData boxData : area.m_PRABoxDataTranslated)
				{
					Shape shp = Debug.DrawBox(boxData.m_Mins,boxData.m_Maxs);
					shp.SetMatrix(boxData.m_Mat4);
					m_DbgShapesBoxes.Insert(shp);
				}
			}
		}
		else if (!draw && m_DbgShapesBoxes.Count() > 0)
		{
			foreach (Shape box : m_DbgShapesBoxes)
			{
				box.Destroy();
			}
			
			m_DbgShapesBoxes.Clear();
		}
	}
	
	static void DrawPolygonLinesDebug(bool draw)
	{
		if (!IsInitialized())
			return;
		
		if (!m_DbgShapesPolygonLines)
			m_DbgShapesPolygonLines = {};
		
		if (draw && m_DbgShapesPolygonLines.Count() == 0)
		{
			array<vector> polygonVectors = {};
			foreach (PlayerRestrictedAreaInstance area : m_Data.m_ValidatedAreas)
			{
				foreach (array<ref array<float>> polygonData : area.PRAPolygons)
				{
					vector first;
					vector current;
					vector last;
					
					foreach (array<float> vertexPos : polygonData)
					{
						polygonVectors.Insert(Vector(vertexPos[0],g_Game.SurfaceY(vertexPos[0],vertexPos[1]) + 2,vertexPos[1]));
					}
					
					int count = polygonVectors.Count();
					for (int i = 0; i < count; ++i)
					{
						current = polygonVectors[i];
						
						if (i == 0)
							first = polygonVectors[i];
						else
							Debug.DrawLine(last,current,COLOR_WHITE,ShapeFlags.TRANSP|ShapeFlags.NOZWRITE);
						
						last = current;
					}
					
					m_DbgShapesPolygonLines.Insert(Debug.DrawLine(current,first,COLOR_RED,ShapeFlags.TRANSP|ShapeFlags.NOZWRITE));
					
					polygonVectors.Clear();
				}
			}
		}
		else if (!draw && m_DbgShapesPolygonLines.Count() > 0)
		{
			foreach (Shape item : m_DbgShapesPolygonLines)
			{
				item.Destroy();
			}
			
			m_DbgShapesPolygonLines.Clear();
		}
	}
}