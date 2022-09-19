class IconsContainer: Container
{
	protected ref map<int, ref Icon> m_EntitiesMap = new map<int, ref Icon>;

	void IconsContainer( LayoutHolder parent )
	{
	}

	override void UnfocusAll()
	{
		for ( int i = 0; i < Count(); i++ )
		{
			for ( int j = 0; j < ITEMS_IN_ROW; j++ )
			{
				Get( i ).GetMainWidget().FindAnyWidget( "Cursor" + j ).Show( false );
			}
		}
		
		
		for ( i = 0; i < m_EntitiesMap.Count(); ++i )
		{
			m_EntitiesMap.GetElement(i).SetActive(false);
		}
	}
	
	int GetItemCount()
	{
		return m_EntitiesMap.Count();
	}
	
	void AddItem( Icon icon )
	{	
		if( !m_EntitiesMap.Contains( icon.GetObject().GetID() ) )
		{
			m_EntitiesMap.Insert( icon.GetObject().GetID(), icon);
		}
	}
	
	Icon GetIcon( int entity_id )
	{
		return m_EntitiesMap.Get( entity_id  );
	}
	
	Icon GetIconByIndex( int index )
	{
		if( index < m_EntitiesMap.Count() && index > -1 )
			return m_EntitiesMap.GetElement( index  );
		return null;
	}

	void RemoveItem( Icon icon )
	{
		if( icon )
		{
			Icon icon_copy = icon; // for some reason garbage collector collects icon too soon, so copy has to be made
			if( icon.GetObject() )
				m_EntitiesMap.Remove( icon.GetObject().GetID() );
			else
			{
				int ix = m_EntitiesMap.GetKeyByValue( icon );
				if( m_EntitiesMap.Contains( ix ) )
					m_EntitiesMap.Remove( ix );
				else
					delete icon;
			}
		}
	}

	void RemoveItem( int entity )
	{
		m_EntitiesMap.Remove( entity );
	}
	
	bool ContainsEntity( EntityAI entity )
	{
		return m_EntitiesMap.Contains( entity.GetID() );
	}
	
	void UpdateItemsTemperature()
	{
		if( !ItemManager.GetInstance().IsDragging() )
		{
			for( int i = 0; i < m_EntitiesMap.Count(); i++ )
			{
				Icon icon = m_EntitiesMap.GetElement(i);
				icon.SetTemperature();
			}			
		}
	}

	override void Refresh()
	{
		super.Refresh();

		for( int i = 0; i < m_EntitiesMap.Count(); i++ )
		{
			m_EntitiesMap.GetElement(i).Refresh();
		}
	}
}
