package com.leggiero.Engine.Modules.Sound;

import android.content.Context;
import android.content.ContextWrapper;
import android.os.Build;
import android.os.Environment;
import android.media.AudioManager;
import com.leggiero.Engine.LeggieroActivity;
import com.leggiero.Engine.LeggieroUtils;


public class SoundHelper {
	
	static {
		LeggieroUtils.LoadNativeLibraries();
	}
	
	// Modules/Sound/Platform/AndroidSoundJNIInterface.cpp
	private native void InitializeDefaultStreamValues(int sampleRate, int framesPerBurst);

	public SoundHelper()
	{
	}
	
	public void InitializeHelper(ContextWrapper cw)
	{
		if (Build.VERSION.SDK_INT >= 17)
		{
			AudioManager myAudioMgr = (AudioManager)cw.getSystemService(Context.AUDIO_SERVICE);
			String sampleRateStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
			int defaultSampleRate = Integer.parseInt(sampleRateStr);
			String framesPerBurstStr = myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
			int defaultFramesPerBurst = Integer.parseInt(framesPerBurstStr);

			InitializeDefaultStreamValues(defaultSampleRate, defaultFramesPerBurst);
		}
	}
}
