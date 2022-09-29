class Paper extends ItemBase
{
	protected ref WrittenNoteData m_NoteContents;
	
	void Paper()
	{
		m_NoteContents = new WrittenNoteData(this);
	}
	
	void ~Paper() {}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (version >= 108 && !ctx.Read(m_NoteContents))
			return false;
				
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_NoteContents);		
	}
	
	override WrittenNoteData GetWrittenNoteData()
	{
		return m_NoteContents;
	}
	
	//================================================================
	// IGNITION ACTION
	//================================================================
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	override bool CanBeIgnitedBy(EntityAI igniter = null)
	{
		return !GetHierarchyParent();
	}
	
	override bool CanIgniteItem(EntityAI ignite_target = null)
	{
		return false;
	}
	
	override void OnIgnitedTarget(EntityAI ignited_item) {}
	
	override void OnIgnitedThis(EntityAI fire_source)
	{
		Fireplace.IgniteEntityAsFireplace(this, fire_source);
	}

	override bool IsThisIgnitionSuccessful(EntityAI item_source = null)
	{
		return Fireplace.CanIgniteEntityAsFireplace(this);
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionCreateIndoorFireplace);
		AddAction(ActionCreateIndoorOven);
		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
}
