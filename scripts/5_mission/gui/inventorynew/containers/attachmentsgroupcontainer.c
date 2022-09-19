class AttachmentsGroupContainer: Container
{
	ref Header m_Header;
	
	override void SetLayoutName()
	{
		m_LayoutName = WidgetLayoutName.AttachmentsGroupContainer;
	}
	
	override void SetHeader(Header header)
	{
		m_Header = header;
		m_Header.GetMainWidget().SetFlags( WidgetFlags.IGNOREPOINTER );
	}
	
	override Header GetHeader()
	{
		return m_Header;
	}
	
	void AttachmentsGroupContainer(LayoutHolder parent) 
	{
		m_FocusedColumn = 0;
	}
	
	override void SetActive(bool active)
	{
		super.SetActive(active);
		if (!active)
		{
			m_FocusedColumn = 0;
		}
	}
	
	override void SetDefaultFocus(bool while_micromanagment_mode = false)
	{
		super.SetDefaultFocus();
		m_FocusedColumn = 0;
	}
	
	SlotsIcon GetSlotsIcon(int row, int column, bool include_invisible = true )
	{
		SlotsContainer c;
		if (include_invisible)
		{
			c = SlotsContainer.Cast(m_Body[row]);	
		}
		else
		{
			c = SlotsContainer.Cast(m_OpenedContainers[row]);
		}
		return c.GetSlotIcon(column);
	}
	
	int GetRowCount(bool include_invisible = true)
	{
		if (include_invisible)
		{
			return m_Body.Count();
		}
		else
		{
			return m_OpenedContainers.Count();
		}
	}
	
	int GetColumnCountForRow(int row, bool include_invisible = true)
	{
		Container c;
		if (include_invisible)
		{
			c = Container.Cast(m_Body[row]);
		}
		else
		{
			c = Container.Cast(m_OpenedContainers[row]);
		}
		 return c.GetColumnCount();
	}
	
	override void SetNextActive()
	{
		HideOwnedTooltip();
		int visible_focus = 0;

		SlotsContainer active;
		if (m_OpenedContainers.Count())
		{
			active = SlotsContainer.Cast(m_OpenedContainers[m_ActiveIndex]);
		}		
		
		if (active && active.IsActive())
		{
			visible_focus = active.GetVisibleFocus();
			active.SetNextActive();
		}
		if (!active || !active.IsActive())
		{
			SlotsContainer next;
			if (!IsLastContainerFocused())
			{
				m_ActiveIndex++;
				
				//int visible_focus = active.GetVisibleFocus();
				
				next = SlotsContainer.Cast(m_OpenedContainers[m_ActiveIndex]);
				next.SetActive(true);
				next.SetVisibleFocus(visible_focus);
			} 
			else if (Container.Cast( GetParent() ))
			{
				SetActive(false);
			}
			else
			{
				SetActive(false);
				SetFirstActive();
			}
		}
	}
	
	override void SetPreviousActive(bool force = false)
	{
		HideOwnedTooltip();
		int visible_focus = 0;
		
		SlotsContainer active;
		if (m_OpenedContainers.Count())
		{
			active = SlotsContainer.Cast(m_OpenedContainers[m_ActiveIndex]);
		}
		
		if (active && active.IsActive())
		{
			visible_focus = active.GetVisibleFocus();
			active.SetPreviousActive();
		}
		
		if (!active || !active.IsActive())
		{
			SlotsContainer prev;
			if (!IsFirstContainerFocused())
			{
				m_ActiveIndex--;
				
				prev = SlotsContainer.Cast(m_OpenedContainers[m_ActiveIndex]);
				prev.SetActive(true);
				prev.SetVisibleFocus(visible_focus);
			} 
			else if (Container.Cast( GetParent() ))
			{
				SetActive(false);
			}
			else
			{
				SetActive(false);
				SetLastActive();
			}			
		}
	}
}
