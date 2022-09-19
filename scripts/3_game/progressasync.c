class ProgressAsync
{
	//proto native void SetUserData(Widget inst);
	proto static native void SetUserData(Widget inst);
	proto static native void SetProgressData(Widget inst);
	proto static native void DestroyAllPendingProgresses();
	proto static native void StartProgress();
}