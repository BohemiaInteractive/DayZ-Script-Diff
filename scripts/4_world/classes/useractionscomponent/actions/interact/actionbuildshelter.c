class ActionBuildShelter: ActionContinuousBase
{
	string 						m_SoundsetBuildStart;
	string 						m_SoundsetBuildLoop;
	string 						m_SoundsetBuildFinish;
	ref protected EffectSound 	m_BuildLoopSound;
	
	void ActionBuildShelter()
	{
		m_CallbackClass = ActionBuildPartCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DEPLOY_2HD;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#build_shelter_leather";
	}
	
	void ~ActionBuildShelter()
	{
		DestroyActionLoopSound();
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor;
	}
	
	override string GetText()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( player )
		{
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			ConstructionPart constrution_part = construction_action_data.GetBuildPartNoToolAtIndex(m_VariantID);
			
			if ( constrution_part )
			{
				string ret = "";
				switch (constrution_part.GetName())
				{
					case "leather":
						ret = "#build_shelter_leather";
					break;
					
					case "fabric":
						ret = "#build_shelter_fabric";
					break;
					
					case "stick":
						ret = "#build_shelter_stick";
					break;
				}
			}
		}
		
		return ret;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
			ConstructionActionData construction_action_data = player.GetConstructionActionData();
			ConstructionPart constrution_part = construction_action_data.GetBuildPartNoToolAtIndex(m_VariantID);
			
			if ( constrution_part )
			{
				switch (constrution_part.GetName())
				{
					case "leather":
						m_Text = "#build_shelter_leather";
					break;
					
					case "fabric":
						m_Text = "#build_shelter_fabric";
					break;
					
					case "stick":
						m_Text = "#build_shelter_stick";
					break;
				}
			}
	}
	
	override bool CanBeUsedLeaning()
	{
		return false;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override bool HasProgress()
	{
		return true;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		if (target.GetObject() && !target.GetObject().CanUseConstructionBuild())
			return false;
		if ( player.IsPlacingLocal() || player.IsPlacingServer() )
			return false;
		
		if ( (!GetGame().IsDedicatedServer()) )
		{
			if ( MiscGameplayFunctions.ComplexBuildCollideCheckClient(player, target, item, m_VariantID ) )
			{
				return true;
			}
			return false;
		}
		return true;
	}
	
	//setup
	override bool SetupAction( PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{	
		if ( super.SetupAction( player, target, item, action_data, extra_data ) )
		{
			if ( !GetGame().IsDedicatedServer() )
			{
				ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
				BuildPartActionData.Cast(action_data).m_PartType = construction_action_data.GetBuildPartNoToolAtIndex(m_VariantID).GetPartName();
			}
			return true;
		}
		
		return false;
	}
	
	override ActionData CreateActionData()
	{
		BuildPartActionData action_data = new BuildPartActionData;
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		ctx.Write(BuildPartActionData.Cast(action_data).m_PartType);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		action_recive_data = new BuildPartActionReciveData;
		super.ReadFromContext(ctx, action_recive_data);
		
		string part_type;
		if ( ctx.Read(part_type) )
		{
			BuildPartActionReciveData.Cast( action_recive_data ).m_PartType = part_type;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		BuildPartActionData.Cast(action_data).m_PartType = BuildPartActionReciveData.Cast( action_recive_data ).m_PartType;
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		Construction construction = base_building.GetConstruction();
		string part_name = BuildPartActionData.Cast(action_data).m_PartType;
		CollisionCheckData check_data = new CollisionCheckData;
		
		check_data.m_PartName = part_name;
		check_data.m_AdditionalExcludes.Insert(action_data.m_Player);
		
		return !construction.IsCollidingEx( check_data ) && construction.CanBuildPart( part_name, action_data.m_MainItem, false );
	}
	
	override void OnStart( ActionData action_data )
	{
		super.OnStart(action_data);
		action_data.m_Player.TryHideItemInHands(true);
	}
	
	override void OnStartClient( ActionData action_data )
	{
		super.OnStartClient(action_data);
		
		DetermineConstructionSounds(action_data);
		PlayActionStartSound(action_data);
	}
	
	override void OnStartAnimationLoopClient( ActionData action_data )
	{
		super.OnStartAnimationLoopClient(action_data);
		
		PlayActionLoopSound(action_data);
	}
	
	override void OnEnd( ActionData action_data )
	{
		super.OnEnd(action_data);
		action_data.m_Player.TryHideItemInHands(false);
	}
	
	override void OnEndClient( ActionData action_data )
	{
		super.OnEndClient(action_data);
		
		StopActionLoopSound();
		//DestroyActionLoopSound();
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( action_data.m_Target.GetObject() );
		Construction construction = base_building.GetConstruction();
		
		string part_name = BuildPartActionData.Cast(action_data).m_PartType;
		
		if ( !construction.IsColliding( part_name ) && construction.CanBuildPart( part_name, action_data.m_MainItem, false ) )
		{
			//build
			construction.BuildPartServer( action_data.m_Player, part_name, AT_BUILD_PART );
		}
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		super.OnFinishProgressClient(action_data);
		
		PlayActionFinishSound(action_data);
	}
	
	void DetermineConstructionSounds( ActionData action_data )
	{
		ConstructionActionData construction_action_data = action_data.m_Player.GetConstructionActionData();
		ConstructionPart constrution_part = construction_action_data.GetBuildPartNoToolAtIndex(m_VariantID);;
		if (constrution_part)
		{
			switch (constrution_part.GetName())
			{
				case "leather":
					m_SoundsetBuildStart = "Shelter_Leather_Build_Start_SoundSet";
					m_SoundsetBuildLoop = "Shelter_Leather_Build_Loop_SoundSet";
					m_SoundsetBuildFinish = "Shelter_Leather_Build_Finish_SoundSet";
				break;
				
				case "fabric":
					m_SoundsetBuildStart = "Shelter_Fabric_Build_Start_SoundSet";
					m_SoundsetBuildLoop = "Shelter_Fabric_Build_Loop_SoundSet";
					m_SoundsetBuildFinish = "Shelter_Fabric_Build_Finish_SoundSet";
				break;
				
				case "stick":
					m_SoundsetBuildStart = "Shelter_Wooden_Stick_Build_Start_SoundSet";
					m_SoundsetBuildLoop = "Shelter_Wooden_Stick_Build_Loop_SoundSet";
					m_SoundsetBuildFinish = "Shelter_Wooden_Stick_Build_Finish_SoundSet";
				break;
				
				default:
					m_SoundsetBuildStart = "";
					m_SoundsetBuildLoop = "";
					m_SoundsetBuildFinish = "";
				break;
			}
		}
	}
	
	void PlayActionStartSound( ActionData action_data )
	{
		EffectSound sound =	SEffectManager.PlaySound(m_SoundsetBuildStart, action_data.m_Target.GetObject().GetPosition());
		sound.SetAutodestroy( true );
	}
	
	void PlayActionLoopSound( ActionData action_data )
	{
		if ( !m_BuildLoopSound || !m_BuildLoopSound.IsSoundPlaying() )
		{
			m_BuildLoopSound = SEffectManager.PlaySound( m_SoundsetBuildLoop, action_data.m_Target.GetObject().GetPosition(), 0, 0, true );
		}
	}
	
	void StopActionLoopSound()
	{
		if (m_BuildLoopSound)
		{
			m_BuildLoopSound.SetSoundFadeOut(0.5);
			m_BuildLoopSound.SoundStop();
		}
	}
	
	void DestroyActionLoopSound()
	{
		SEffectManager.DestroyEffect( m_BuildLoopSound );
	}
	
	void PlayActionFinishSound( ActionData action_data )
	{
		EffectSound sound =	SEffectManager.PlaySound(m_SoundsetBuildFinish, action_data.m_Target.GetObject().GetPosition());
		sound.SetAutodestroy( true );
	}
}