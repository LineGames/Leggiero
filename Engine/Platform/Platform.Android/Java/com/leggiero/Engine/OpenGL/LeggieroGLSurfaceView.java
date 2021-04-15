package com.leggiero.Engine.OpenGL;

import android.content.Context;
import android.util.AttributeSet;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.os.SystemClock;
import com.leggiero.Engine.LeggieroUtils;


/**
 * Custom GLSurfaceView to process events
 */
public class LeggieroGLSurfaceView extends GLSurfaceView {

	static {
		LeggieroUtils.LoadNativeLibraries();
	}
	
	private native void AndroidTouchDown(int pointerId, float x, float y, long eventTime, long currentTime);
	private native void AndroidTouchMove(int pointerId, float x, float y, long eventTime, long currentTime);
	private native void AndroidTouchUp(int pointerId, float x, float y, long eventTime, long currentTime);
	
	
	public LeggieroGLSurfaceView(Context context) {
        super(context);
    }
	
	public LeggieroGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

	
	/**
	 * Touch Event Handler
	 */
	public boolean onTouchEvent(final MotionEvent event) {  
		
		final int historySize = event.getHistorySize();
		final int pointerCount = event.getPointerCount();

		final int[] ids = new int[pointerCount];
        final float[] xs = new float[pointerCount];
        final float[] ys = new float[pointerCount];
		
		boolean isProcessed = false;

		for (int p = 0; p < pointerCount; p++) {
			ids[p] = event.getPointerId(p);
		}
		
		for (int h = 0; h < historySize; h++) {
			long historicalEventTime = event.getHistoricalEventTime(h);
			for (int p = 0; p < pointerCount; p++) {
				xs[p] = event.getHistoricalX(p, h);
				ys[p] = event.getHistoricalY(p, h);
			}
			switch (event.getAction() & MotionEvent.ACTION_MASK)
			{
			case MotionEvent.ACTION_DOWN:
				{
					AndroidTouchDown(ids[0], xs[0], ys[0], 
							historicalEventTime, SystemClock.uptimeMillis());
				}
				break;

			case MotionEvent.ACTION_POINTER_DOWN:
				{
					final int pointerIdx = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
					AndroidTouchDown(ids[pointerIdx], xs[pointerIdx], ys[pointerIdx], 
							historicalEventTime, SystemClock.uptimeMillis());
				}
				break;
					
			case MotionEvent.ACTION_MOVE:
				{
					for (int p = 0; p < pointerCount; p++) {
						AndroidTouchMove(ids[p], xs[p], ys[p], 
							historicalEventTime, SystemClock.uptimeMillis());
					}
				}
				break;

			case MotionEvent.ACTION_POINTER_UP:
				{
					final int pointerIdx = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
					AndroidTouchUp(ids[pointerIdx], xs[pointerIdx], ys[pointerIdx], 
							historicalEventTime, SystemClock.uptimeMillis());
				}
				break;
					
			case MotionEvent.ACTION_UP:
				{
					AndroidTouchUp(ids[0], xs[0], ys[0], 
							historicalEventTime, SystemClock.uptimeMillis());
				}
				break;
			}
		}
		
		long eventTime = event.getEventTime();
		for (int p = 0; p < pointerCount; p++) {
			xs[p] = event.getX(p);
			ys[p] = event.getY(p);
		}
		switch (event.getAction() & MotionEvent.ACTION_MASK)
		{
			case MotionEvent.ACTION_DOWN:
				{
					isProcessed = true;
					AndroidTouchDown(ids[0], xs[0], ys[0], 
							eventTime, SystemClock.uptimeMillis());
				}
				break;

			case MotionEvent.ACTION_POINTER_DOWN:
				{
					isProcessed = true;
					final int pointerIdx = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
					AndroidTouchDown(ids[pointerIdx], xs[pointerIdx], ys[pointerIdx], 
							eventTime, SystemClock.uptimeMillis());
				}
				break;
					
			case MotionEvent.ACTION_MOVE:
				{
					isProcessed = true;
					for (int p = 0; p < pointerCount; p++) {
						AndroidTouchMove(ids[p], xs[p], ys[p], 
							eventTime, SystemClock.uptimeMillis());
					}
				}
				break;

			case MotionEvent.ACTION_POINTER_UP:
				{
					isProcessed = true;
					final int pointerIdx = event.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
					AndroidTouchUp(ids[pointerIdx], xs[pointerIdx], ys[pointerIdx], 
							eventTime, SystemClock.uptimeMillis());
				}
				break;
					
			case MotionEvent.ACTION_UP:
				{
					isProcessed = true;
					AndroidTouchUp(ids[0], xs[0], ys[0], 
							eventTime, SystemClock.uptimeMillis());
				}
				break;
		}
		
		if (isProcessed)
		{
			return true;
		}
		
		return super.onTouchEvent(event);
    }  
}
