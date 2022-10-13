class ActionDigInStashCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DIG_STASH);
	}
};

class ActionDigInStash: ActionContinuousBase
{	
	static float m_DigStashSlopeTolerance = 0.6;
	
	void ActionDigInStash()
	{
		m_CallbackClass		= ActionDigInStashCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_DIGMANIPULATE;
		m_FullBody			= true;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight	= UASoftSkillsWeight.ROUGH_LOW;
		m_Text 				= "#bury";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget	= new CCTObject(UAMaxDistances.DEFAULT);
		m_ConditionItem		= new CCINonRuined();
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		ItemBase targetIB;
		if (Class.CastTo(targetIB, target.GetObject()) && targetIB.CanBeDigged())
		{
			if (player.IsPlacingLocal())
			{
				return false;
			}
			
			if (targetIB.IsRuined() || targetIB.GetInventory().IsAttachment())
			{
				return false;
			}
			
			if (targetIB.GetInventory().IsAttachment())
			{
				return false;
			}
			
			if (targetIB.IsInherited(UndergroundStash))
			{
				return false;
			}

			//! was initialized from inventory?
			EntityAI entityToCheck = targetIB;
			if (targetIB.GetInventory().IsInCargo());
				entityToCheck = player;
			
			// here we check if a stash is nearby and block digging a new one in close proximity
			array<Object> excludedObjects = new array<Object>;
			excludedObjects.Insert(targetIB);
			array<Object> nearbyObjects = new array<Object>;
			// For now we exclude an area of 2 X 2 X 2 meters
			if (GetGame().IsBoxColliding(entityToCheck.GetPosition(), entityToCheck.GetOrientation(), "2 2 2", excludedObjects, nearbyObjects))
			{
				for (int i = 0; i < nearbyObjects.Count(); i++)
				{
					if (nearbyObjects[i].IsInherited(UndergroundStash))
					{
						return false;
					}
				}
			}
			
			// Check surface
			int liquidType;
			string surfaceType;
			GetGame().SurfaceUnderObject(entityToCheck, surfaceType, liquidType);
			
			if (!GetGame().IsSurfaceDigable(surfaceType))
			{
				return false;
			}
			else
			{
				//! Check slope angle
				vector position = entityToCheck.GetPosition();
				
				array<vector> positions = new array<vector>;
				positions.Insert(position + "0.5 0 0.5");
				positions.Insert(position + "-0.5 0 0.5");
				positions.Insert(position + "0.5 0 -0.5");
				positions.Insert(position + "-0.5 0 -0.5");
				
				float difference = GetGame().GetHighestSurfaceYDifference(positions);
				
				return difference < m_DigStashSlopeTolerance;
			}
		}
		
		return false;
	}

	override void OnExecuteClient(ActionData action_data)
	{
		super.OnExecuteClient(action_data);
		
		SpawnParticleShovelRaise(action_data);
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);
		
		if (!GetGame().IsMultiplayer())
		{
			SpawnParticleShovelRaise(action_data);
		}
	}
	
	void SpawnParticleShovelRaise(ActionData action_data)
	{
		ParticleManager.GetInstance().PlayOnObject(ParticleList.DIGGING_STASH, action_data.m_Player);
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
		if (!targetEntity)
		{
			ErrorEx("Cannot get entity=" + targetEntity);
			return;
		}
		
		InventoryLocation targetIL = new InventoryLocation();
		if (!targetEntity.GetInventory().GetCurrentInventoryLocation(targetIL))
		{
			ErrorEx("Cannot get inventory location of entity=" + targetEntity);
			return;
		}
		
		UndergroundStash stash = UndergroundStash.Cast(GetGame().CreateObjectEx("UndergroundStash", targetEntity.GetPosition(), ECE_PLACE_ON_SURFACE));  
		if (stash)
		{
			stash.PlaceOnGround();
			if (GameInventory.LocationCanRemoveEntity(targetIL))
			{
				action_data.m_Player.ServerTakeEntityToTargetCargo(stash, targetEntity);
			}
			else
			{
				Debug.Log(string.Format("Cannot remove entity=%1 obj from current location=%2", targetEntity, InventoryLocation.DumpToStringNullSafe(targetIL)));
			}
		}
		else
		{
			ErrorEx("Stash not spawned!");
		}
		
		//Apply tool damage
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, 10);
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty(m_SpecialtyWeight);
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		return string.Format("Player %1 Dug in %2 at position %3", action_data.m_Player, action_data.m_Target.GetObject(), action_data.m_Target.GetObject().GetPosition());
	}
}
