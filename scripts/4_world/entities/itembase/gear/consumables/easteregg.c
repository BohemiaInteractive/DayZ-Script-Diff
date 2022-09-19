enum eCaptureState
{
	CAPTURE = 0,
	RELEASE = 1,
	STASIS = 2,
	CAPTUREFX = 3,
	RELEASEFX = 4,
	
	//Keep this last value at the end, add any new states before
	END
}

class EasterEgg : Inventory_Base
{
	//Capture parameters
	private DayZCreatureAI 			m_StoredCreature = null;
	private string 					m_CreatureType;
	private int 					m_CreatureHash = 0; //Used for sync
	private int 					m_CaptureState = eCaptureState.STASIS;
	private const vector 			CAPTURE_VELOCITY = { 0, 0, 0 };
	
	//VFX
	protected Particle 				m_ParCapture;
	private   float 				m_ParScale = 1;
	private const float 			PARTICLE_SCALE_MULT = 0.1; //Used because we use DistanceSqr to get relevant scale
	
	//SFX
	protected EffectSound 			m_CaptureSound; //Egg SFX
	protected EffectSound 			m_CreatureSound; //Creature specific SFX
	protected bool					m_DangerSound = false; //Used to determine if release animal is dangerous and play sound accordingly
	
	protected ref map<int, string>	m_CreatureSoundMap; //Store all possible creature sound sets mapped to their respective hash
	protected int 					m_CaptureSoundHash; //Used to find capture sound set in map
	protected int 					m_ReleaseSoundHash; //Used to find release sound set in map
	
	
	void EasterEgg()
	{
		m_CreatureType = "";
		SetEventMask( EntityEvent.CONTACT | EntityEvent.TOUCH );
		SetFlags( EntityFlags.TRIGGER, false );
		RegisterNetSyncVariableInt( "m_CaptureState", 0, eCaptureState.END );
		RegisterNetSyncVariableInt( "m_CreatureHash", 0, 0 );
		RegisterNetSyncVariableFloat( "m_ParScale", 0, 0, 0.1 );
		RegisterNetSyncVariableBool( "m_DangerSound" );
		RegisterNetSyncVariableInt( "m_CaptureSoundHash", 0, 0 );
		RegisterNetSyncVariableInt( "m_ReleaseSoundHash", 0, 0 );
		
		RegisterSoundSetMap();
	}
	
	void ~EasterEgg()
	{
		if ( m_ParCapture )
			m_ParCapture.Stop();
	}
	
	// ------------------------------
	// CORE EXECUTION DEPENDING ON CURRENT STATE
	// ------------------------------
	void ContactEvent( IEntity other, vector pos )
	{
		switch ( m_CaptureState )
		{
			case eCaptureState.CAPTURE:
			DayZCreatureAI capAnimal = DayZCreatureAI.Cast( other );
			if ( capAnimal && capAnimal.IsAlive() )
			{
				if ( GetGame().IsServer() )
					Capture( capAnimal );
			}
			else
				m_CaptureState = eCaptureState.STASIS; //We did not capture anything, go back to stasis
		break;
		
			case eCaptureState.RELEASE:
			Release( pos );
			PlayVFX();
			PlaySFX( eCaptureState.RELEASE );
		break;
			
			case eCaptureState.CAPTUREFX:
			case eCaptureState.RELEASEFX:
			//Intermediate state to play FX on next client side contact event
			//Creates slight delay but saves network traffic
			if ( m_CreatureHash != 0 )
			{
				//Make sure to go back in stasis
				m_CaptureState = eCaptureState.STASIS;
				SetSynchDirty();
			}
		break;
			
			case eCaptureState.STASIS:
			//Do nothing here, feel free to add logic for fun fumble effects when nothing happens :) 
			
		break;
			
			default: //default in case state is somehow not initialized 
			
		break;
		}
	}
	
	//Used for capture
	override void EOnTouch( IEntity other, int extra )
	{
		ContactEvent( other, GetPosition() );
	}
	
