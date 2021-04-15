package com.leggiero.Engine.Modules.Sound;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileDescriptor;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.res.AssetManager;
import android.content.res.AssetFileDescriptor;
import android.os.Build;
import android.os.Environment;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.net.Uri;
import com.leggiero.Engine.LeggieroActivity;
import com.leggiero.Engine.LeggieroUtils;
import com.leggiero.Engine.Modules.Sound.BGMPlayingContext;


public class BGMHelper {
	
	protected static ContextWrapper m_globalContext;


	static {
	}
	
	public BGMHelper()
	{
	}
	
	public void InitializeHelper(ContextWrapper cw)
	{
		m_globalContext = cw;
	}

	public static BGMPlayingContext CreatePlayingContextFromFile(String filePath, int contextId)
	{
		MediaPlayer player = null;

		FileInputStream fis = null;
		FileDescriptor fd = null;
		try {
            fis = new FileInputStream(filePath);
			fd = fis.getFD();
			if (!fd.valid())
			{
				// Invalid File Descriptor
				return null;
			}

			player = new MediaPlayer();
			try {
				player.setDataSource(fd);
			}
			catch (Exception e) {
				// Cannot use the given file as a source
				return null;
			}
        }
        catch (Exception ex) {
			// Input file error
            return null;
        }
        finally {
			if (fis != null)
			{
				try {
					fis.close();
				}
				catch (Exception e) {
					// Ignore close() exception
				}
			}
        }

		if (Build.VERSION.SDK_INT < 26) {
			player.setAudioStreamType(AudioManager.STREAM_MUSIC);
		}

		return new BGMPlayingContext(player, contextId);
	}

	public static BGMPlayingContext CreatePlayingContextInBundle(String virtualPath, int contextId)
	{
		if (m_globalContext == null)
		{
			return null;
		}

		AssetManager am = m_globalContext.getAssets();
		if (am == null)
		{
			return null;
		}

		AssetFileDescriptor afd = null;
		MediaPlayer player = null;
		try {
			afd = am.openFd(virtualPath);
			player = new MediaPlayer();
			try {
				if (Build.VERSION.SDK_INT >= 24) {
					player.setDataSource(afd);
				}
				else {
					player.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
				}
			}
			catch (Exception ex) {
				// Cannot use the given asset as a source
				return null;
			}
		}
		catch (Exception e) {
			// Invalid Asset
			return null;
		}
		finally {
			if (afd != null)
			{
				try {
					afd.close();
				}
				catch (Exception ex) {
					// Ignore close() exception
				}
			}
        }
		
		if (Build.VERSION.SDK_INT < 26) {
			player.setAudioStreamType(AudioManager.STREAM_MUSIC);
		}

		return new BGMPlayingContext(player, contextId);
	}
}
