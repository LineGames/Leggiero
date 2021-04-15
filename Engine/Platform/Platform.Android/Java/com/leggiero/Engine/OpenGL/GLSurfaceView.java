package com.leggiero.Engine.OpenGL;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;


public class GLSurfaceView extends SurfaceView implements SurfaceHolder.Callback {

	public GLSurfaceView(Context context) {
        super(context);
        InitializeView();
    }
	
	public GLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        InitializeView();
    }
	
	private void InitializeView() {
		m_surfaceHolder = getHolder();
		m_surfaceHolder.addCallback(this);
    }
	
	public SurfaceHolder GetSurfaceHolder() {
        return m_surfaceHolder;
    }
	
	private SurfaceHolder m_surfaceHolder;
	
	//////////////////////////////////////////////////////////////////////////////// SurfaceHolder.Callback
	@Override
	public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
		
	}

	@Override
	public void surfaceCreated(SurfaceHolder arg0) {
		
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder arg0) {
		
	}

}
