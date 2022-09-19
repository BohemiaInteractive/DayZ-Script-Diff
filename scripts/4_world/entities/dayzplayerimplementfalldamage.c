class DayZPlayerImplementFallDamage
{
	const string 	FD_AMMO					= "FallDamage";	//!< ammo type used for damaging
	const float		FD_DMG_FROM_HEIGHT		= 2.5;			//!< damage will not be taken into account bellow this HeightToDamage
	const float		FD_MAX_DMG_AT_HEIGHT	= 15;			//!< height where player gets 100% damage
	const float		FD_MAX_HEIGHT_LEG_BREAK = 8;			//!< height where legs break most of the time
	
	DayZPlayer 		m_Player;

	void DayZPlayerImplementFallDamage(DayZPlayer pPlayer)
	{
		m_Player 	= pPlayer;
	}

	//! damage coef calculated from height
	float DamageCoef(float pHeight)
	{
		if (pHeight >= FD_MAX_DMG_AT_HEIGHT)
			return 100;

		return (pHeight - FD_DMG_FROM_HEIGHT);
	}

	//! handle fall damage
	void HandleFallDamage(float pHeight)
	{
		if (GetGame().IsServer())
		{
			//! no dmg height
			if (pHeight <= FD_DMG_FROM_HEIGHT)
				return;

			vector posMS = m_Player.WorldToModel(m_Player.GetPosition());
			
			float normalizedHeight = MiscGameplayFunctions.Normalize(pHeight, FD_MAX_HEIGHT_LEG_BREAK); //Normalize the height player fell from
			float dmg; //Damage dealt to leg
			
			//This uses a quadratic EaseInOut function ro deal damage to legs
			/*if (normalizedHeight < 0.5)
				dmg = 2*Math.Pow(normalizedHeight,2);
			else
				dmg = 1 - Math.Pow(-2*normalizedHeight+2,2)/2;*/
			
			dmg = Easing.EaseInOutQuart(normalizedHeight);
			
			//We multiply by 100 to "un normalize" value and multiply by a random factor to deal extra damage to legs
			dmg = (dmg * 100) * Math.RandomFloatInclusive(0.8, 1.5);
			
			if ( pHeight >= FD_MAX_HEIGHT_LEG_BREAK )
				dmg += 100; //Simply adding 100 should be enough to break legs 100% of time without calling GetHealth method
			
			PlayerBase.Cast(m_Player).DamageAllLegs(dmg);
			
			//Deal global damage when falling
			m_Player.ProcessDirectDamage( DT_CUSTOM, m_Player, "", FD_AMMO, posMS, DamageCoef(pHeight) );

			//! updates injury state of player immediately
			PlayerBase pb = PlayerBase.Cast(m_Player);
			if (pb) pb.ForceUpdateInjuredState();
		}
	}
};