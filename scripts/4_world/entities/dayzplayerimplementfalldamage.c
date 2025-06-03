class FallDamageData
{
	int m_LandType;
	float m_Height;
	float m_HealthCoef;
	float m_ShockCoef;
	float m_BrokenLegsCoef;
	float m_BleedingHandsCoef;
	float m_BleedingFeetCoef;
	float m_BleedingLegsCoef;
	float m_GlovesHealthCoef;
	float m_ShoesHealthCoef;
	float m_PantsHealthCoef;
	float m_BleedingChanceCoef;
	float m_BleedingChanceLegsCoef;
}

class DayZPlayerImplementFallDamage
{
	static const string	FALL_DAMAGE_AMMO_HEALTH = "FallDamageHealth";
	static const string	FALL_DAMAGE_AMMO_SHOCK 	= "FallDamageShock";

	static const string	FALL_DAMAGE_AMMO_HEALTH_ATTACHMENT			= "FallDamageHealthAttachment";
	static const string	FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS	= "FallDamageHealthOtherAttachments";

	const float		HEALTH_HEIGHT_LOW 		= 5;
	const float		HEALTH_HEIGHT_HIGH 		= 14;

	private const float		SHOCK_HEIGHT_LOW 		= 3;
	private const float		SHOCK_HEIGHT_HIGH 		= 12;
	private const float		BROKENLEGS_HEIGHT_LOW 	= 5;
	private const float		BROKENLEGS_HEIGHT_HIGH 	= 9;
	
	private const float		GLOVES_HEALTH_HEIGHT_LOW 		= 5;
	private const float		GLOVES_HEALTH_HEIGHT_HIGH 		= 15;
	private const float		SHOES_HEALTH_HEIGHT_LOW 		= 2;
	private const float		SHOES_HEALTH_HEIGHT_HIGH 		= 15;
	private const float		PANTS_HEALTH_HEIGHT_LOW 		= 5;
	private const float		PANTS_HEALTH_HEIGHT_HIGH 		= 16;
	
	private const float		BLEEDINGHANDS_HEIGHT_LOW 	= 5;
	private const float		BLEEDINGFEET_HEIGHT_LOW 	= 3;
	private const float		BLEEDINGLEGS_HEIGHT_LOW 	= 10;

	private const float		RANDOM_THRESHOLD_HEALTH_LOW 		= 0.05;
	private const float		RANDOM_THRESHOLD_HEALTH_HIGH 		= 0.1;
	private const float		RANDOM_THRESHOLD_LEGS_LOW 			= 0.05;
	private const float		RANDOM_THRESHOLD_LEGS_HIGH 			= 0.1;
	private const float		RANDOM_THRESHOLD_ATTACHMENTS_LOW	= 0;
	private const float		RANDOM_THRESHOLD_ATTACHMENTS_HIGH 	= 0.2;
	
	private const float     BLEEDING_CHANCE_HEIGHT_LOW		= 3;
	private const float     BLEEDING_CHANCE_HEIGHT_HIGH		= 15;
	
	private static const float	BROKENLEGS_HEALTH_DAMAGE_MAX = 125;
	
	private static const string BLEEDING_HANDS_SELECTIONS[2] = {"LeftForeArmRoll", "RightForeArmRoll"};
	private static const string BLEEDING_FEET_SELECTIONS[2] = {"LeftFoot", "RightFoot"};
	private static const string BLEEDING_LEGS_SELECTIONS[2] = {"LeftLeg", "RightLeg"};
	
	private const int DAMAGE_TYPE_GLOBAL 		= 0;
	private const int DAMAGE_TYPE_LEGS			= 1;
	private const int DAMAGE_TYPE_ATTACHMENTS 	= 2;
	
	DayZPlayer 		m_Player;
	private ref FallDamageData m_FallDamageData;
	
	#ifdef DIAG_DEVELOPER
	bool m_Debug = false;
	private static ref map<string, float> m_AmmoTypeData; 
	#endif

	void DayZPlayerImplementFallDamage(DayZPlayer pPlayer)
	{
		m_Player 	= pPlayer;
		m_FallDamageData = new FallDamageData();
		
		#ifdef DIAG_DEVELOPER
		PreloadAmmoTypeData();
		#endif
	}
	
	float GetHeight()
	{
		return m_FallDamageData.m_Height;
	}

	int GetLandType()
	{
		return m_FallDamageData.m_LandType;
	}

