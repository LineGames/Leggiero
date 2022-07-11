package com.leggiero.Engine;

import android.os.Bundle;
import android.os.Build;
import android.os.IBinder;
import android.os.AsyncTask;
import android.app.PendingIntent;
import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.pm.PackageManager;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.telephony.TelephonyManager;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.DisplayCutout;
import android.view.View;
import android.view.WindowManager;
import android.view.WindowInsets;
import android.provider.Settings.Secure;
import androidx.fragment.app.FragmentActivity;
import java.lang.reflect.Method;
import java.lang.Thread;
import java.lang.System;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.security.SecureRandom;

import com.leggiero.Engine.LeggieroApplication;
import com.leggiero.Engine.OpenGL.LeggieroGLSurfaceView;
import com.leggiero.Engine.OpenGL.LeggieroGLRenderer;
import com.leggiero.Engine.ApplicationReferrerReceiver;

import com.leggiero.Engine.Modules.FileSystem.FileSystemHelper;
import com.leggiero.Engine.Modules.Sound.SoundHelper;
import com.leggiero.Engine.Modules.Sound.BGMHelper;


public class LeggieroActivity extends FragmentActivity {

	// Static Singleton-like Interface
	public static LeggieroActivity m_this;

	protected LeggieroGLSurfaceView m_surfaceView;
	protected LeggieroGLRenderer m_renderer;


	static {
		LeggieroUtils.LoadNativeLibraries();
	}
	

	// Native Interfaces
	protected native void InitializeLeggieroApp(boolean isTablet, int width, int height, float screenXPPI, float screenYPPI, String localeString);
	
	protected native void OnAppStart();
	protected native void OnAppStop();
	protected native void OnAppRestart();
	protected native void OnAppResume();
	protected native void OnAppPause();
	protected native void OnAppDestroy();

	protected native int OnSystemBackButtonPressed();

	protected native void OnLowMemoryWarning();

	protected native void NotifySafePaddingArea(int left, int top, int right, int bottol);
	

	//////////////////////////////////////////////////////////////////////////////// Initialization

	@Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
		m_this = this;
        m_surfaceView = new LeggieroGLSurfaceView(this);
		if (Build.VERSION.SDK_INT >= 11) {
			m_surfaceView.setPreserveEGLContextOnPause(true);
		}

		// Set Immersive Mode
		setSystemUiVisibility(false);

