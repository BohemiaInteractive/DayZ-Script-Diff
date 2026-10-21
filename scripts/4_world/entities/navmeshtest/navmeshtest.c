// for testing NavmeshEntityType
class Land_NavmeshTest : Building
{
	ref array<ref Param2<string, bool>> m_Signals = new array<ref Param2<string, bool>>();
	
	void Land_NavmeshTest()
	{
	}
	
	void AddSignal(string name, bool flipped)
	{
		Param2<string, bool> signal = new Param2<string, bool>(name, flipped);
		m_Signals.Insert(signal);
	}
	
	bool IsSignalActive(int idx)
	{
		string signal = m_Signals[idx].param1;
		if (m_Signals[idx].param2)
		{
			return GetAnimationPhase(signal) < 0.001;
		}
		return GetAnimationPhase(signal) > 0;
	}
	
	void SetSignalActive(int idx, bool state)
	{
		string signal = m_Signals[idx].param1;
		if (state)
		{
			if (m_Signals[idx].param2)
			{
				SetAnimationPhase(signal, 0.0);
			}
			else
			{
				SetAnimationPhase(signal, 1.0);
			}
		}
		else
		{
			if (m_Signals[idx].param2)
			{
				SetAnimationPhase(signal, 1.0);
			}
			else
			{
				SetAnimationPhase(signal, 0.0);
			}
		}
	}
	
	void ToggleSignal(int idx)
	{
		bool isActive = IsSignalActive(idx);
		if (isActive)
		{
			SetSignalActive(idx, false);
		}
		else
		{
			SetSignalActive(idx, true);
		}
	}

	#ifdef DIAG_DEVELOPER
	override void GetDebugActions(out TSelectableActionInfoArrayEx outputList)
	{
		int i;
		int cnt;
		int val;
		string name;
		
		cnt = m_Signals.Count();
		
		for (i = 0; i < cnt; i++)
		{
			val = i + EActions.PLAYER_BOT_INTERNAL_START;
			
			name = m_Signals[i].param1;
			if (IsSignalActive(i))
			{
				name = name + " (active)";
			}
							
			outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, val, name, FadeColors.LIGHT_GREY));
		}

		outputList.Insert(new TSelectableActionInfoWithColor(SAT_DEBUG_ACTION, EActions.SEPARATOR, "___________________________", FadeColors.RED));
						
		super.GetDebugActions(outputList);
	}
	
	override bool OnAction(int action_id, Man player, ParamsReadContext ctx)
	{
		if (super.OnAction(action_id, player, ctx))
			return true;

		if (!GetGame().IsServer())
		{
			return false;
		}
		
		int idx = action_id - EActions.PLAYER_BOT_INTERNAL_START;
		if (idx >= 0 && idx < m_Signals.Count())
		{
			ToggleSignal(idx);
			return true;
		}
		
		return false;
	}
	#endif
};

// use case examples

class Land_NavmeshTest_A : Land_NavmeshTest
{
	void Land_NavmeshTest_A()
	{
		AddSignal("Wall_Hide", true);
	}
};

class Land_NavmeshTest_B : Land_NavmeshTest
{
	void Land_NavmeshTest_B()
	{
		AddSignal("Wall_Hide", true);
		AddSignal("Door_Hide", true);
		AddSignal("Door_Rotate", false);
	}
};

class Land_NavmeshTest_C : Land_NavmeshTest
{
	void Land_NavmeshTest_C()
	{
		AddSignal("Wall_Hide", true);
		AddSignal("Window_Hide", true);
	}
};

class Land_NavmeshTest_D : Land_NavmeshTest
{
	void Land_NavmeshTest_D()
	{
		AddSignal("Wall_Hide", true);
		AddSignal("Roof_Hide", true);
	}
};

class Land_NavmeshTest_E : Land_NavmeshTest
{
	void Land_NavmeshTest_E()
	{
		AddSignal("Wall_Hide", true);
		AddSignal("Door_Hide", true);
		AddSignal("Door_Rotate", false);
		AddSignal("Roof_Hide", true);
	}
};

class Land_NavmeshTest_F : Land_NavmeshTest
{
	void Land_NavmeshTest_F()
	{
		AddSignal("Wall_Hide", true);
		AddSignal("Window_Hide", true);
		AddSignal("Roof_Hide", true);
	}
};