	void HandleFallDamage(FallDamageData pData)
	{
		m_FallDamageData = pData;
		
		m_FallDamageData.m_HealthCoef = Math.InverseLerp(HEALTH_HEIGHT_LOW, HEALTH_HEIGHT_HIGH, pData.m_Height);
		m_FallDamageData.m_HealthCoef = Randomize(DAMAGE_TYPE_GLOBAL, m_FallDamageData.m_HealthCoef);
		m_FallDamageData.m_HealthCoef = Math.Clamp(m_FallDamageData.m_HealthCoef, 0, 1);

		m_FallDamageData.m_ShockCoef = Math.InverseLerp(SHOCK_HEIGHT_LOW, SHOCK_HEIGHT_HIGH, pData.m_Height);
		m_FallDamageData.m_ShockCoef = Math.Clamp(m_FallDamageData.m_ShockCoef, 0, 1);

		m_FallDamageData.m_BrokenLegsCoef = Math.InverseLerp(BROKENLEGS_HEIGHT_LOW, BROKENLEGS_HEIGHT_HIGH, pData.m_Height);
		m_FallDamageData.m_BrokenLegsCoef = Randomize(DAMAGE_TYPE_LEGS, m_FallDamageData.m_BrokenLegsCoef);
		m_FallDamageData.m_BrokenLegsCoef = Math.Clamp(m_FallDamageData.m_BrokenLegsCoef, 0, 1);

		m_FallDamageData.m_BleedingHandsCoef = Math.InverseLerp(0, BLEEDINGHANDS_HEIGHT_LOW, pData.m_Height);
		m_FallDamageData.m_BleedingHandsCoef = Math.Clamp(m_FallDamageData.m_BleedingHandsCoef, 0, 1);
		
		m_FallDamageData.m_BleedingFeetCoef = Math.InverseLerp(0, BLEEDINGFEET_HEIGHT_LOW, pData.m_Height);
		m_FallDamageData.m_BleedingFeetCoef = Math.Clamp(m_FallDamageData.m_BleedingFeetCoef, 0, 1);

		m_FallDamageData.m_BleedingLegsCoef = Math.InverseLerp(0, BLEEDINGLEGS_HEIGHT_LOW, pData.m_Height);
		m_FallDamageData.m_BleedingLegsCoef = Math.Clamp(m_FallDamageData.m_BleedingLegsCoef, 0, 1);
		
		m_FallDamageData.m_GlovesHealthCoef = Math.InverseLerp(GLOVES_HEALTH_HEIGHT_LOW, GLOVES_HEALTH_HEIGHT_HIGH, pData.m_Height);
		m_FallDamageData.m_GlovesHealthCoef = Randomize(DAMAGE_TYPE_ATTACHMENTS, m_FallDamageData.m_GlovesHealthCoef);
		m_FallDamageData.m_GlovesHealthCoef = Math.Clamp(m_FallDamageData.m_GlovesHealthCoef, 0, 1);
		
		m_FallDamageData.m_ShoesHealthCoef = Math.InverseLerp(SHOES_HEALTH_HEIGHT_LOW, SHOES_HEALTH_HEIGHT_HIGH, pData.m_Height);
		m_FallDamageData.m_ShoesHealthCoef = Randomize(DAMAGE_TYPE_ATTACHMENTS, m_FallDamageData.m_ShoesHealthCoef);
		m_FallDamageData.m_ShoesHealthCoef = Math.Clamp(m_FallDamageData.m_ShoesHealthCoef, 0, 1);
		
		m_FallDamageData.m_PantsHealthCoef = Math.InverseLerp(PANTS_HEALTH_HEIGHT_LOW, PANTS_HEALTH_HEIGHT_HIGH, pData.m_Height);
		m_FallDamageData.m_PantsHealthCoef = Randomize(DAMAGE_TYPE_ATTACHMENTS, m_FallDamageData.m_PantsHealthCoef);
		m_FallDamageData.m_PantsHealthCoef = Math.Clamp(m_FallDamageData.m_PantsHealthCoef, 0, 1);
		
		m_FallDamageData.m_BleedingChanceCoef = Math.InverseLerp(BLEEDING_CHANCE_HEIGHT_LOW, BLEEDING_CHANCE_HEIGHT_HIGH, pData.m_Height);
		m_FallDamageData.m_BleedingChanceCoef = Math.Clamp(m_FallDamageData.m_BleedingChanceCoef, 0, 0.7);
		
		m_FallDamageData.m_BleedingChanceLegsCoef = 0.15;
		
		if (GetGame().IsServer())
		{
			PlayerBase playerBase = PlayerBase.Cast(m_Player);
			if (playerBase)
			{
				AttachBleedingToZonesByHeight(playerBase);
				DamageAttachedGear(playerBase);
				playerBase.DamageAllLegs(BROKENLEGS_HEALTH_DAMAGE_MAX * m_FallDamageData.m_BrokenLegsCoef);
				playerBase.ForceUpdateInjuredState();
			}

			m_Player.ProcessDirectDamage(DamageType.CUSTOM, m_Player, "", FALL_DAMAGE_AMMO_HEALTH, vector.Zero, m_FallDamageData.m_HealthCoef);
			m_Player.ProcessDirectDamage(DamageType.CUSTOM, m_Player, "", FALL_DAMAGE_AMMO_SHOCK, vector.Zero, m_FallDamageData.m_ShockCoef);
				
		}
	}
	
