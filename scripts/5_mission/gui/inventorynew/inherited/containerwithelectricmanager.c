class ContainerWithElectricManager: ContainerWithCargoAndAttachments
{
	override void UpdateInterval()
	{
		super.UpdateInterval();
		
		if ( m_Entity )
			ElectricityIcon();
	}
	
	void ElectricityIcon()
	{
		if ( GetRootWidget().FindAnyWidget( "electricity" ) )
		{
			bool show_electricity_icon = m_Entity.GetCompEM().HasElectricityIcon();
			GetRootWidget().FindAnyWidget( "electricity" ).Show( show_electricity_icon );
		}
		
		if ( GetRootWidget().FindAnyWidget( "electricity_can_work" ) )
		{
			bool show_electricity_can_work_icon = m_Entity.GetCompEM().CanWork() && !m_Entity.IsRuined();
			GetRootWidget().FindAnyWidget( "electricity_can_work" ).Show( show_electricity_can_work_icon );
		}
	}

	override void SetEntity( EntityAI entity, bool immedUpdate = true )
	{
		super.SetEntity( entity, immedUpdate );
		ElectricityIcon();
	}
}
