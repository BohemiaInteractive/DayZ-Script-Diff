
class ActionEnterLadder: ActionInteractBase
{
	private const string GEOM_LOD_NAME 	= LOD.NAME_GEOMETRY;
	private	const string MEM_LOD_NAME 	= LOD.NAME_MEMORY;	

	void ActionEnterLadder()
	{
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#enter_ladder";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		//! is action_data.m_Target ? not on ladder and not falling ? 
		if (!target || !target.GetObject() || player.GetCommand_Ladder() || player.GetCommand_Fall() )
			return false;

		Building building;
		if (!Class.CastTo(building, target.GetObject()))
			return false;

		// TODO: direction tests
		// Get memory LOD from p3d and save all its selections
		LOD lod = building.GetLODByName(MEM_LOD_NAME);
		if(lod == NULL)
			return false;

		array<Selection> memSelection	= new array<Selection>();
		if(!lod.GetSelections(memSelection))
			return false;

		array<string> selections = new array<string>();
		string compName = "";
		building.GetActionComponentNameList(target.GetComponentIndex(), selections);
		foreach (string tmp : selections)
		{
			if (tmp.Length() >= 6 && tmp.Substring(0,6) == "ladder" && tmp.IndexOfFrom(6,"_") < 0)
			{
				compName = tmp;
				break;
			}
		}
		
		if (compName == string.Empty)
			return false;
		
		string condCompName = string.Format("%1_con", compName);
		vector pos = player.GetPosition();
		
		bool found = false;
		vector ladderEnterPointGlobal;
		vector ladderDirPointGlobal;
		float minDistanceSq = 100.0;
				
		string dirCompName = string.Format("%1_con_dir", compName);
				
		for ( int i = 0; i < memSelection.Count(); i++ )
		{
			if ( memSelection[i].GetName() == condCompName )
			{
					
				for( int j = 0; j < memSelection[i].GetVertexCount(); j++ )
				{
					ladderEnterPointGlobal = building.ModelToWorld( memSelection[i].GetVertexPosition(lod, j) );
					if( vector.DistanceSq(ladderEnterPointGlobal,pos) < UAMaxDistances.LADDERS * UAMaxDistances.LADDERS)
					{
						HumanCommandLadder.DebugDrawLadder(building, HumanCommandLadder.DebugGetLadderIndex(compName));
						found = true;
						break;
					}
				}
			}
		}
		
		if (found)
		{
			for (int k = 0; k < memSelection.Count(); k++)
			{
				if( memSelection[k].GetName() == dirCompName )
				{
					for( int l = 0; l < memSelection[k].GetVertexCount(); l++ )
					{
						vector dirPoint = building.ModelToWorld( memSelection[k].GetVertexPosition(lod, l) );
						float dst = vector.DistanceSq(ladderEnterPointGlobal,dirPoint);
						if(  dst < minDistanceSq)
						{
							minDistanceSq = dst;
							ladderDirPointGlobal = dirPoint;
						//HumanCommandLadder.DebugDrawLadder(building, HumanCommandLadder.DebugGetLadderIndex(compName));
						//found = true;
						}
					}
				}
			}
					
			pos = pos - ladderEnterPointGlobal;
			ladderDirPointGlobal = ladderDirPointGlobal - ladderEnterPointGlobal;
						
			float angle = Math.AbsFloat(pos.VectorToAngles()[0] - ladderDirPointGlobal.VectorToAngles()[0]);
					
			if ( angle < 90 || angle > 270)
			{	
				return true;
			}
		}

		return false;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start(action_data);
		
		Building building;
		if (Class.CastTo(building, action_data.m_Target.GetObject()))
		{
			array<string> selections = new array<string>();
			string compName = "";
			building.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);
			foreach (string tmp : selections)
			{
				if (tmp.Length() >= 6 && tmp.Substring(0,6) == "ladder" && tmp.IndexOfFrom(6,"_") < 0)
				{
					compName = tmp;
					break;
				}
			}
			
			int ladderIndex = HumanCommandLadder.DebugGetLadderIndex(compName);
			LOD geomLod = action_data.m_Target.GetObject().GetLODByName(GEOM_LOD_NAME);
			string ladderType = "metal";

			for (int i = 0; i < geomLod.GetPropertyCount(); ++i)
			{
				if (geomLod.GetPropertyName(i) == "laddertype")
				{
					ladderType = geomLod.GetPropertyValue(i);
					break;
				}
			}

			action_data.m_Player.SetClimbingLadderType(ladderType);
			action_data.m_Player.StartCommand_Ladder(building, ladderIndex );
		}
	}
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
	
	override bool IsInstant()
	{
		return true;
	
	}
	
	override bool CanBeUsedSwimming()
	{
		return true;
	}
};