class PartyTent extends TentBase
{
	void PartyTent()
	{
		m_ToggleAnimations.Insert( new ToggleAnimations("Door1o", "Door1c", OPENING_0), 0 );
		m_ToggleAnimations.Insert( new ToggleAnimations("Door2o", "Door2c", OPENING_1), 0 );
		m_ToggleAnimations.Insert( new ToggleAnimations("Door3o", "Door3c", OPENING_2), 0 );
		m_ToggleAnimations.Insert( new ToggleAnimations("Door4o", "Door4c", OPENING_3), 0 );
		m_ToggleAnimations.Insert( new ToggleAnimations("Door5o", "Door5c", OPENING_4), 0 );
		m_ToggleAnimations.Insert( new ToggleAnimations("Door6o", "Door6c", OPENING_5), 0 );
		
		m_ShowAnimationsWhenPitched.Insert( "Body" );
		m_ShowAnimationsWhenPitched.Insert( "Pack" );
		/*m_ShowAnimationsWhenPitched.Insert( "Door1o" );
		m_ShowAnimationsWhenPitched.Insert( "Door2o" );
		m_ShowAnimationsWhenPitched.Insert( "Door3o" );
		m_ShowAnimationsWhenPitched.Insert( "Door4o" );
		m_ShowAnimationsWhenPitched.Insert( "Door5o" );
		m_ShowAnimationsWhenPitched.Insert( "Door6o" );*/
		
		m_ShowAnimationsWhenPacked.Insert( "Inventory" );
		
		m_HalfExtents = Vector(1.3, 0.35, 2.7);
	}
	
	override string GetSoundOpen()
	{
		return "LargeTent_Door_Open_SoundSet";
	}
	
	override string GetSoundClose()
	{
		return "LargeTent_Door_Close_SoundSet";
	}
	
	override string GetSoundOpenWindow()
	{
		return "LargeTent_Window_Open_SoundSet";
	}
	
	override string GetSoundCloseWindow()
	{
		return "LargeTent_Window_Close_SoundSet";
	}
		
	override bool HasClutterCutter()
	{
		return true;
	}
	
	override string GetClutterCutter()
	{
		return "LargeTentClutterCutter"; //TODO add custom clutter cutter?
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
				m_ClutterCutter = GetGame().CreateObjectEx( "LargeTentClutterCutter", pb.GetLocalProjectionPosition(), ECE_PLACE_ON_SURFACE );
				m_ClutterCutter.SetOrientation( pb.GetLocalProjectionOrientation() );	
			}
		}	
	}
	
	override string GetDeploySoundset()
	{
		return "placeLargeTent_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "largetent_deploy_SoundSet";
	}
	
	override void HandleCamoNetAttachment(bool hide)
	{
		bool is_closed;
		bool is_ruined;
		string component;
		string zone;
		string camo_off;
		string camo_on;
		ToggleAnimations toggle;
		
		for (int i = 0; i < m_ToggleAnimations.Count(); i++)
		{
			toggle = m_ToggleAnimations.GetKey(i);
			camo_off = "Camo" + toggle.GetToggleOff().Substring(4,2);
			camo_on = "Camo" + toggle.GetToggleOn().Substring(4,2);
			
			if (hide)
			{
				SetAnimationPhase( camo_off, hide );
				SetAnimationPhase( camo_on, hide );
			}
			else
			{
				component = toggle.GetToggleOff(); //either one works
				component.ToLower();
				DamageSystem.GetDamageZoneFromComponentName(this,component,zone);
				is_ruined = GetHealthLevel(zone) == GameConstants.STATE_RUINED;
				is_closed = m_OpeningMask & toggle.GetOpeningBit();
				
				if (is_closed)
				{
					SetAnimationPhase( camo_on, is_ruined );
				}
				else
				{
					SetAnimationPhase( camo_off, is_ruined );
				}
			}
		}
		
		SetAnimationPhase( "Camo_Roof", hide );
	}
	
	override void AnimateCamonetToggle(ToggleAnimations toggle)
	{
		if (!m_CamoNet)
			return;
		
		bool is_closed = m_OpeningMask & toggle.GetOpeningBit();
		string camo_off = "Camo" + toggle.GetToggleOff().Substring(4,2);
		string camo_on = "Camo" + toggle.GetToggleOn().Substring(4,2);
		
		SetAnimationPhase( camo_off, is_closed );
		SetAnimationPhase( camo_on, !is_closed );
	}
	
	//on part destroy
	override void AnimateCamonetByOpeningSelection(string opening_selection)
	{
		string camo = "Camo" + opening_selection.Substring(4,2);
		SetAnimationPhase(camo,1);
	}
};

class PartyTent_Blue extends PartyTent {};
class PartyTent_Brown extends PartyTent {};
class PartyTent_Lunapark extends PartyTent {};

//placing classes
class PartyTentPlacing extends PartyTent {};
class PartyTent_BluePlacing extends PartyTentPlacing {};
class PartyTent_BrownPlacing extends PartyTentPlacing {};
class PartyTent_LunaparkPlacing extends PartyTentPlacing {};
