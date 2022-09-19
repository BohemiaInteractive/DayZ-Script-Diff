class Chemlight_ColorBase : ItemBase
{
	string 					m_DefaultMaterial;
	string 					m_GlowMaterial;
	
	ChemlightLight 			m_Light;
	
	private int				m_Efficiency0To10; // Synchronized variable
	static private float 	m_EfficiencyDecayStart = 0.05; // At this % of maximum energy the output of the light starts to weaken.
	
	//! Returns efficiency. The value is synchronized from server to all clients and is accurate down to 0.1 units.
	float GetEfficiency0To1()
	{
		return m_Efficiency0To10 / 10;
	}
	
	//! Returns efficiency. The value is synchronized from server to all clients and is accurate down to 0.1 units.
	float GetEfficiencyDecayStart()
	{
		return m_EfficiencyDecayStart;
	}
	
	override void OnEnergyConsumed()
	{
		super.OnEnergyConsumed();
		
		if ( GetGame().IsServer() )
		{
			float energy_coef = GetCompEM().GetEnergy0To1();
			
			if ( energy_coef < m_EfficiencyDecayStart  &&  m_EfficiencyDecayStart > 0 )
			{
				m_Efficiency0To10 = Math.Round(  (energy_coef / m_EfficiencyDecayStart) * 10  );
				SetSynchDirty();
			}
		}
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel,newLevel,zone);
		
		SetObjectMaterial( 0, GetMaterialForDamageState(GetCompEM().IsWorking(),newLevel) );
	}
	
	void Chemlight_ColorBase()
	{
		//materials
		array<string> config_materials	= GetHiddenSelectionsMaterials();

		if (config_materials.Count() == 2)
		{
			m_DefaultMaterial = config_materials[0];
			m_GlowMaterial = config_materials[1];
		}
		else
		{
			string error = "Error! Item " + GetType() + " must have 2 entries in config array hiddenSelectionsMaterials[]. One for the default state, the other one for the glowing state. Currently it has " + config_materials.Count() + ".";
			Error(error);
		}
		
		m_Efficiency0To10 = 10;
		RegisterNetSyncVariableInt("m_Efficiency0To10");
	}
	
	void CreateLight()
	{
		SetObjectMaterial( 0, GetMaterialForDamageState(true) ); // must be server side!
		
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			m_Light = ChemlightLight.Cast( ScriptedLightBase.CreateLight( ChemlightLight, "0 0 0") );
			m_Light.AttachOnMemoryPoint(this, "light");
			
			string type = GetType();
			
			switch( type )
			{
				case "Chemlight_White": 
					m_Light.SetColorToWhite();
					break;
				case "Chemlight_Red": 
					m_Light.SetColorToRed();
					break;
				case "Chemlight_Green": 
					m_Light.SetColorToGreen();
					break;
				case "Chemlight_Blue": 
					m_Light.SetColorToBlue();
					break;
				case "Chemlight_Yellow": 
					m_Light.SetColorToYellow();
					break;
					
				default: { m_Light.SetColorToWhite(); };
			}
		}
	}
	
	override void OnWorkStart()
	{
		
	}
	
	// Inventory manipulation
	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);
		
		StandUp();
	}
	
	void StandUp()
	{
		if ( GetGame().IsServer()  &&  GetCompEM().IsWorking() )
		{
			vector ori_rotate = "0 0 0";
			SetOrientation(ori_rotate);
		}
	}
	
	override void OnWorkStop()
	{
		SetObjectMaterial( 0, GetMaterialForDamageState(false) );
		
		if (m_Light)
		{
			m_Light.FadeOut();
		}
		
		if ( GetGame().IsServer() )
		{
			//Safeguard if item is turned off by another event than running out of energy
			if (GetCompEM().GetEnergy() > 0)
				return;
			
			SetHealth(0);
		}
	}
	
	override void OnWork (float consumed_energy)
	{
		if (!m_Light)
			CreateLight();
		
		// Handle fade out of chemlight
		if (m_Light)
		{
			float efficiency = GetEfficiency0To1();
			
			if ( efficiency < 1 )
			{
				m_Light.SetIntensity( efficiency, GetCompEM().GetUpdateInterval() );
			}
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTurnOnWhileInHands);
	}
	
	string GetMaterialForDamageState(bool glowing,int healthLevel = -1)
	{
		int currentHealthLevel;
		int suffixIndex;
		string base;
		
		if (healthLevel == -1)
			currentHealthLevel = GetHealthLevel();
		else
			currentHealthLevel = healthLevel;
		
		if (glowing)
			base = m_GlowMaterial;
		else
			base = m_DefaultMaterial;
		
		suffixIndex = base.IndexOf(".rvmat");
		if (suffixIndex == -1)
		{
			Error("Error - no valid rvmat found for chemlight");
			return "";
		}
		base = base.Substring(0,suffixIndex);
		
		if (currentHealthLevel == GameConstants.STATE_BADLY_DAMAGED || currentHealthLevel == GameConstants.STATE_DAMAGED)
		{
			base = base + "_damage";
		}
		else if (currentHealthLevel == GameConstants.STATE_RUINED)
		{
			base = base + "_destruct";
		}
		
		return base + ".rvmat";
	}
};

class Chemlight_White: Chemlight_ColorBase {};
class Chemlight_Red: Chemlight_ColorBase {};
class Chemlight_Green: Chemlight_ColorBase {};
class Chemlight_Blue: Chemlight_ColorBase {};
class Chemlight_Yellow: Chemlight_ColorBase {};
