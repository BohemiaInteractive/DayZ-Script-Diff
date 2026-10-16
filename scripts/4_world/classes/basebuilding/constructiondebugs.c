#ifdef DIAG_DEVELOPER

class ConstructionDebug
{
	//Context: ConstructionBase.CanDismantlePart
	protected static string m_DismantlePartName;
	protected static bool m_DismantleConstructed;
	protected static bool m_DismantleHasDependents;
	protected static bool m_DismantleSupportFailed;

	//Context: ActionDismantlePart
	protected static string m_ActionDismantlePartName;
	protected static string m_ActionDismantleReason;

	//Context: ActionPartInfo
	protected static int m_TargetComponentIdx = -1;
	protected static ref map<int, ref ActionInfoDebugData> m_ActionInfoDebug = new map<int, ref ActionInfoDebugData>();
	
	//Context: Collision printout data
	protected static string m_CollidingPartName;
	protected static ref array<string> m_ValidCollisions = new array<string>();
	
	//Context: Insidebox printout data
	protected static string m_InsideboxPartName;
	
	static void ResetValues()
	{
		m_DismantlePartName = "";
		m_DismantleConstructed = false;
		m_DismantleHasDependents = false;
		m_DismantleSupportFailed = false;
		
		m_ActionDismantlePartName = "";
		m_ActionDismantleReason = "";
		
		m_TargetComponentIdx = -1;
		m_ActionInfoDebug.Clear();
		
		m_CollidingPartName = "";
		m_ValidCollisions.Clear();
		
		m_InsideboxPartName = "";
	}

	static void SetCanDismantlePartData(ConstructionDismantleDebugData data)
	{
		m_DismantlePartName = data.partName;
		m_DismantleConstructed = data.constructed;
		m_DismantleHasDependents = data.hasDependents;
		m_DismantleSupportFailed = !data.supportState;
	}
	
	static void ResetDismantlePartData()
	{
		m_DismantlePartName = "";
	}

	static void SetActionDismantleData(ActionDismantleDebugData data)
	{
		m_ActionDismantlePartName = data.partName;
		m_ActionDismantleReason = data.reason;
	}
	
	static void ResetActionDismantleData()
	{
		m_ActionDismantlePartName = "";
	}

	static void AddBuildPartInfoData(ActionInfoDebugData data)
	{
		UpdateTargetComponentIdx(data.componentIdx);
		
		m_ActionInfoDebug.Set(data.partName.Hash(), data);
	}
	
	static void RemoveBuildPartInfoData(string partName) //?
	{
		m_ActionInfoDebug.Remove(partName.Hash());
	}
	
	protected static void UpdateTargetComponentIdx(int idx)
	{
		if (m_TargetComponentIdx != idx)
		{
			m_ActionInfoDebug.Clear();
			m_TargetComponentIdx = idx;
		}
	}
	
	static void SetCollisionPrintoutData(string partName, notnull array<string> collisions)
	{
		m_CollidingPartName = partName;
		m_ValidCollisions.Copy(collisions);
	}
	
	static void ResetCollisionPrintoutData()
	{
		m_CollidingPartName = "";
		m_ValidCollisions.Clear();
	}
	
	static void SetInsideboxCheckDebugData(string partName)
	{
		m_InsideboxPartName = partName;
	}
	
	static void Update()
	{
		DrawCanDismantlePart();
		DrawActionDismantle();
		DrawBuildPartInfo();
		DrawCollisionPrintouts();
		DrawInsideboxPrintouts();
	}

	protected static void DrawCanDismantlePart()
	{
		if (m_DismantlePartName != "")
		{
			DbgUI.Begin("CanDismantlePart Generic Debug", 0, 0);
			{
				DbgUI.Text("cannot dismantle: " + m_DismantlePartName);
				if (!m_DismantleConstructed)
					DbgUI.Text("     not constructed");
				if (m_DismantleHasDependents)
					DbgUI.Text("     has dependent part/s");
				if (m_DismantleSupportFailed)
					DbgUI.Text("     support check failed");
			}
			DbgUI.End();
		}
	}

