/**@class 	HandStartAction
 * @brief	simple class starting animation action specified by m_action and m_actionType
 */
class HandStartAction extends HandStateBase
{
	WeaponActions m_Action; /// action to be played
	int m_ActionType; /// specific action sub-type

	void HandStartAction (Man player = NULL, HandStateBase parent = NULL, WeaponActions action = WeaponActions.NONE, int actionType = -1)
	{
		m_Action = action;
		m_ActionType = actionType;
	}

	override void OnEntry (HandEventBase e)
	{
		super.OnEntry(e);

		if (e.m_Player)
		{
			HumanCommandWeapons hcw = e.m_Player.GetCommandModifier_Weapons();
			if (hcw)
			{
				if (m_ActionType == -1)
				{
					hcw.StartAction(-1, -1);
					if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("HCW: " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " playing A=-1 AT=-1 fini=" + hcw.IsActionFinished());
				}
				else
				{
					hcw.StartAction(m_Action, m_ActionType);
		
					if (hcw.GetRunningAction() == m_Action && hcw.GetRunningActionType() == m_ActionType)
					{
						if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("HCW: " + Object.GetDebugName(e.m_Player) + " STS = " + e.m_Player.GetSimulationTimeStamp() + " playing A=" + typename.EnumToString(WeaponActions, m_Action) + " AT=" + WeaponActionTypeToString(m_Action, m_ActionType) + " fini=" + hcw.IsActionFinished());
					}
					else
						Error("HCW: NOT playing A=" + typename.EnumToString(WeaponActions, m_Action) + " AT=" + WeaponActionTypeToString(m_Action, m_ActionType) + " fini=" + hcw.IsActionFinished());
				}
			}
			else
				if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("---: remote playing A=" + typename.EnumToString(WeaponActions, m_Action) + " AT=" + WeaponActionTypeToString(m_Action, m_ActionType));
		}
		else
		{
			if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("---: warning, no player wants to play A=" + typename.EnumToString(WeaponActions, m_Action) + " AT=" + WeaponActionTypeToString(m_Action, m_ActionType));
		}
	}
	override void OnExit (HandEventBase e)
	{
		super.OnExit(e);
	}
	
	override bool IsIdle () { return false; }
};