	private float Randomize(int pType, float pValue)
	{
		Math.Randomize(GetGame().GetTime());
		
		int randomizedSign = -1;
		if (Math.RandomIntInclusive(1, 2) % 2 == 1)
			randomizedSign = 1;

		float randomizationValue = 0;
		switch (pType)
		{
			case DAMAGE_TYPE_GLOBAL:
				randomizationValue = Math.RandomFloatInclusive(RANDOM_THRESHOLD_HEALTH_LOW, RANDOM_THRESHOLD_HEALTH_HIGH);
				break;
			case DAMAGE_TYPE_LEGS:
				randomizationValue = Math.RandomFloatInclusive(RANDOM_THRESHOLD_LEGS_LOW, RANDOM_THRESHOLD_LEGS_HIGH);
				break;
			case DAMAGE_TYPE_ATTACHMENTS:
				randomizationValue = Math.RandomFloatInclusive(RANDOM_THRESHOLD_ATTACHMENTS_LOW, RANDOM_THRESHOLD_ATTACHMENTS_HIGH);
				break;
		}
		
		return pValue + (randomizedSign * pValue * randomizationValue);
	}
	
	private void AttachBleedingToZonesByHeight(notnull PlayerBase pPlayer)
	{
		Math.Randomize(GetGame().GetTime());

			int bleedingSelectionIndex;
			BleedingSourcesManagerServer bleedingManager = pPlayer.GetBleedingManagerServer();
			if (Math.RandomFloatInclusive(0.0, 1.0) <= m_FallDamageData.m_BleedingChanceCoef && m_FallDamageData.m_BleedingHandsCoef == 1.0 && pPlayer.FindAttachmentBySlotName("Gloves") == null)
			{
				bleedingSelectionIndex = Math.RandomInt(0, 2);
				bleedingManager.AttemptAddBleedingSourceBySelection(BLEEDING_HANDS_SELECTIONS[bleedingSelectionIndex]);
			}
			
			if (Math.RandomFloatInclusive(0.0, 1.0) <= m_FallDamageData.m_BleedingChanceCoef && m_FallDamageData.m_BleedingFeetCoef == 1.0 && pPlayer.FindAttachmentBySlotName("Feet") == null)
			{
				
				bleedingSelectionIndex = Math.RandomInt(0, 2);
				bleedingManager.AttemptAddBleedingSourceBySelection(BLEEDING_FEET_SELECTIONS[bleedingSelectionIndex]);
			}	
			if (Math.RandomFloatInclusive(0.0, 1.0) <= m_FallDamageData.m_BleedingChanceLegsCoef && m_FallDamageData.m_BleedingLegsCoef == 1.0)
			{
				
				bleedingSelectionIndex = Math.RandomInt(0, 2);
				bleedingManager.AttemptAddBleedingSourceBySelection(BLEEDING_LEGS_SELECTIONS[bleedingSelectionIndex]);
			}
	}
	
	private void DamageAttachedGear(notnull PlayerBase pPlayer)
	{
		EntityAI gloves = pPlayer.FindAttachmentBySlotName("Gloves");
		if (gloves)
			gloves.ProcessDirectDamage(DamageType.CUSTOM, pPlayer, "", FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS, gloves.WorldToModel(gloves.GetPosition()), m_FallDamageData.m_GlovesHealthCoef);

		EntityAI shoes = pPlayer.FindAttachmentBySlotName("Feet");
		if (shoes)
			shoes.ProcessDirectDamage(DamageType.CUSTOM, pPlayer, "", FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS, shoes.WorldToModel(shoes.GetPosition()), m_FallDamageData.m_ShoesHealthCoef);
		
		EntityAI pants = pPlayer.FindAttachmentBySlotName("Legs");
		if (pants)
			pants.ProcessDirectDamage(DamageType.CUSTOM, pPlayer, "", FALL_DAMAGE_AMMO_HEALTH_ATTACHMENT, pants.WorldToModel(pants.GetPosition()), m_FallDamageData.m_PantsHealthCoef);
	}

#ifdef DIAG_DEVELOPER
	FallDamageDebugData GetFallDamageDebugData()
	{
		FallDamageDebugData data = new FallDamageDebugData();
		data.Synch(this);

		return data;
	}
	
