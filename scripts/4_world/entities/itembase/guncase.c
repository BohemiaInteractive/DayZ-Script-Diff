class GunCase_ColorBase : Container_Base
{
	protected ref OpenableBehaviour m_Openable;
	
	override void InitItemVariables()
	{
		super.InitItemVariables();
		
		m_Openable = new OpenableBehaviour(true);
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_Openable.IsOpened());
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		bool opened;
		if (version >= 139 && ctx.Read(opened))
		{
			if (opened)
				Open();
			else
				Close();
		}
		
		return true;
	}
	
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent))
			return false;

		if (parent && parent.IsKindOf("GunCase_ColorBase"))
			return false;

		return true;
	}

	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (GetHealthLevel() == GameConstants.STATE_RUINED)
			return false;

		if (!IsOpen())
			return false;

		return super.CanReceiveItemIntoCargo(item);
	}
	
	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}
	
	override void Open()
	{
		m_Openable.Open();
		SetSynchDirty();
	}
	
	override void Close()
	{
		m_Openable.Close();
		SetSynchDirty();
	}
}
class GunCase_Black : GunCase_ColorBase {}
class GunCase_Orange : GunCase_ColorBase {}
class GunCase_White : GunCase_ColorBase {}
