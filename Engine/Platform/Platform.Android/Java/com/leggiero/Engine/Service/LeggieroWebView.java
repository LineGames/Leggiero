package com.leggiero.Engine.Service;

import android.os.Bundle;
import android.os.Build;
import java.util.TimeZone;
import android.view.inputmethod.InputMethodManager;
import android.content.res.Resources;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.view.WindowManager;
import android.app.Dialog;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ActivityInfo;
import android.net.Uri;
import android.content.pm.ResolveInfo;
import java.util.List;
import com.leggiero.Engine.LeggieroActivity;


public class LeggieroWebView {

	static public boolean s_isURLPageOpened = false;

	static public void OpenURLByPage(String title, String backButtonName, String url)
	{		
		URLActivity.m_url = url;
		URLActivity.m_backButtonName = backButtonName;

        Intent intent = new Intent(LeggieroActivity.m_this, URLActivity.class);
        LeggieroActivity.m_this.startActivity(intent);
	}

	static public int IsURLPageOpened()
	{
		if (s_isURLPageOpened) {
			return 1;
		}
		return 0;
	}

	static public int CanOpenURL(String url)
	{		
		String uri = url;
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(uri));
		PackageManager    packageManager     = LeggieroActivity.m_this.getApplicationContext().getPackageManager();
		List<ResolveInfo> resolvedActivities = packageManager.queryIntentActivities(intent, 0);
		if(resolvedActivities.size() > 0)
		{
			return 1;
		}
		return 0;
	}

	static public void OpenURL(String url)
	{
		Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
		LeggieroActivity.m_this.startActivity(intent);
	}
}