	void ShowFallDamageDebugInfo(bool enabled)
	{
		FallDamageDebugData data = GetFallDamageDebugData();
		DisplayFallDamageDebugInfo(enabled, data);
	}
	
	private static string LandTypeToString(int landType)
	{
		switch (landType)
		{
			case 1:
				return "LIGHT";
			case 2:
				return "MEDIUM";
			case 3:
				return "HEAVY";
		}
		
		return "NONE";
	}
	
	private void PreloadAmmoTypeData()
	{
		if (m_AmmoTypeData == null)
			m_AmmoTypeData = new map<string, float>();
		
		m_AmmoTypeData.Insert(FALL_DAMAGE_AMMO_HEALTH, GetValuesFromAmmoType(FALL_DAMAGE_AMMO_HEALTH)[0]);
		m_AmmoTypeData.Insert(FALL_DAMAGE_AMMO_SHOCK, GetValuesFromAmmoType(FALL_DAMAGE_AMMO_SHOCK)[2]);
		m_AmmoTypeData.Insert(FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS, GetValuesFromAmmoType(FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS)[0]);
		m_AmmoTypeData.Insert(FALL_DAMAGE_AMMO_HEALTH_ATTACHMENT, GetValuesFromAmmoType(FALL_DAMAGE_AMMO_HEALTH_ATTACHMENT)[0]);
	}
	
	private vector GetValuesFromAmmoType(string pAmmoType)
	{
		vector values = vector.Zero;
		values[0] = GetGame().ConfigGetFloat(string.Format("CfgAmmo %1 DamageApplied Health damage", pAmmoType));
		values[1] = GetGame().ConfigGetFloat(string.Format("CfgAmmo %1 DamageApplied Blood damage", pAmmoType));
		values[2] = GetGame().ConfigGetFloat(string.Format("CfgAmmo %1 DamageApplied Shock damage", pAmmoType));
		
		return values;
	}

	static void DisplayFallDamageDebugInfo(bool enabled, FallDamageDebugData data)
	{
		int windowPosX = 10;
		int windowPosY = 200;

		DbgUI.Begin("Fall Damage (last)", windowPosX, windowPosY);
		if (enabled)
		{
			DbgUI.Text(string.Format("Height:\t%1", Math.Clamp(data.m_Height, 0, float.MAX)));
			DbgUI.Text(string.Format("Land Type:\t%1 (%2)", data.m_LandType, LandTypeToString(data.m_LandType)));
			DbgUI.Text("");
			DbgUI.Text("Health");
			DbgUI.Text(string.Format("  coef:%1 dmg:%2", data.m_HealthCoef, m_AmmoTypeData[FALL_DAMAGE_AMMO_HEALTH] * data.m_HealthCoef));
			DbgUI.Text("Shock");
			DbgUI.Text(string.Format("  coef:%1 dmg:%2", data.m_ShockCoef, m_AmmoTypeData[FALL_DAMAGE_AMMO_SHOCK] * data.m_ShockCoef));
			DbgUI.Text("Broken Legs:");
			DbgUI.Text(string.Format("  coef:%1 dmg:%2", data.m_BrokenLegsCoef, BROKENLEGS_HEALTH_DAMAGE_MAX * data.m_BrokenLegsCoef));
			DbgUI.Text("Gloves damage:");
			DbgUI.Text(string.Format("  coef:%1 dmg:%2", data.m_GlovesHealthCoef, m_AmmoTypeData[FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS] * data.m_GlovesHealthCoef));
			DbgUI.Text("Shoes damage:");
			DbgUI.Text(string.Format("  coef:%1 dmg:%2", data.m_ShoesHealthCoef, m_AmmoTypeData[FALL_DAMAGE_AMMO_HEALTH_OTHER_ATTACHMENTS] * data.m_ShoesHealthCoef));
			DbgUI.Text("Pants damage:");
			DbgUI.Text(string.Format("  coef:%1 dmg:%2" , data.m_PantsHealthCoef, m_AmmoTypeData[FALL_DAMAGE_AMMO_HEALTH_ATTACHMENT] * data.m_PantsHealthCoef));
			DbgUI.Text("Bleeding coefs:");
			DbgUI.Text(string.Format("  hands:%1", data.m_BleedingHandsCoef));
			DbgUI.Text(string.Format("  feet:%1", data.m_BleedingFeetCoef));
			DbgUI.Text(string.Format("  legs:%1", data.m_BleedingLegsCoef));
			DbgUI.Text(string.Format("  H&F chance:%1", data.m_BleedingChanceCoef));
			
			
		}
		DbgUI.End();
	}
	
