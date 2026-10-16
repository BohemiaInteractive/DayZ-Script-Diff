class CombinationLock4 extends CombinationLock
{
	override void SetBaseLockValues()
	{
		//set lock init values
		m_LockDigits 			= 4;
		m_Combination		 	= 1111;
		m_CombinationInside		= m_Combination;
		m_CombinationLocked 	= 9999;
		m_LockDigitsCount 		= m_CombinationLocked.ToString().Length();
		m_IsLocked 				= false;
	}	
}