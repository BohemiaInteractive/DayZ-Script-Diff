class AreaDamageComponentData
{
	string Hitzone = "";
	vector Modelpos = "0.5 0.5 0.5";
}

class AreaDamageComponent : AreaDamageEvents
{
	AreaDamageManager 		m_Parent;
	
	int 					m_DamageType;
	string					m_AmmoName;

	ref array<typename>		m_DamageableTypes
	
	void AreaDamageComponent(AreaDamageManager parent)
	{
		m_Parent = parent;
		
		m_DamageType 		= DT_CUSTOM;
		m_AmmoName			= "MeleeDamage";

		m_DamageableTypes	= new array<typename>;
		m_DamageableTypes.Insert(DayZPlayer);
	}
	
	override bool OnEvaluateDamage(TriggerInsider insider, float damageCoef)
	{
		// No need to damage if the coef is 0!
		if ( ShouldDamage(insider.GetObject()) && damageCoef > 0 )
			return EvaluateDamageInternal(insider, damageCoef);
		
		return false;
	}
	
	override void PreDamageActions()
	{
		m_Parent.PreDamageActions();
	}

	override void PostDamageActions()
	{
		m_Parent.PostDamageActions();
	}
	
	protected bool ShouldDamage(Object object)
	{
		return object && object.IsAlive() && object.IsAnyInherited( m_DamageableTypes );
	}
	
	protected AreaDamageComponentData GetAreaDamageComponentData(Object object)
	{
		return new AreaDamageComponentData;
	}
	
	private bool EvaluateDamageInternal(TriggerInsider insider, float damageCoef)
	{
		PreDamageActions();
		
		AreaDamageTriggerInsider dInsider;
		if ( CastTo( dInsider, insider ) )
			dInsider.lastDamaged = g_Game.GetTickTime();
		
		Object object = insider.GetObject();
		
		AreaDamageComponentData data = GetAreaDamageComponentData(object);
		object.ProcessDirectDamage(m_DamageType, m_Parent.GetParentObject(), data.Hitzone, m_AmmoName, data.Modelpos, damageCoef);
				
		PostDamageActions();
		
		return true;
	}
}