		if (Build.VERSION.SDK_INT >= 19) {
			m_surfaceView.setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener() {
				@Override
				public void onSystemUiVisibilityChange(int visibility) {
					if( ( (visibility & View.SYSTEM_UI_FLAG_HIDE_NAVIGATION) == 0 ) ||
					    ( (visibility & View.SYSTEM_UI_FLAG_FULLSCREEN     ) == 0 ) ){
						setSystemUiVisibility(false);
					}
				}
			});
		}

		if (Build.VERSION.SDK_INT >= 28) {
			m_surfaceView.setOnApplyWindowInsetsListener(new View.OnApplyWindowInsetsListener() {
				@Override
				public WindowInsets onApplyWindowInsets(View v, WindowInsets insets) {
					handleCutoutApplied(insets.getDisplayCutout());
					return insets.consumeDisplayCutout();
				}
			});
		}

		setUseCutoutArea(true);

		// Set Screen Orientation Mode
		setSystemOrientation();

        m_surfaceView.setEGLContextClientVersion(3);
        m_surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 8);
        
        m_renderer = new LeggieroGLRenderer();
        m_surfaceView.setRenderer(m_renderer);
        setContentView(m_surfaceView);
        
        StartUpLeggieroApp();
    }

	protected void StartUpLeggieroApp()
	{
		ContextWrapper cw = new ContextWrapper(this);

		initializeModules(cw);
		
		// Initialize Screen Size
		Display display = getWindowManager().getDefaultDisplay();
		int realWidth;
		int realHeight;
		float screenXPPI;
		float screenYPPI;
		if (Build.VERSION.SDK_INT >= 17){
			DisplayMetrics realMetrics = new DisplayMetrics();
			display.getRealMetrics(realMetrics);
			realWidth = realMetrics.widthPixels;
			realHeight = realMetrics.heightPixels;
			screenXPPI = realMetrics.xdpi;
			screenYPPI = realMetrics.ydpi;
		} else if (Build.VERSION.SDK_INT >= 14) {
			try {
				Method mGetRawH = Display.class.getMethod("getRawHeight");
				Method mGetRawW = Display.class.getMethod("getRawWidth");
				realWidth = (Integer) mGetRawW.invoke(display);
				realHeight = (Integer) mGetRawH.invoke(display);
			} catch (Exception e) {
				realWidth = display.getWidth();
				realHeight = display.getHeight();
			}

			DisplayMetrics dm = new DisplayMetrics();
			display.getMetrics(dm);
			screenXPPI = dm.xdpi;
			screenYPPI = dm.ydpi;
		} else {
			realWidth = display.getWidth();
			realHeight = display.getHeight();

			DisplayMetrics dm = new DisplayMetrics();
			display.getMetrics(dm);
			screenXPPI = dm.xdpi;
			screenYPPI = dm.ydpi;
		}

		// Deal with Pentile Display's abnormal xdpi value
		if (screenXPPI > screenYPPI)
		{
			float screenPPIRatio = screenXPPI / screenYPPI;
			if (screenPPIRatio > 2.9f && screenPPIRatio > 3.1f)
			{
				screenXPPI /= 3.0f;
			}
		}
		else
		{
			float screenPPIRatio = screenYPPI / screenXPPI;
			if (screenPPIRatio > 2.9f && screenPPIRatio > 3.1f)
			{
				screenYPPI /= 3.0f;
			}
		}

		// Locale
		String localeString = GetSystemDefaultLocaleString();

		// Initialization by App
		onGameAppCreation();

		// Initialize Application
		InitializeLeggieroApp(IsTablet(cw), realWidth, realHeight, screenXPPI, screenYPPI, localeString);

		// After
		onGameAppCreated();
	}

	protected void initializeModules(ContextWrapper cw) {
		FileSystemHelper fileSysHelper = new FileSystemHelper();
		fileSysHelper.InitializeHelper(cw);

		SoundHelper soundHelper = new SoundHelper();
		soundHelper.InitializeHelper(cw);

		BGMHelper bgmHelper = new BGMHelper();
		bgmHelper.InitializeHelper(cw);
	}

	protected void onGameAppCreation() {
		// to override
	}

	protected void onGameAppCreated() {
		// to override
	}


	//////////////////////////////////////////////////////////////////////////////// Setting

	protected void setSystemOrientation() {
		if (LeggieroProjectSetting.kIsLandscapeMode) {
			if (Build.VERSION.SDK_INT >= 18) {
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_USER_LANDSCAPE);
			}
			else {
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
			}
		}
		else {
			if (Build.VERSION.SDK_INT >= 18) {
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_USER_PORTRAIT);
			}
			else {
				setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
			}
		}
	}

	protected void setSystemUiVisibility(boolean isVisible) {
		if (Build.VERSION.SDK_INT < 19) {
			getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}else{
			int uiOptions = View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						  | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
						  | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN;
			if( isVisible == false ){
				uiOptions |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
						  |  View.SYSTEM_UI_FLAG_FULLSCREEN
						  |  View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
			}
			if (m_surfaceView != null) {
        		m_surfaceView.setSystemUiVisibility(uiOptions);
			}
		}
	}

	protected void setUseCutoutArea(boolean isUseCutout) {
		if (Build.VERSION.SDK_INT >= 28) {
			if (isUseCutout) {
				getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;
			}
			else {
				getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_NEVER;
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////////// Setting Handling

	protected int m_safePaddingLeft, m_safePaddingTop, m_safePaddingRight, m_safePaddingBottom;

	protected void handleCutoutApplied(DisplayCutout cutout) {
		if (cutout == null) {
			m_safePaddingLeft = 0;
			m_safePaddingTop = 0;
			m_safePaddingRight = 0;
			m_safePaddingBottom = 0;
		}
		else {
			m_safePaddingLeft = cutout.getSafeInsetLeft();
			m_safePaddingTop = cutout.getSafeInsetTop();
			m_safePaddingRight = cutout.getSafeInsetRight();
			m_safePaddingBottom = cutout.getSafeInsetBottom();
		}
		NotifySafePaddingArea(m_safePaddingLeft, m_safePaddingTop, m_safePaddingRight, m_safePaddingBottom);
	}


	//////////////////////////////////////////////////////////////////////////////// System Event Handle
	
	@Override
    protected void onDestroy() {
    	if (m_renderer != null) {
        	m_renderer.RequestGraphicShutdown();
        }
		OnAppDestroy();
		m_renderer = null;
		m_surfaceView = null;
		super.onDestroy();
    }
    
	@Override
    protected void onStart() {
		OnAppStart();
		super.onStart();
    }

    @Override
    protected void onStop() {
		OnAppStop();
        super.onStop();
    }

	@Override
    protected void onRestart() {
		OnAppRestart();
        super.onRestart();
    }
	
    @Override
    protected void onResume() {
        OnAppResume();
        if (m_surfaceView != null) {
        	m_surfaceView.onResume();
        }
		super.onResume();
    }

    @Override
    protected void onPause() {
        OnAppPause();
        if (m_surfaceView != null) {
        	m_surfaceView.onPause();
        }
		super.onPause();
    }

	@Override
	public void onBackPressed() {
		int handledResult = OnSystemBackButtonPressed();
		if (handledResult == 0) {
			// Check process by game app
			if (!onBackButtonProcessing()) {
				if (Build.VERSION.SDK_INT >= 11) {
					if (m_surfaceView != null) {
						m_surfaceView.setPreserveEGLContextOnPause(false);
					}
				}
				super.onBackPressed();
			}
		}
	}

	protected boolean onBackButtonProcessing() {
		// to override
		return false;
	}

	@Override
	public void onLowMemory() {
		OnLowMemoryWarning();
		super.onLowMemory();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);

		if (LeggieroProjectSetting.kIsLandscapeMode) {
			if (newConfig.orientation != Configuration.ORIENTATION_LANDSCAPE) {
				setSystemOrientation();
			}
		}
		else {
			if (newConfig.orientation != Configuration.ORIENTATION_PORTRAIT) {
				setSystemOrientation();
			}
		}
	}

	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus) {
			setSystemUiVisibility( false );
		}
	}


	//////////////////////////////////////////////////////////////////////////////// Activity Utility

	public int GetAndroidApiLevel() {
		return Build.VERSION.SDK_INT;
	}

	public static boolean IsTablet(Context context) {
		TelephonyManager manager = 
			(TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
		if (manager.getPhoneType() == TelephonyManager.PHONE_TYPE_NONE) {
			// Tablet
			return true;
		} else {
			// Mobile
			return false;
		}
	}

	public String GetSystemDefaultLocaleString() {
		Locale currentDefaultLocale = Locale.getDefault();
		String localeString = currentDefaultLocale.getLanguage();
		String countryString = currentDefaultLocale.getCountry();
		if (countryString != "")
		{
			localeString = localeString + "_" + countryString;
		}

		return localeString;
	}

	// Get Stored Referrer
	public String GetStoredReferrerString() {
		SharedPreferences referrerPreference = getSharedPreferences(ApplicationReferrerReceiver.AppReferrer_PREFS_NAME, Context.MODE_PRIVATE);

		String cachedReferrerString = referrerPreference.getString(ApplicationReferrerReceiver.AppReferrer_PREF_KEY, null);
		if (cachedReferrerString != null)
		{
			return cachedReferrerString;
		}

		return "";
	}

	// Force Finish Activity
	public void FinishActivity() {
		System.exit(0);
	}

	// Set Device Sleep Mode by App
	public void SetNoSleepMode(boolean isPreventSleep) {
		if (m_surfaceView != null) {
			final boolean isPreventSleepCopy = isPreventSleep;
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					m_surfaceView.setKeepScreenOn(isPreventSleepCopy);
				}
			});
		}
	}


	//////////////////////////////////////////////////////////////////////////////// Input Dialog Service

	public static Dialog m_inputDialog = null;
	public static String m_inputDialogText;
	public static int m_inputDialogCloseWhenTouchOutside;
	public static boolean m_isInputDialogCanceled;
	public static boolean m_isInputDialogOpenRequested = false;
	public static byte[] m_inputDialogTextByteArray;
}
