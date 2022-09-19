class EmoteConstructor
{
	bool ConstructEmotes(PlayerBase player, out map<int, ref EmoteBase> emoteMap)
	{
		TTypenameArray emote_array_names = new TTypenameArray;
		RegisterEmotes(emote_array_names);
		Sort(emote_array_names,emote_array_names.Count());
		
		emoteMap = new map<int, ref EmoteBase>;
		ref EmoteBase new_emote;
		
		for (int i = 0; i < emote_array_names.Count(); i++)
		{
			new_emote = EmoteBase.Cast(emote_array_names[i].Spawn());
			if (new_emote)
			{
				if (new_emote.GetID() < 0)
				{
					ErrorEx("Emote " + emote_array_names[i].ToString() + " has an invalid ID, registation failed!");
					return false;
				}
				else if (emoteMap.Contains(new_emote.GetID()))
				{
					ErrorEx("Emote " + emote_array_names[i].ToString() + " has a duplicate ID, registation failed!");
					return false;
				}
				else
				{
					new_emote.SetOwnerPlayer(player); 
					emoteMap.Insert(new_emote.GetID(), new_emote);
				}
			}
		}
		return true;
	}
	
	void RegisterEmotes(TTypenameArray emotes)
	{
		emotes.Insert(EmoteGreeting);
		emotes.Insert(EmoteSOS);
		emotes.Insert(EmoteHeart);
		emotes.Insert(EmoteTaunt);
		emotes.Insert(EmoteLyingDown);
		emotes.Insert(EmoteTauntKiss);
		emotes.Insert(EmotePoint);
		emotes.Insert(EmoteTauntElbow);
		emotes.Insert(EmoteThumb);
		emotes.Insert(EmoteThumbDown);
		emotes.Insert(EmoteThroat);
		emotes.Insert(EmoteDance);
		emotes.Insert(EmoteSalute);
		emotes.Insert(EmoteTimeout);
		//emotes.Insert(EmoteDabbing);
		emotes.Insert(EmoteFacepalm);
		emotes.Insert(EmoteClap);
		emotes.Insert(EmoteSilent);
		emotes.Insert(EmoteWatching);
		emotes.Insert(EmoteHold);
		emotes.Insert(EmoteListening);
		emotes.Insert(EmotePointSelf);
		emotes.Insert(EmoteLookAtMe);
		emotes.Insert(EmoteTauntThink);
		emotes.Insert(EmoteMove);
		emotes.Insert(EmoteGetDown);
		emotes.Insert(EmoteCome);
		emotes.Insert(EmoteSurrender); //exception, partially handled in EmoteManager directly (..)
		emotes.Insert(EmoteCampfireSit);
		emotes.Insert(EmoteSitA);
		emotes.Insert(EmoteSitB);
		emotes.Insert(EmoteRPSRandom);
		emotes.Insert(EmoteRPSRock);
		emotes.Insert(EmoteRPSPaper);
		emotes.Insert(EmoteRPSScisors);
		emotes.Insert(EmoteNod);
		emotes.Insert(EmoteShake);
		emotes.Insert(EmoteShrug);
		emotes.Insert(EmoteSuicide);
		emotes.Insert(EmoteVomit);
	}
}