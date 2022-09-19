class MouthRag extends Clothing
{
	bool m_IncomingLambdaChange;
	
	void MouthRag()
	{
		m_IncomingLambdaChange = false;
	}

	override bool CanDetachAttachment(EntityAI parent)
	{
		return m_IncomingLambdaChange;
	}

	override bool IsObstructingVoice()
	{
		return true;
	}
	
	override int GetVoiceEffect()
	{
		return VoiceEffectObstruction;
	}
	
	void SetIncomingLambaBool(bool state)
	{
		m_IncomingLambdaChange = state;
	}
	
	bool GetIncomingLambdaBool()
	{
		return m_IncomingLambdaChange;
	}
};