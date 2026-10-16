//! Contains derived info for multutude of uses
class InstancedAttachmentGroupDataBase //TODO: move!
{
	protected EntityAI m_Parent;
	
	void InstancedAttachmentGroupDataBase(EntityAI parent)
	{
		m_Parent = parent;
	}
	
	void Init();
}

class InstancedAttachmentGroupDataBuilding : InstancedAttachmentGroupDataBase
{
	protected ref map<string,ref AttachmentGroupProximityMetadata> m_AttGroupPositionData; //! single position related to each particular attachment group
	
	override void Init()
	{
		super.Init();
		
		TAttachmentGroupMap groupMap = m_Parent.GetTypeAttachmentGroupInfoHolder().GetAttachmentGroupMap();
		if (groupMap)
		{
			m_AttGroupPositionData = new map<string,ref AttachmentGroupProximityMetadata>();
			foreach (string groupName, ref TypeAttachmentGroupData groupData : groupMap)
			{
				bool createMetadata = false;
				AttachmentGroupPositionData posData;
				AttachmentGroupBoxData boxData;
				
				vector pos;
				float radius;
				if (CalculatePositionData(groupData.m_SelectionName,pos,radius))
				{
					createMetadata = true;
					posData = new AttachmentGroupPositionData(pos,radius);
				}
				
				vector min,max;
				if (CalculateBoxData(groupData.m_SelectionName,min,max))
				{
					createMetadata = true;
					boxData = new AttachmentGroupBoxData(min,max);
				}
				
				if (createMetadata)
					m_AttGroupPositionData.Set(groupName,new AttachmentGroupProximityMetadata(posData,boxData));
			}
		}
	}
	
	//! False if no valid position found
	protected bool CalculatePositionData(string namedSelection, out vector position, out float radius)
	{
		radius = 2.0; //UAMaxDistances.DEFAULT;
		
		if (m_Parent.MemoryPointExists(namedSelection)) //first look for mem point
		{
			position = m_Parent.ModelToWorld(m_Parent.GetMemoryPointPos(namedSelection));
			string memRadiusSelection = "" + namedSelection + "_radius";
			if (m_Parent.MemoryPointExists(memRadiusSelection)) //if using mem point, look for radius point too
			{
				vector radiusPoint = m_Parent.ModelToWorld(m_Parent.GetMemoryPointPos(memRadiusSelection));
				radius = vector.Distance(position,radiusPoint);
			}
			
			return true;
		}
		else //if not found, look for the named component center (building-specific solution, position possibly averaged from multiple components!)
		{
			TIntArray ost = new TIntArray();
			m_Parent.GetActionComponentsForSelectionName(m_Parent.GetViewGeometryLevel(),namedSelection,ost);
			
			if (ost.Count() == 0)
				return false;
			
			bool validPosFilled = false;
			vector tmpPos;
			foreach (int comp : ost)
			{
				if (!validPosFilled)
				{
					tmpPos = m_Parent.ModelToWorld(m_Parent.GetActionComponentCenter(m_Parent.GetViewGeometryLevel(),comp));
					validPosFilled = true;
				}
				else
				{
					tmpPos = tmpPos + m_Parent.ModelToWorld(m_Parent.GetActionComponentCenter(m_Parent.GetViewGeometryLevel(),comp));
				}
			}
			
			position = tmpPos * (1 / ost.Count());
			return true;
		}
	}
	
	//! False if no valid box points found
	protected bool CalculateBoxData(string namedSelection, out vector min, out vector max)
	{
		string minSelection = "" + namedSelection + "_min";
		string maxSelection = "" + namedSelection + "_max";
		if (m_Parent.MemoryPointExists(minSelection) && m_Parent.MemoryPointExists(maxSelection))
		{
			min = m_Parent.ModelToWorld(m_Parent.GetMemoryPointPos(minSelection));
			max = m_Parent.ModelToWorld(m_Parent.GetMemoryPointPos(maxSelection));
			//sanitize minsmaxs
			vector.SanitizeMinsMaxs(min,max);
			return true;
		}
		
		return false;
	}
	
	map<string,ref AttachmentGroupProximityMetadata> GetAttGroupPositionMap()
	{
		return m_AttGroupPositionData;
	}
	
	bool GetValidAttGroupPosition(string groupName, out vector position, out float radius)
	{
		AttachmentGroupProximityMetadata metaData = m_AttGroupPositionData.Get(groupName);
		if (metaData)
			return metaData.GetValidRadialCheckData(position,radius);
		
		return false;
	}
	
	bool GetValidAttGroupBox(string groupName, out vector min, out vector max)
	{
		AttachmentGroupProximityMetadata metaData = m_AttGroupPositionData.Get(groupName);
		if (metaData)
			return metaData.GetValidBoxCheckData(min,max);
		
		return false;
	}
}

//! Extended metadata for group position
class AttachmentGroupProximityMetadata
{
	protected ref AttachmentGroupPositionData m_PositionData;
	protected ref AttachmentGroupBoxData m_BoxData;
	
	void AttachmentGroupProximityMetadata(AttachmentGroupPositionData posData, AttachmentGroupBoxData boxData)
	{
		m_PositionData = posData;
		m_BoxData = boxData;
	}
	
	bool GetValidRadialCheckData(out vector position, out float radius)
	{
		if (m_PositionData)
		{
			position = m_PositionData.m_Position;
			radius = m_PositionData.m_Radius;
			return true;
		}
		
		return false;
	}
	
	bool GetValidBoxCheckData(out vector min, out vector max)
	{
		if (m_BoxData)
		{
			min = m_BoxData.m_Min;
			max = m_BoxData.m_Max;
			return true;
		}
		
		return false;
	}
}

class AttachmentGroupPositionData
{
	vector m_Position;
	float m_Radius;
	
	void AttachmentGroupPositionData(vector pos, float radius)
	{
		m_Position = pos;
		m_Radius = radius;
	}
}

class AttachmentGroupBoxData
{
	vector m_Min;
	vector m_Max;
	
	void AttachmentGroupBoxData(vector min, vector max)
	{
		m_Min = min;
		m_Max = max;
	}
}