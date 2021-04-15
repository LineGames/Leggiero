package com.leggiero.Engine.Modules.Sound;

import android.content.Context;
import android.content.ContextWrapper;
import android.os.Build;
import android.os.Environment;
import android.media.AudioManager;
import android.media.MediaPlayer;
import com.leggiero.Engine.LeggieroActivity;
import com.leggiero.Engine.LeggieroUtils;


public class BGMPlayingContext implements MediaPlayer.OnCompletionListener, MediaPlayer.OnErrorListener {
	
	static {
		LeggieroUtils.LoadNativeLibraries();
	}
	
	// Modules/Sound/Platform/AndroidBGMJNIInterface.cpp
	private native void NotifyOnPlayFinished(int contextId, int isFinished);

	protected MediaPlayer	m_player;
	protected int			m_contextId;

	protected boolean		m_isPrepared;
	protected boolean		m_isInError;

	public BGMPlayingContext(MediaPlayer myPlayer, int contextId)
	{
		m_contextId = contextId;

		m_player = myPlayer;
		m_player.setOnCompletionListener(this);
		m_player.setOnErrorListener(this);

		m_isPrepared = false;
		m_isInError = false;
	}

	public void DoPrepare()
	{
		if (m_isPrepared)
		{
			return;
		}

		try {
			m_player.prepare();
			m_isPrepared = true;
		}
		catch (Exception e) {
			m_isInError = true;
		}
	}

	public void DoPlay()
	{
		if (!m_isPrepared)
		{
			DoPrepare();
			if (!m_isPrepared)
			{
				// Some Error
				return;
			}
		}

		if (m_isInError)
		{
			m_player.reset();
			m_isInError = false;
		}

		try {
			m_player.start();
		}
		catch (Exception e) {
			m_isInError = true;
		}
	}

	public void DoStop()
	{
		try {
			if (m_player.isPlaying()) {
				m_player.stop();
			}
		}
		catch (Exception e) {
			// Ignore
		}
	}

	public void DoPause()
	{
		try {
			if (m_player.isPlaying()) {
				m_player.pause();
			}
		}
		catch (Exception e) {
			// Ignore
		}
	}

	public int IsPlaying()
	{
		if (m_isInError)
		{
			return 0;
		}

		boolean isPlayingNow = false;
		try {
			isPlayingNow = m_player.isPlaying();
		}
		catch (Exception e) {
			// Ignore
		}

		return (isPlayingNow ? 1 : 0);
	}

	public int GetLengthInMS()
	{
		if (!m_isPrepared)
		{
			DoPrepare();
			if (!m_isPrepared)
			{
				// Some Error
				return 0;
			}
		}
		return m_player.getDuration();
	}

	public int GetCurrentPositionInMS()
	{
		return m_player.getCurrentPosition();
	}

	public void DoSeek(int positionInMS)
	{
		try {
			m_player.seekTo(positionInMS);
		}
		catch (Exception e) {
			// Ignore
		}
	}

	public int GetIsLooping()
	{
		return (m_player.isLooping() ? 1 : 0);
	}

	public void DoSetLooping(int isLooping)
	{
		boolean toLoop = (isLooping == 1);
		m_player.setLooping(toLoop);
	}
	
	public void DoSetVolume(float volume)
	{
		m_player.setVolume(volume, volume);
	}

	public void ReleaseResource()
	{
		m_player.release();
		m_player = null;
	}

	@Override
    public void onCompletion(MediaPlayer mp) {
        NotifyOnPlayFinished(m_contextId, (m_isInError ? 0 : 1));
    }

	@Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
		m_isInError = true;
        return false;
    }
}
