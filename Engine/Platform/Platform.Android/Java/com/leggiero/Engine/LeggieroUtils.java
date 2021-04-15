package com.leggiero.Engine;

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
import java.lang.System;
import java.util.List;


public class LeggieroUtils {

	//////////////////////////////////////////////////////////////////////////////// Library Utils

	// Game Application Library Name
	public final static String kApplicationLibraryName = LeggieroProjectSetting.kApplicationName_lower + "app_android";

	// Load Native Library
	public static void LoadNativeLibraries() {
		System.loadLibrary("c++_shared");
		System.loadLibrary(LeggieroUtils.kApplicationLibraryName);
	}


	//////////////////////////////////////////////////////////////////////////////// Java Utils

	// Get TimeZone Name
	public static String GetTimeZoneName() {
		return TimeZone.getDefault().getID();
	}

	// Get TimeZone Offset in Second
	public static int GetTimeZoneOffsetInSec() {
		return (TimeZone.getDefault().getRawOffset() / 1000);
	}

	// Get Device Name
	public static String GetDeviceName() {
		final String manufacturer = Build.MANUFACTURER, model = Build.MODEL;
		return model.startsWith(manufacturer) ? _CapitalizePhrase(model) : _CapitalizePhrase(manufacturer) + " " + model;
	}

	// Get OS Version
	public static String GetOSVersionString() {
		return android.os.Build.VERSION.RELEASE;
	}


	//////////////////////////////////////////////////////////////////////////////// Internal Utils

	private static String _CapitalizePhrase(String s) {
		if (s == null || s.length() == 0)
			return s;
		else {
			StringBuilder phrase = new StringBuilder();
			boolean next = true;
			for (char c : s.toCharArray()) {
				if (next && Character.isLetter(c) || Character.isWhitespace(c))
					next = Character.isWhitespace(c = Character.toUpperCase(c));
				phrase.append(c);
			}
			return phrase.toString();
		}
	}
}
