package com.leggiero.Engine;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;


public class ApplicationReferrerReceiver extends BroadcastReceiver {

	public final static String AppReferrer_PREFS_NAME = LeggieroProjectSetting.kApplicationName_lower + "AppReferrer_Pref";
	public final static String AppReferrer_PREF_KEY = LeggieroProjectSetting.kApplicationName_lower + "AppReferrer";

	@Override
    public void onReceive(Context context, Intent intent) {
		String referrerString = "";
		if (intent.getAction().equals("com.android.vending.INSTALL_REFERRER")) {
			referrerString = intent.getStringExtra("referrer");
			if (referrerString != null) {
				SharedPreferences referrerPreference = context.getSharedPreferences(AppReferrer_PREFS_NAME, Context.MODE_PRIVATE);

				final Editor e = referrerPreference.edit();
				e.putString(AppReferrer_PREF_KEY, referrerString);
				e.commit();
			}
		}
    }
}
