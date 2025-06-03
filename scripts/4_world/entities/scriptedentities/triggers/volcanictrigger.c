//! Volcanic area trigger
class VolcanicTrigger : EffectTrigger
{	
	static VolcanicTrigger m_SourceDamageInstance; // this is used only server side for logging damage source in admin log
	static const float HEAT_DAMAGE_MULTIPLIER = 5;
	static const float HEAT_DAMAGE_TICK_TIME = 2;
	
	protected ParticleSource m_VaporParticle; 
		
	override void EEInit()
	{
		if (GetGame().IsServer() && !m_SourceDamageInstance)
			m_SourceDamageInstance = this;
		
		super.EEInit();
				
		#ifndef SERVER
		SpawnVaporEffect();
		#endif
	}
	
	override string GetDisplayName()
	{
		return "#STR_volcanicarea";
	}
	
	override void EEDelete( EntityAI parent )
	{
		if (m_VaporParticle)
			m_VaporParticle.Stop();
		
		super.EEDelete(parent);
	}
		
	override void OnEnterServerEvent( TriggerInsider insider )
	{
		super.OnEnterServerEvent(insider);
				
		if (insider)
		{
			PlayerBase playerInsider = PlayerBase.Cast(insider.GetObject());
			if (playerInsider)
				playerInsider.SetInEffectAreaTypeFlag(EEffectAreaType.VOLCANIC);
		}
	}
	
	override void OnLeaveServerEvent( TriggerInsider insider )
	{
		super.OnLeaveServerEvent(insider);
				
		if (insider)
		{
			PlayerBase playerInsider = PlayerBase.Cast(insider.GetObject());
			if (playerInsider)
				playerInsider.UnsetInEffectAreaTypeFlag(EEffectAreaType.VOLCANIC);
		}
	}
	
	override void OnEnterClientEvent( TriggerInsider insider )
	{
		super.OnEnterClientEvent(insider);
		
		if (insider)
		{
			PlayerBase playerInsider = PlayerBase.Cast(insider.GetObject());
			if (playerInsider)
				playerInsider.SetInEffectAreaTypeFlag(EEffectAreaType.VOLCANIC);
		}
	}
	
	override void OnLeaveClientEvent( TriggerInsider insider )
	{
		super.OnLeaveClientEvent(insider);
		
		if (insider)
		{
			PlayerBase playerInsider = PlayerBase.Cast(insider.GetObject());
			if (playerInsider)
				playerInsider.UnsetInEffectAreaTypeFlag(EEffectAreaType.VOLCANIC);
		}
	}
			
	protected void SpawnVaporEffect()
	{
		vector pos = GetPosition();
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]) + 0.5;	// Snap to terrain surface
		
		m_VaporParticle = ParticleManager.GetInstance().PlayInWorld(ParticleList.VOLCANO, pos);
		
		int randSize = Math.RandomIntInclusive(-1,2);
		int randRate = Math.RandomIntInclusive(-1,2);

		m_VaporParticle.IncrementParticleParam(EmitorParam.SIZE, randSize);
		m_VaporParticle.IncrementParticleParam(EmitorParam.BIRTH_RATE, randRate);
	}
	
	// Server side effect application
	static void ApplyEffects(PlayerBase player)
	{
		player.ProcessDirectDamage(DamageType.CUSTOM, m_SourceDamageInstance, "", "HeatDamage", "0 0 0", HEAT_DAMAGE_MULTIPLIER);
	}
}