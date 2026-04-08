class Boat_01_ColorBase : BoatScript
{
	override void EEInit()
	{		
		super.EEInit();

		SetAnimationPhase("ShowDamage",0);
		SetAnimationPhase("HideDamage",1);
	}

	override int GetAnimInstance()
	{
		return VehicleAnimInstances.ZODIAC;
	}

	override int GetSeatAnimationType(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		case 2:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 3:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		}

		return 0;
	}
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
			return false;
		
		string attType = attachment.GetType();
		
		if (EngineIsOn() && attType == "SparkPlug")
			return false;

		return true;
	}

	override bool CanDisplayAttachmentCategory(string category_name)
	{
		if ( !super.CanDisplayAttachmentCategory(category_name))
		{
			return false;
		}

		return true;
	}
	
	override bool CanDisplayCargo()
	{
		if (!super.CanDisplayCargo())
			return false;

		return true;
	}

	override bool CanReachSeatFromSeat(int currentSeat, int nextSeat)
	{
		return true;
	}

	override void OnDamageDestroyed(int oldLevel)
	{
		SetAnimationPhase("HideAntiwater",1);
	}

	override void OnDebugSpawn()
	{
		float amount = GetFluidCapacity(BoatFluid.FUEL);
		Fill(BoatFluid.FUEL, amount);

		//-----ATTACHMENTS
		GetInventory().CreateInInventory("Sparkplug");

	}
	
	// DEPRECATED
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;
}

class Boat_01_Blue : Boat_01_ColorBase {}
class Boat_01_Orange : Boat_01_ColorBase {}
class Boat_01_Black : Boat_01_ColorBase {}
class Boat_01_Camo : Boat_01_ColorBase {}