	protected static void DrawActionDismantle()
	{
		if (m_ActionDismantlePartName != "")
		{
			DbgUI.Begin("ActionDismantlePart Specific Debug", 0, 200);
			{
				DbgUI.Text("cannot dismantle: " + m_ActionDismantlePartName);
				DbgUI.Text("reason: " + m_ActionDismantleReason);
			}
			DbgUI.End();
		}
	}

	protected static void DrawBuildPartInfo()
	{
		DbgUI.Begin("ActionPartInfo Debug", 400, 0);
		{
			DbgUI.Text("target component idx: " + m_TargetComponentIdx);
			DbgUI.Spacer(10);
			
			foreach (int hash, ref ActionInfoDebugData data : m_ActionInfoDebug)
			{
				if (data.failureMask & EConstructionInfoCategories.PART_MISSING_REQUIREMENT)
					DbgUI.Text(data.partName + " - missing required part/s!");
				if (data.failureMask & EConstructionInfoCategories.PART_MISSING_SUPPORT)
					DbgUI.Text(data.partName + " - missing statics support/s!");
				if (data.failureMask & EConstructionInfoCategories.PART_CONFLICTED)
					DbgUI.Text(data.partName + " - conflicts with part: " + data.conflictingPart + "!");
				if (data.failureMask & EConstructionInfoCategories.MATERIALS_PRESENCE)
					DbgUI.Text(data.partName + " - missing material in slot: " + data.missingSlot + " (qty: " + data.missingQty + ")!");
				if (data.failureMask & EConstructionInfoCategories.MATERIALS_DAMAGE)
					DbgUI.Text(data.partName + " - material/s ruined!");
				if (data.failureMask & EConstructionInfoCategories.TOOL_PRESENCE)
					DbgUI.Text(data.partName + " - not buildable with: " + data.badToolType + "!");
				DbgUI.Spacer(10);
			}
		}
		DbgUI.End();
	}
	
	protected static void DrawCollisionPrintouts()
	{
		if (m_CollidingPartName != "")
		{
			DbgUI.Begin("IsCollidingEx Debug", 0, 300);
			{
				DbgUI.Text("part " + m_CollidingPartName + " blocked by:");
				foreach (string name : m_ValidCollisions)
				{
					DbgUI.Text("    " + name);
				}
			}
			DbgUI.End();
		}
	}
	
	protected static void DrawInsideboxPrintouts()
	{
		if (m_InsideboxPartName != "")
		{
			DbgUI.Begin("Insidebox Debug", 800, 0);
			{
				DbgUI.Text("insidebox check failed for part: " + m_InsideboxPartName);
			}
			DbgUI.End();
		}
	}
}

class ConstructionDismantleDebugData
{
	string partName;
	bool constructed;
	bool hasDependents;
	bool supportState;
}

class ActionDismantleDebugData
{
	string partName;
	string reason;
}

class ActionInfoDebugData
{
	string partName;
	int failureMask;
	int componentIdx; //target component idx
	string conflictingPart;
	string missingSlot;
	float missingQty;
	string badToolType;
}

//------------------------------

class CharacterStepsDebug
{
	protected static string m_BoneName;
	protected static string m_LastSurfaceType;
	
	static void Update()
	{
		DrawStepPrintouts();
	}
	
	static void SetStepData(string boneName, string lastSurfaceName)
	{
		m_BoneName = boneName;
		m_LastSurfaceType = lastSurfaceName;
	}
	
	protected static void DrawStepPrintouts()
	{
		if (m_BoneName != "")
		{
			DbgUI.Begin("PlayerSurface Debug", 10, 10);
			{
				DbgUI.Text("Leg: " + m_BoneName);
				DbgUI.Text("Last surface type: " + m_LastSurfaceType);
			}
			DbgUI.End();
		}
	}
}

#endif
