class CargoContainerRow: LayoutHolder
{
	protected int 				m_NumberRow;
	protected int				m_NumberColumnFocused;
	protected int				m_Width;
	protected int				m_MaxColumns;
	
	protected EntityAI			m_Entity;	
	
	protected CargoContainer	m_ParentContainer;
	protected ref SizeToChild	m_Resizer1;
	protected ref SizeToParent	m_Resizer2;
	
	void CargoContainerRow( LayoutHolder parent )
	{
		m_ParentContainer = CargoContainer.Cast( parent );
		#ifdef PLATFORM_CONSOLE
		m_MaxColumns = 5;
		#else
		m_MaxColumns = 10;
		#endif
		
		for ( int i = 0; i < m_MaxColumns; i++ )
		{
			WidgetEventHandler.GetInstance().RegisterOnDropReceived( GetMainWidget().FindAnyWidget( "Icon" + i ),  this, "Column" + i );
			WidgetEventHandler.GetInstance().RegisterOnDraggingOver( GetMainWidget().FindAnyWidget( "Icon" + i ),  this, "ColumnOnDraggingOver" + i );
		}
		
		m_RootWidget.GetScript( m_Resizer1 );
		m_RootWidget.FindAnyWidget( "SpacerTop" ).GetScript( m_Resizer2 );
	}
	
	void UnfocusAll()
	{
		for ( int i = 0; i < m_MaxColumns; i++ )
		{
			//GetMainWidget().FindAnyWidget( "Cursor" + i ).Show( false );
			#ifdef PLATFORM_WINDOWS
			#ifndef PLATFORM_CONSOLE
			#ifndef SERVER_FOR_CONSOLE
			CargoGrid grid = CargoGrid.Cast(m_Entity.GetInventory().GetCargo());
			if(grid)
			{
				
				EntityAI entity = grid.FindEntityInCargoOn( m_NumberRow, i );
				if( entity )
				{
					Icon icon = m_ParentContainer.GetIcon( entity );
					
					if( icon )
					{
						icon.SetActive( false );
					}
				}
				else
				{
					GetMainWidget().FindAnyWidget( "Cursor" + i ).Show( false );
				}
			}
			#endif
			#endif
			#endif
		}
		
		m_NumberColumnFocused = 0;
	}
	
	void SetFocus( int column )
	{	
		m_NumberColumnFocused = column;
		
		//GetMainWidget().FindAnyWidget( "Cursor" + column ).Show( true );
		
		// Show selector
		#ifdef PLATFORM_WINDOWS
		#ifndef PLATFORM_CONSOLE
		#ifndef SERVER_FOR_CONSOLE
		CargoGrid grid = CargoGrid.Cast(m_Entity.GetInventory().GetCargo());
		if(grid)
		{
			EntityAI focused_item_new = grid.FindEntityInCargoOn( m_NumberRow, m_NumberColumnFocused );
			if( focused_item_new )
			{
				Icon icon = m_ParentContainer.GetIcon( focused_item_new );
				
				if( icon )
				{
					icon.SetActive( true );
				}			
			}
			else
			{
				GetMainWidget().FindAnyWidget( "Cursor" + column ).Show( true );
			}
		}
		#endif
		#endif
		#endif
	}
	
	bool GetIconSize(int row, int column, out int width, out int height)
	{
		#ifdef PLATFORM_WINDOWS
		#ifndef PLATFORM_CONSOLE
		#ifndef SERVER_FOR_CONSOLE
		CargoGrid grid = CargoGrid.Cast(m_Entity.GetInventory().GetCargo());
		if(grid)
		{
			EntityAI focused_item_new = grid.FindEntityInCargoOn( row, column );
			if( focused_item_new )
			{
				Icon icon = m_ParentContainer.GetIcon( focused_item_new );
				
				if( icon )
				{
					width = icon.GetSizeX();
					height = icon.GetSizeY();
					
					return true;
				}
			}
		}
		#endif
		#endif
		#endif
		
		return false;
	}
	
	int GetFocusedColumn()
	{
		return m_NumberColumnFocused;
	}

