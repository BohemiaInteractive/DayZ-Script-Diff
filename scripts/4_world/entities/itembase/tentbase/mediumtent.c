class MediumTent extends TentBase
{
	void MediumTent()
	{		
		m_ToggleAnimations.Insert( new ToggleAnimations("EntranceO", "EntranceC", OPENING_0), 0 );
			
		m_ShowAnimationsWhenPitched.Insert( "Body" );
		//m_ShowAnimationsWhenPitched.Insert( "EntranceO" );
		m_ShowAnimationsWhenPitched.Insert( "Pack" );

		m_ShowAnimationsWhenPacked.Insert( "Inventory" );
		
		m_HalfExtents = Vector(0.8, 0.15, 1.3);
	}
		
	override void EEInit()
	{		
		super.EEInit();
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{		
		super.OnItemLocationChanged(old_owner, new_owner);
	}
	
	override string GetSoundOpen()
	{
		return "MediumTent_Door_Open_SoundSet";
	}
	
	override string GetSoundClose()
	{
		return "MediumTent_Door_Close_SoundSet";
	}
	
	override bool HasClutterCutter()
	{
		return true;
	}
	
	override string GetClutterCutter()
	{
		return "MediumTentClutterCutter";
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
		
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		PlayerBase pb = PlayerBase.Cast( player );
				
		if ( GetGame().IsServer() )
		{
			if ( !m_ClutterCutter )
			{
				m_ClutterCutter = GetGame().CreateObjectEx( "MediumTentClutterCutter", pb.GetLocalProjectionPosition(), ECE_PLACE_ON_SURFACE );	
				m_ClutterCutter.SetOrientation( pb.GetLocalProjectionOrientation() );
			}
		}	
	}
		
	override string GetDeploySoundset()
	{
		return "placeMediumTent_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "mediumtent_deploy_SoundSet";
	}	
};

class MediumTent_Green extends MediumTent {};
class MediumTent_Orange extends MediumTent {};

//placing classes
class MediumTentPlacing extends MediumTent {};
class MediumTent_GreenPlacing extends MediumTentPlacing {};
class MediumTent_OrangePlacing extends MediumTentPlacing {};