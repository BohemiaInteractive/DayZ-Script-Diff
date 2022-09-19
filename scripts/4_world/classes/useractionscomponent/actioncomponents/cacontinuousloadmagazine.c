class CAContinuousLoadMagazine : CAContinuousRepeat
{
	protected float 				m_TargetUnits;
	protected float 				m_SpentQuantity;
	protected float 				m_ItemQuantity;
	protected float					m_ItemMaxQuantity;
	protected float 				m_SourceQuantity;
	protected float 				m_QuantityFilledPerSecond;
	//protected ref Param1<float>		m_SpentUnits;
	
	void CAContinuousLoadMagazine( float time_to_complete_action )
	{
		m_DefaultTimeToComplete = time_to_complete_action;
	}
	
	override void Setup( ActionData action_data )
	{
		super.Setup(action_data);
		m_SpentQuantity = 0;
		if ( !m_SpentUnits )
		{ 
			m_SpentUnits = new Param1<float>(0);
		}
		else
		{	
			m_SpentUnits.param1 = 0;
		}
		
		Magazine itm;
		Class.CastTo(itm, action_data.m_MainItem);
		m_ItemQuantity = itm.GetAmmoCount();
		m_ItemMaxQuantity = itm.GetAmmoMax();
		Magazine trg;
		Class.CastTo(trg,  action_data.m_Target.GetObject() );
		m_SourceQuantity = trg.GetAmmoCount();
		m_TargetUnits = itm.GetAmmoMax();		
	}
};