	//Used for release
	override void EOnContact( IEntity other, Contact extra )
	{
		ContactEvent( other, extra.Position );
	}
	
	override void OnInventoryExit( Man player )
	{
		//Do not execute on simple drop as it may cause issues
		PlayerBase player_PB = PlayerBase.Cast( player );
		if ( player_PB && player_PB.GetThrowing().IsThrowingAnimationPlaying() )
		{
			if ( m_CreatureType != "" )
				m_CaptureState = eCaptureState.RELEASE;
			else
				m_CaptureState = eCaptureState.CAPTURE;
		}
		else
		{
			m_CaptureState = eCaptureState.STASIS;
		}
		
		//Make sure state is properly synchronized or VFX might bug out
		SetSynchDirty();
	}
	
	override void OnInventoryEnter( Man player )
	{
		//Make sure to stop particles once in inventory
		if ( GetGame().IsClient() && m_ParCapture )
		{
			m_ParCapture.Stop();
			m_ParCapture.Delete();
		}
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		//DestroyEg();
	}
	
	// ------------------------------
	// CAPTURE AND RELEASE LOGIC
	// ------------------------------
	private void Capture( DayZCreatureAI capAnimal )
	{
		if ( !IsAlive() )
		{
			if ( m_ParCapture )
				m_ParCapture.Delete();
			Delete();
			return;
		}
		
		m_StoredCreature = capAnimal;
		m_CreatureType = m_StoredCreature.GetType();
		m_CreatureHash = m_CreatureType.Hash();
		m_CaptureState = eCaptureState.CAPTUREFX;
		m_DangerSound = m_StoredCreature.IsDanger();
		m_CaptureSoundHash = m_StoredCreature.CaptureSound().Hash();
		m_ReleaseSoundHash = m_StoredCreature.ReleaseSound().Hash();
		
		//Resize particle upon capture as there is enough delay to be sure value is synced 
		ResizeParticle( capAnimal );
		
		SetSynchDirty();
	
		capAnimal.Delete();
		SetQuantity( GetQuantityMax() );
		SetVelocity( this, CAPTURE_VELOCITY );
	}
	
	private void Release( vector pos )
	{
		if ( GetGame().IsServer() )
		{
			m_CaptureState = eCaptureState.RELEASEFX;
			m_CreatureHash = 0;
			SetSynchDirty();
			
			GetGame().CreateObject( m_CreatureType, pos, false, true );
			m_CreatureType = "";
			
			DecreaseHealth( "", "", GetMaxHealth() * 0.4 );
			SetQuantity( GetQuantityMin(), false );
			SetVelocity( this, CAPTURE_VELOCITY );
			
			if ( !IsAlive() )
			{
				if ( m_ParCapture )
					m_ParCapture.Delete();
				Delete();
			}
		}
	}
	
