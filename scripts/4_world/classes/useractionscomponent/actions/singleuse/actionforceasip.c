class ActionForceASip: ActionForceABite
{
	void ActionForceASip()
	{
		m_Sounds.Insert("DrinkBottle_0");
		m_Sounds.Insert("DrinkBottle_1");
		m_Text = "#give_a_sip";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}	
};