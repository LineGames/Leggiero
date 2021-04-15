package com.leggiero.Engine.OpenGL;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView.Renderer;

import com.leggiero.Engine.LeggieroUtils;


public class LeggieroGLRenderer implements Renderer {

	static {
		LeggieroUtils.LoadNativeLibraries();
	}
	
	private native void LeggieroGraphicInitialize();
	private native void LeggieroGraphicShutdown();
	
	private native void LeggieroDrawFrame();
	private native void OnSurfaceSizeChanged(int width, int height);
	

	@Override
	public void onDrawFrame(GL10 arg0) {
		if (m_isShutdownRequest)
		{
			DoGraphicShutdown();
			return;
		}
		
		if (m_isInitialized)
		{
			LeggieroDrawFrame();
		}
	}

	@Override
	public void onSurfaceChanged(GL10 arg0, int arg1, int arg2) {
		OnSurfaceSizeChanged(arg1, arg2);
	}

	@Override
	public void onSurfaceCreated(GL10 arg0, EGLConfig arg1) {
		LeggieroGraphicInitialize();
		m_isInitialized = true;
	}


	private boolean m_isShutdownRequest = false;
	private boolean m_isInitialized = false;
	
	public void RequestGraphicShutdown()
	{
		m_isShutdownRequest = true;
	}
	
	private void DoGraphicShutdown()
	{
		m_isShutdownRequest = false;
		if (m_isInitialized)
		{
			m_isInitialized = false;
			LeggieroGraphicShutdown();
		}
	}
}