	// ------------------------------
	// CAPTURE AND RELEASE EFFECTS
	// ------------------------------
	private void PlayVFX()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( !m_ParCapture && m_CaptureState != eCaptureState.STASIS )
			{
				//Ideally play a one time effect such as an explosion
				m_ParCapture = ParticleManager.GetInstance().PlayInWorld( ParticleList.EASTER_EGG_ACTIVATE, GetPosition() );
				
				//Resize, -1 signifies ALL emitors
				m_ParCapture.SetParameter( -1, EmitorParam.SIZE, m_ParScale );
				m_ParCapture.SetWiggle( 7, 0.3 );
			}
		}
	}
	
	private void ResizeParticle( DayZCreatureAI capAnimal )
	{
		//Determine particle scale depending on captured animal scale
		vector mins, maxs;
		capAnimal.GetWorldBounds( mins, maxs );
		m_ParScale = vector.DistanceSq( mins, maxs );
		
		//Multiply to rescale down as fed values can be really large
		m_ParScale *= PARTICLE_SCALE_MULT;
	}
	
	private void PlaySFX( int releaseCase = eCaptureState.CAPTURE )
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			string soundSet = "";
			if ( releaseCase == eCaptureState.CAPTURE )
			{
				PlaySoundSet( m_CaptureSound, "EasterEgg_Catch_SoundSet", 0, 0 );
				
				m_CreatureSoundMap.Find( m_CaptureSoundHash, soundSet );
				PlaySoundSet( m_CreatureSound, soundSet, 0, 0 );
			}
			else
			{
				if ( !m_DangerSound )
				{
					PlaySoundSet( m_CaptureSound, "EasterEgg_Spawn_SoundSet", 0, 0 );
					
					m_CreatureSoundMap.Find( m_ReleaseSoundHash, soundSet );
					PlaySoundSet( m_CreatureSound, soundSet, 0, 0 );
				}
				else
				{
					PlaySoundSet( m_CaptureSound, "EasterEgg_Spawn_Danger_SoundSet", 0, 0 );
					
					m_CreatureSoundMap.Find( m_ReleaseSoundHash, soundSet );
					PlaySoundSet( m_CreatureSound, soundSet, 0, 0 );
				}
			}
		}
	}
	
	
	override void OnVariablesSynchronized()
	{
		if ( m_CaptureState == eCaptureState.CAPTUREFX )
		{
			PlayVFX();
			PlaySFX();
		}
		else if ( m_CaptureState == eCaptureState.RELEASEFX )
		{
			PlayVFX();
			PlaySFX( eCaptureState.RELEASE );
		}
	}
	
	// ------------------------------
	// SOUNDSET MAP REGISTRATION
	// ------------------------------
	void RegisterSoundSetMap()
	{
		//Register all possible creature sounds in map with their respective hash
		string soundSet;
		m_CreatureSoundMap = new map<int, string>;
		
		//Cow sounds
		soundSet = "CattleMooA_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "CattleBellow_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		
		//Deer sounds
		soundSet = "DeerRoar_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "DeerBleat_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		
		//Goat sounds
		soundSet = "GoatBleat_A_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "GoatBleat_B_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		
		//Hare sounds
		soundSet = "HareChirp_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "HareSquawk_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );

		//Hen sounds
		soundSet = "HenCluck_X_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "HenScream_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );

		//Hog sounds
		soundSet = "HogGrunt_G_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "HogSqueal_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );

		//Sheep sounds
		soundSet = "SheepBleat_G_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "SheepBleat_E_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );

		//Wolf sounds
		soundSet = "WolfBark_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "WolfWhimper_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );

		//Zmb F sounds
		soundSet = "ZmbF_Normal_CallToArmsShort_Soundset";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "ZmbF_Normal_HeavyHit_Soundset";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );

		//Zmb M sounds
		soundSet = "ZmbM_Normal_CallToArmsShort_Soundset";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "ZmbM_Normal_HeavyHit_Soundset";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );

		//Bear sounds
		soundSet = "BearRoarShort_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
		soundSet = "BearSnarl_SoundSet";
		m_CreatureSoundMap.Insert( soundSet.Hash(), soundSet );
	}
	
	
	// ------------------------------
	// STORAGE SAVING AND LOADING 
	// ------------------------------
	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave( ctx );
		
		ctx.Write( m_CaptureState );
		ctx.Write( m_CreatureType );
		ctx.Write( m_ParScale );
		ctx.Write( m_DangerSound );
		ctx.Write( m_CaptureSoundHash );
		ctx.Write( m_ReleaseSoundHash );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		if ( !ctx.Read( m_CaptureState ) )
			return false;
		
		if ( !ctx.Read( m_CreatureType ) )
			return false;
		
		if ( !ctx.Read( m_ParScale ) )
			return false;
		
		if ( !ctx.Read( m_DangerSound ) )
			return false;
		
		if ( !ctx.Read( m_CaptureSoundHash ) )
			return false;
		
		if ( !ctx.Read( m_ReleaseSoundHash ) )
			return false;
		
		return true;
	}
	
	//Protection against dupers  during 1.12
	private void DestroyEg()
	{
		Delete();
	}
};
