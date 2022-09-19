class ActionVariantManager
{
	protected ref array<ref ActionBase> m_VariantActions;
	protected typename m_type;
	protected ref ScriptInvoker e_OnUpdate;
	
	void ActionVariantManager( typename type )
	{
		m_type = type;
		m_VariantActions = new array<ref ActionBase>;
		e_OnUpdate = new ScriptInvoker();
	}
	
	ScriptInvoker GetOnUpdateInvoker()
	{
		return e_OnUpdate;
	}
	
	void Clear()
	{
		m_VariantActions.Clear();
	}
	
	void SetActionVariantCount( int count )
	{
		if ( count < m_VariantActions.Count() )
		{
			m_VariantActions.Clear();
		}

		for (int i = m_VariantActions.Count(); i < count; i++ )
		{
			ActionBase action = ActionManagerBase.GetActionVariant( m_type );
			action.SetVariantID(i);
			m_VariantActions.Insert(action);
		}
		
	}
	
	int GetActions( out array<ref ActionBase> variants_action)
	{
		variants_action = m_VariantActions;
		return m_VariantActions.Count();
	}
	
	int GetActionsCount()
	{
		return m_VariantActions.Count();
	}
	
	void UpdateVariants( Object item, Object target, int componet_index )
	{
		e_OnUpdate.Invoke( item, target, componet_index );
	}
}