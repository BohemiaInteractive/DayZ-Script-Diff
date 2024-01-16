class ContainerWithElectricManager : ContainerWithCargoAndAttachments
{
	override void UpdateInterval()
	{
		super.UpdateInterval();
		
		if (m_Entity)
			ElectricityIcon();
	}
	
	override void SetEntity(EntityAI entity, bool immedUpdate = true)
	{
		super.SetEntity(entity, immedUpdate);

		ElectricityIcon();
	}
	
	void ElectricityIcon()
	{
		if (GetRootWidget().FindAnyWidget("electricity"))
		{
			bool hasElectricityIcon = m_Entity.GetCompEM().HasElectricityIcon();
			GetRootWidget().FindAnyWidget("electricity").Show(hasElectricityIcon);
		}
		
		if (GetRootWidget().FindAnyWidget("electricity_can_work"))
		{
			bool electricityCanWorkIcon = m_Entity.GetCompEM().CanWork() && !m_Entity.IsRuined();
			GetRootWidget().FindAnyWidget("electricity_can_work").Show(electricityCanWorkIcon);
		}
	}
}
