class ShelterBase extends TentBase
{
	static const string SITE_OBJECT_TYPE = "ShelterSite";

	void ShelterBase()
	{
		m_ShowAnimationsWhenPitched.Insert("Body");
		m_ShowAnimationsWhenPacked.Insert("Body");
		
		m_HalfExtents = Vector(0.8, 0.15, 1.3);
	}

	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionTogglePlaceObject);
		RemoveAction(ActionToggleTentOpen);
		RemoveAction(ActionPackTent);
		RemoveAction(ActionDeployObject);
		
		AddAction(ActionDeconstructShelter);
	}
	
	override void EEInit()
	{		
		super.EEInit();
		
		TryPitch(true, true);
	}
	
	void Deconstruct()
	{
		DeconstructEx();
	}
	
	void DeconstructEx(bool createdItemsFall = false)
	{
		StaticConstructionMethods.SpawnConstructionMaterialPilesEx(this, null, GetMaterialPath(), GetShelterVariant(), "", false, createdItemsFall);
		SpawnKitEx(createdItemsFall);
		Delete();
	}
	
	string GetMaterialPath()
	{
		return string.Format("%1 %2 Construction site %3 Materials", CFG_VEHICLESPATH, SITE_OBJECT_TYPE, GetShelterVariant());
	}
	
	string GetShelterVariant()
	{
		return "";
	}
	
	void SpawnKit()
	{
		SpawnKitEx();
	}
	
	void SpawnKitEx(bool shouldKitFall = false)
	{
		ItemBase constructionKit = ItemBase.Cast(g_Game.CreateObjectEx("ShelterKit", GetPosition(), ECE_PLACE_ON_SURFACE));
		constructionKit.SetHealth01("", "", GetHealth01());
		
		if (shouldKitFall)
			constructionKit.ThrowPhysically(null, vector.Zero);
	}
	
	override bool IsPlayerInside(PlayerBase player, string selection)
	{
		vector player_pos = player.GetPosition();
		vector shelter_pos = GetPosition();
		vector ref_dir = GetDirection();
		ref_dir[1] = 0;
		ref_dir.Normalize();
		 
		vector x[2];
		vector b1,b2;
		GetCollisionBox(x);
		b1 = x[0];
		b2 = x[1];

		vector dir_to_shelter = shelter_pos - player_pos;
		dir_to_shelter[1] = 0;
		float len = dir_to_shelter.Length();

		dir_to_shelter.Normalize();
		
		vector ref_dir_angle = ref_dir.VectorToAngles();
		vector dir_to_shelter_angle = dir_to_shelter.VectorToAngles();
		vector test_angles = dir_to_shelter_angle - ref_dir_angle;
		
		vector test_position = test_angles.AnglesToVector() * len;
		
		if (test_position[0] < b1[0] || test_position[0] > b2[0] || test_position[2] < 0.2 || test_position[2] > 2.2 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	override void OnItemFallRebuilding()
	{
		if (!g_Game.IsServer() || m_IsFalling)
			 return;
		
		DropCargoItems();
		DeconstructEx(true);
		m_IsFalling = true;
		
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ClearIsFalling, 800);
	}
}

class ShelterFabric extends ShelterBase
{
	override string GetShelterVariant()
	{
		return "fabric";
	}
}

class ShelterLeather extends ShelterBase
{
	override string GetShelterVariant()
	{
		return "leather";
	}
}

class ShelterStick extends ShelterBase
{
	override string GetShelterVariant()
	{
		return "stick";
	}
}
