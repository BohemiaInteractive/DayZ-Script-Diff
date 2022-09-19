class ActionDefibrilateBase: ActionContinuousBase
{	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Defibrillator defib;
		Class.CastTo(defib, item);
		
		return CanDefibrillate(defib);
	}

	bool CanDefibrillate(Defibrillator defib)
	{
		if ( defib.IsCharged() ) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	void DefibrillateClient(PlayerBase player, Defibrillator defib)
	{
		defib.DischargeClient(player);
	}
	
	void DefibrillateServer(PlayerBase player, Defibrillator defib)
	{	
		if ( defib.IsCharged() ) 
		{
			defib.DischargeServer(player);
			
			
			float regain_energy = player.GetSoftSkillsManager().SubtractSpecialtyBonus( defib.GetEnergyNeededToCharge(), this.GetSpecialtyWeight() );
			regain_energy = defib.GetEnergyNeededToCharge() - regain_energy;
			
			ItemBase battery;
			
			if (Class.CastTo(battery, defib.GetCompEM().GetEnergySource()))
			{
				battery.GetCompEM().AddEnergy( regain_energy );
			}
			else
			{
				DPrint("ERROR! Defibrillator has no battery! Defibrillator softskill bonus can't be applied!");
			}
		}		
		player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};