
#ifdef DIAG_DEVELOPER
class ActionDismantlePartInstantDebug : ActionInstantBase
{	
	void ActionDismantlePartInstantDebug()
	{
		m_Text = "dismantle part debug";
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone();
		m_ConditionTarget = new CCTCursor();
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData cad = player.GetConstructionActionData();
		if (cad.GetFastConstructionMode() != EConstructionDebugModes.INSTANT)
			return false;
		
		EntityAI targetEntity;
		if (Class.CastTo(targetEntity,target.GetObject()) && targetEntity.CanUseConstruction())
		{
			ConstructionBase construction = ConstructionBase.Cast(targetEntity.GetConstructionBasic());
			string partName;
			ConstructionPart constructionPart;
			
			array<string> compNames = new array<string>();
			targetEntity.GetActionComponentNameList(target.GetComponentIndex(), compNames);
			int count = compNames.Count();
			for (int i = 0; i < count; ++i)
			{
				partName = compNames[i];
				constructionPart = construction.GetConstructionPart(partName);
				if (constructionPart)
					break;
			}
			
			if (constructionPart && constructionPart.IsBuilt())
			{
				cad.SetTargetPart(constructionPart);
				return true;
			}
		}
		
		return false;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ConstructionActionData cad = player.GetConstructionActionData();
		ConstructionPart part = cad.GetTargetPart();
		
		if (part)
		{
			m_Text = "dbgDismantle " + part.GetName() + " - INSTANT";
			
			#ifdef DIAG_DEVELOPER
			if (DiagMenu.GetBool(DiagMenuIDs.BASEBUILDING_FULLDEBUG))
			{
				m_Text = "dbgDismantle | part classname: " + part.GetPartName() + " - INSTANT";
			}
			#endif
		}
	}
	
	override void OnStartServer(ActionData action_data)
	{
		ConstructionActionData cad = action_data.m_Player.GetConstructionActionData();
		ConstructionPart part = cad.GetTargetPart();
		if (part)
		{
			EntityAI targetEntity = EntityAI.Cast(action_data.m_Target.GetObject());
			ConstructionBase construction = ConstructionBase.Cast(targetEntity.GetConstructionBasic());
			construction.DismantlePartServerEx(action_data.m_Player, part.GetPartName(), -1);
		}
	}
};
#endif