
//! TrialService is used to query if the game is trial version or not
class TrialService
{
	/*!
		Xbox: Retrieves if the game license is trial
			
		@param bIsSimulator switch in between the testing simulator for trial and the retail version.
							If the game is in retail mode it automatically ignores this switch and 
							uses the correct app runtime version.
		@return boolean if the game is in trial mode.		
	*/
	proto native bool IsGameTrial(bool bIsSimulator);
	
	/*!
		Xbox: Retrieves if the game license is active
			
		@param bIsSimulator switch in between the testing simulator for trial and the retail version.
							If the game is in retail mode it automatically ignores this switch and 
							uses the correct app runtime version.
		@return boolean if the game is in trial mode.		
	*/
	proto native bool IsGameActive(bool bIsSimulator);
};