	void FillDebugFallDamageData(FallDamageDebugData data)
	{
		data.m_Height 			= m_FallDamageData.m_Height;
		data.m_LandType 		= m_FallDamageData.m_LandType;
		data.m_HealthCoef 		= m_FallDamageData.m_HealthCoef;
		data.m_ShockCoef 		= m_FallDamageData.m_ShockCoef;
		data.m_BrokenLegsCoef 	= m_FallDamageData.m_BrokenLegsCoef;
		data.m_GlovesHealthCoef = m_FallDamageData.m_GlovesHealthCoef;
		data.m_ShoesHealthCoef 	= m_FallDamageData.m_ShoesHealthCoef;
		data.m_PantsHealthCoef 	= m_FallDamageData.m_PantsHealthCoef;

		data.m_BleedingHandsCoef 	= m_FallDamageData.m_BleedingHandsCoef;
		data.m_BleedingFeetCoef 	= m_FallDamageData.m_BleedingFeetCoef;
		data.m_BleedingLegsCoef 	= m_FallDamageData.m_BleedingLegsCoef;
		data.m_BleedingChanceCoef 	= m_FallDamageData.m_BleedingChanceCoef;
	}
#endif
	
	//!
	//! DEPRECATED
	//!
	const string 	FD_AMMO					= "FallDamage";	//!< ammo type used for damaging
	const float		FD_DMG_FROM_HEIGHT		= 2.5;			//!< damage will not be taken into account bellow this HeightToDamage
	const float		FD_MAX_DMG_AT_HEIGHT	= 15;			//!< height where player gets 100% damage
	const float		FD_MAX_HEIGHT_LEG_BREAK = 8;			//!< height where legs break most of the time

	void HandleFallDamage(float pHeight);
	float DamageCoef(float pHeight);
}

#ifdef DIAG_DEVELOPER
class FallDamageDebugData : Param
{
	int 	m_LandType;
	float 	m_Height;
	float 	m_HealthCoef;
	float 	m_ShockCoef;
	float 	m_BrokenLegsCoef;
	float 	m_GlovesHealthCoef;
	float 	m_ShoesHealthCoef;
	float 	m_PantsHealthCoef;
	float 	m_BleedingHandsCoef;
	float 	m_BleedingFeetCoef;
	float 	m_BleedingLegsCoef;
	float	m_BleedingChanceCoef;
	
	void Synch(DayZPlayerImplementFallDamage fallDamage)
	{
		m_Height = fallDamage.GetHeight();
		m_LandType = fallDamage.GetLandType();
		
		fallDamage.FillDebugFallDamageData(this);
	}
	
	override bool Serialize(Serializer ctx)
	{
		return ctx.Write(m_Height) && ctx.Write(m_LandType) && ctx.Write(m_HealthCoef) && ctx.Write(m_ShockCoef) && ctx.Write(m_BrokenLegsCoef) && ctx.Write(m_GlovesHealthCoef) && ctx.Write(m_ShoesHealthCoef) && ctx.Write(m_PantsHealthCoef) && ctx.Write(m_BleedingHandsCoef) && ctx.Write(m_BleedingFeetCoef) && ctx.Write(m_BleedingLegsCoef);
	}
	
	override bool Deserializer(Serializer ctx)
	{
		return ctx.Read(m_Height) && ctx.Read(m_LandType) && ctx.Read(m_HealthCoef) && ctx.Read(m_ShockCoef) && ctx.Write(m_BrokenLegsCoef) && ctx.Read(m_GlovesHealthCoef) && ctx.Read(m_ShoesHealthCoef) && ctx.Read(m_PantsHealthCoef) && ctx.Read(m_BleedingHandsCoef) && ctx.Read(m_BleedingFeetCoef) && ctx.Read(m_BleedingLegsCoef);
	}
}
#endif