	override void SetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
			m_LayoutName = WidgetLayoutName.CargoContainerRowXbox;
		#else
			switch( InventoryMenu.GetWidthType() )
			{
				case ScreenWidthType.NARROW:
				{
					m_LayoutName = WidgetLayoutName.CargoContainerRowNarrow;
					break;
				}
				case ScreenWidthType.MEDIUM:
				{
					m_LayoutName = WidgetLayoutName.CargoContainerRowMedium;
					break;
				}
				case ScreenWidthType.WIDE:
				{
					m_LayoutName = WidgetLayoutName.CargoContainerRowWide;
					
					break;
				}
			}
		#endif
		
	}
	
	void SetEntity( EntityAI entity )
	{
		m_Entity = entity;
	}
	
	void ColumnOnDraggingOver0( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 0, null );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 0, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 0, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 0, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver1( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 1, null );
		}		
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 1, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 1, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 1, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver2( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 2, null );
		}		
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 2, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 2, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 2, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver3( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 3, null );
		}		
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 3, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 3, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 3, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver4( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 4, null );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 4, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 4, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 4, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver5( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 5, null );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 5, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 5, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 5, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver6( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 6, null );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 6, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 6, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 6, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver7( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 7, null );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 7, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 7, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 7, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver8( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 8, null );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 8, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 8, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 8, null, m_ParentContainer );
		}
	}
	
	void ColumnOnDraggingOver9( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			ContainerWithCargo.Cast( m_Parent.m_Parent ).DraggingOverGrid( w, m_NumberRow, 9, null );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 9, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 9, null, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DraggingOverGrid( w, m_NumberRow, 9, null, m_ParentContainer );
		}
	}
	
	void Column0( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 0 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 0, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 0, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 0, m_ParentContainer );
		}
	}
	
	void Column1( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 1 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 1, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 1, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 1, m_ParentContainer );
		}
	}
	
	void Column2( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 2 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 2, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 2, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 2, m_ParentContainer );
		}
	}
	
	void Column3( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 3 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 3, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 3, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 3, m_ParentContainer );
		}
	}
	
	void Column4( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 4 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 4, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 4, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 4, m_ParentContainer );
		}
	}
	
	void Column5( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 5 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 5, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 5, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 5, m_ParentContainer );
		}
	}
	
	void Column6( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 6 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 6, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 6, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 6, m_ParentContainer );
		}
	}
	
	void Column7( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 7 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 7, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 7, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 7, m_ParentContainer );
		}
	}
	
	void Column8( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 8 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 8, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 8, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 8, m_ParentContainer );
		}
	}
	
	void Column9( Widget w )
	{
		if( m_Parent.m_Parent.IsInherited( ContainerWithCargo ) )
		{
			( ContainerWithCargo.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 9 );
		}
		else if( m_Parent.m_Parent.IsInherited( ContainerWithCargoAndAttachments ) )
		{
			( ContainerWithCargoAndAttachments.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 9, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( HandsContainer ) )
		{
			( HandsContainer.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 9, m_ParentContainer );
		}
		else if( m_Parent.m_Parent.IsInherited( AttachmentCategoriesRow ) )
		{
			( AttachmentCategoriesRow.Cast( m_Parent.m_Parent ) ).DropReceived( w, m_NumberRow, 9, m_ParentContainer );
		}
	}
	
	override void OnShow()
	{
		GetRootWidget().Show( true );
	}
	
	override void OnHide()
	{
		GetRootWidget().Show( false );
	}
	
	void SetNumber( int number )
	{
		m_NumberRow = number;
	}
	
	int GetWidth()
	{
		return m_Width;
	}

	void SetWidth( int width, bool immedUpdate = true )
	{
		m_Width = width;
		int i;
		
		for ( i = width; i < m_MaxColumns; i++ )
		{
			GetMainWidget().FindAnyWidget( "Icon" + i ).Show( false, false );
			GetMainWidget().FindAnyWidget( "Spacer" + i ).Show( false, false );
		}
		
		GetMainWidget().FindAnyWidget( "Spacer10" ).Show( m_Width != 0, false );
		
		if (immedUpdate)
		{			
			m_Resizer2.Refresh();
			GetMainWidget().Update();	
		}
		
		m_Resizer1.ResizeParentToChild();	
	}
}
