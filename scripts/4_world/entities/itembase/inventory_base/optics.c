class M68Optic : ItemOptics {};
class M4_T3NRDSOptic : ItemOptics {};
class FNP45_MRDSOptic : ItemOptics {};
class Crossbow_RedpointOptic : ItemOptics {};
class ReflexOptic : ItemOptics {};
class ACOGOptic : ItemOptics {};
class KashtanOptic : ItemOptics {};
class LongrangeOptic : ItemOptics {};
class PistolOptic : ItemOptics {};
class GrozaOptic : ItemOptics {};
class KobraOptic : ItemOptics {};
class ACOGOptic_6x : ItemOptics {};

class KazuarOptic: ItemOptics
{
	override void InitOpticMode() //TODO - decide whether to randomize on spawn and how to determine it (attachment etc.)
	{
		super.InitOpticMode();
		
		SetCurrentOpticMode(GameConstants.OPTICS_STATE_NIGHTVISION);
	}
	
	override int GetCurrentNVType()
	{
		if (IsWorking())
		{
			switch (m_CurrentOpticMode)
			{
				case GameConstants.OPTICS_STATE_DAY:
					return NVTypes.NV_OPTICS_KAZUAR_DAY;
				
				case GameConstants.OPTICS_STATE_NIGHTVISION:
					return NVTypes.NV_OPTICS_KAZUAR_NIGHT;
			}
			Error("Undefined optic mode of " + this);
			return NVTypes.NONE;
		}
		else
		{
			return NVTypes.NV_OPTICS_OFF;
		}
	}
	
	override void OnOpticModeChange()
	{
		super.OnOpticModeChange();
		
		UpdateSelectionVisibility();
	}
	
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide_cover_pilot");
	}
	
	override void UpdateSelectionVisibility()
	{
		super.UpdateSelectionVisibility();
		
		switch (GetCurrentOpticMode())
		{
			case GameConstants.OPTICS_STATE_NIGHTVISION:
				HideSelection("hide_cover");
				HideSelection("hide_cover_pilot");
			break;
			
			case GameConstants.OPTICS_STATE_DAY:
				ShowSelection("hide_cover");
				if ( !GetGame().IsDedicatedServer() && !IsInOptics() ) //quick sanity check, just in case
				{
					ShowSelection("hide_cover_pilot");
				}
			break;
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVMode);
	}
};

class StarlightOptic: ItemOptics
{
	override void InitOpticMode() //TODO - decide whether to randomize on spawn and how to determine it (attachment etc.)
	{
		super.InitOpticMode();
		
		SetCurrentOpticMode(GameConstants.OPTICS_STATE_NIGHTVISION);
	}
	
	override int GetCurrentNVType()
	{
		if (IsWorking())
		{
			switch (m_CurrentOpticMode)
			{
				case GameConstants.OPTICS_STATE_DAY:
					return NVTypes.NV_OPTICS_STARLIGHT_DAY;
				
				case GameConstants.OPTICS_STATE_NIGHTVISION:
					return NVTypes.NV_OPTICS_STARLIGHT_NIGHT;
			}
			Error("Undefined optic mode of " + this);
			return NVTypes.NONE;
		}
		else
		{
			return NVTypes.NV_OPTICS_OFF;
		}
	}
	
	override void OnOpticModeChange()
	{
		super.OnOpticModeChange();
		
		UpdateSelectionVisibility();
	}
	
	override void OnOpticEnter()
	{
		super.OnOpticEnter();
		
		HideSelection("hide_cover_pilot");
	}
	
	override void UpdateSelectionVisibility()
	{
		super.UpdateSelectionVisibility();
		
		switch (GetCurrentOpticMode())
		{
			case GameConstants.OPTICS_STATE_NIGHTVISION:
				HideSelection("hide_cover");
				HideSelection("hide_cover_pilot");	
			break;
			
			case GameConstants.OPTICS_STATE_DAY:
				ShowSelection("hide_cover");
				if ( !GetGame().IsDedicatedServer() && !IsInOptics() ) //quick sanity check, just in case
				{
					ShowSelection("hide_cover_pilot");
				}
			break;
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVMode);
	}
};
