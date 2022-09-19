
enum EFireworksState
{
	DEFAULT,
	PLACED,
	IGNITED,
	FIRING,
	FINISHED
}

class FireworksBase: Inventory_Base
{
	protected  EFireworksState m_State;
	protected  EFireworksState m_StatePrev;
	protected ref Timer m_TimerEvent;
	protected int m_RandomSeed;

	void FireworksBase()
	{
		Init();
	}
	
	override void EEOnCECreate()
	{
		StandUp();
	}
	
	protected void Init();
	

	
	override protected void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
		AddAction(ActionLightItemOnFire);
	}
	
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	protected float GetMaxAllowedWetness()
	{
		return 0.1;
	}
	
	protected EFireworksState GetState()
	{
		return m_State;
	}
	
	protected void SetState(EFireworksState state)
	{
		if (state != m_StatePrev && GetGame().IsServer())
		{
			m_State = state;
			SetSynchDirty();
			OnStateChangedServer(state);
			m_StatePrev = state;
		}
	}
	
	protected void OnStateChangedServer(EFireworksState currentState);
	protected void OnStateChangedClient(EFireworksState currentState);

	
	override protected void OnInventoryEnter(Man player)
	{
		if (GetState() == EFireworksState.PLACED)
		{
			SetState(EFireworksState.DEFAULT);
		}
	}
	
	override protected void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);
		
		if (!IsBeingPlaced())
			StandUp();
	}
	
	protected void StandUp()
	{
		if (!IsRuined())
		{
			SetOrientation(vector.Zero);
		}
	}
	
	//! Executed on Server when some item ignited this one
	override protected void OnIgnitedThis( EntityAI fire_source)
	{
		SetState(EFireworksState.IGNITED);
	}
	
	// Checkes if Torch can be ignited
	override protected bool CanBeIgnitedBy(EntityAI igniter = NULL)
	{
		if (GetWet() >= GetMaxAllowedWetness())
		{
			return false;
		}
		
		if (MiscGameplayFunctions.IsUnderRoofEx(this, 60))
		{
			return false;
		}
		
		if ((GetState() <= EFireworksState.PLACED) && vector.Dot(vector.Up,GetDirectionUp()) > 0.95)
		{
			return true;	
		}
		return false;
	}
	
	//!Called periodically after the entity gets ignited
	protected void OnEventServer(int type);
	
	protected float GetEventDelay()
	{
		return 0;
	}
	
	override protected void OnVariablesSynchronized()
	{
		//Print("new state client: " + m_State);
		super.OnVariablesSynchronized();
		if (m_State != m_StatePrev)
		{
			OnStateChangedClient(m_State);
			m_StatePrev = m_State;
		}
	}
	
	#ifdef DEVELOPER
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "Ignite";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		if (button_index == 1)
		{
			OnIgnitedThis(null);
		}
	}
	#endif
}
