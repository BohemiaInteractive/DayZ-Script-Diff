class HescoBox extends Inventory_Base
{
	static const int FOLDED            = 0;
	static const int UNFOLDED 		   = 1;
	static const int FILLED 		   = 2;
	static const int PERCENTUAL_DAMAGE = 1;
	
	ref Timer 						m_Timer;

	protected int m_State;
	
	void HescoBox()
	{
		m_State = FOLDED;

		//synchronized variables
		RegisterNetSyncVariableInt( "m_State", FOLDED, FILLED );
	}
	
	override bool HasProxyParts()
	{
		return true;
	}
	
	override bool CanPutIntoHands( EntityAI parent )
	{
		if( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		return CanBeManipulated();
	}
		
	void Synchronize()
	{
		SetSynchDirty();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		//refresh visuals
		RefreshVisuals();
	}
	
	void RefreshVisuals()
	{
	}

	int GetState()
	{
		return m_State;
	}
	
	void SetState( int state )
	{
		m_State = state;
	}

	bool CanBeFilledAtPosition( vector position )
	{
		string surface_type;
		g_Game.SurfaceGetType( position[0], position[2], surface_type );
		
		return g_Game.IsSurfaceDigable(surface_type);
	}

	bool CanBeManipulated()
	{
		if ( GetState() == FOLDED )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	void Fold()
	{
		this.ShowSelection( "inventory" );
		this.HideSelection( "placing" );
		this.HideSelection( "filled" );
		
		SetState( FOLDED );
		RefreshPhysics();
		
		if ( g_Game.IsServer() )
		{
			SetAllowDamage(true);
			Synchronize();
			float fold_damage = ( GetMaxHealth( "", "" ) / 100 ) * PERCENTUAL_DAMAGE;
			DecreaseHealth( "", "", fold_damage );
		}
	}

	void Unfold()
	{
		this.HideSelection( "inventory" );
		this.ShowSelection( "placing" );
		this.HideSelection( "filled" );
		
		SetState( UNFOLDED );
		RefreshPhysics();
		
		if ( g_Game.IsServer() )
		{
			SetAllowDamage(true);
			Synchronize();
			float unfold_damage = ( GetMaxHealth( "", "" ) / 100 ) * PERCENTUAL_DAMAGE;
			DecreaseHealth( "", "", unfold_damage );	
		}
	}

	override void EEItemLocationChanged (notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged (oldLoc, newLoc);
		
		//RefreshPhysics();
	}

	override void RefreshPhysics()
	{
		super.RefreshPhysics();
		
		if ( this  &&  !ToDelete() )
		{
			RemoveProxyPhysics( "inventory" );
			RemoveProxyPhysics( "placing" );
			RemoveProxyPhysics( "filled" );
			
			int state = GetState();
			
			switch (state)
			{
				case UNFOLDED:
					//ShowSelection( "placing" );
					AddProxyPhysics( "placing" ); 
					
				return;
				
				case FOLDED:
					AddProxyPhysics( "inventory" ); 
				return;
				
				case FILLED:
					AddProxyPhysics( "filled" ); 
				return;
			}
		}
	}

	void Fill()
	{
		this.HideSelection( "inventory" );
		this.HideSelection( "placing" );
		this.ShowSelection( "filled" );
		
		SetState( FILLED );
		RefreshPhysics();
		
		if ( g_Game.IsServer() )
		{
			Synchronize();
			DecreaseHealth( "", "", 5 ); //TODO Daniel implement soft skill bonus via useraction
			SetAllowDamage(false);
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		// Save state
		ctx.Write( m_State );
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		// Load folded/unfolded state
		int state = FOLDED;
		if ( !ctx.Read(state) )
			state = FOLDED;
		
		switch (state)
		{
			case FOLDED:
			{
				Fold();
				break;
			}
			case UNFOLDED:
			{
				Unfold();
				break;
			}
			case FILLED:
			{
				Fill();
				break;
			}
		}
		return true;
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		if ( g_Game.IsServer() )
		{
			Unfold();
		}
	}
		
	override bool IsDeployable()
	{
		return true;
	}

	override string GetDeploySoundset()
	{
		return "placeHescoBox_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "hescobox_deploy_SoundSet";
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionFoldObject);
		AddAction(ActionDeployObject);
	}
	
	//!DEPRECATED
	protected ref EffectSound 	m_DeployLoopSound; //DEPRECATED in favor of m_DeployLoopSoundEx
	
	void PlayDeployLoopSound(); //!DEPRECATED
	void StopDeployLoopSound(); //!DEPRECATED
}
