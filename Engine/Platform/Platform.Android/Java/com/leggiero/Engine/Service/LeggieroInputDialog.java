package com.leggiero.Engine.Service;

import android.os.Bundle;
import android.os.Build;
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
import android.widget.TextView;
import android.view.WindowManager;
import android.app.Dialog;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ActivityInfo;
import android.content.pm.ResolveInfo;
import java.util.List;
import com.leggiero.Engine.LeggieroActivity;


public class LeggieroInputDialog {

	static public void OpenInputDialog(final String title,
			final String message, final String acceptButtonName,
			final String cancelButtonName, 
			final String defaultText) {

		Log.v("Leggiero", "Open Input Dialog");

		LeggieroActivity.m_this.m_inputDialogText = defaultText;		
		LeggieroActivity.m_this.m_isInputDialogCanceled = false;
		LeggieroActivity.m_this.m_isInputDialogOpenRequested = true;
		LeggieroActivity.m_this.m_inputDialogCloseWhenTouchOutside = 1;

		Log.v("Leggiero", "Open Input Dialog try Run ui thread Start");
		LeggieroActivity.m_this.runOnUiThread(new Runnable() {
			public void run() {

				Context context = LeggieroActivity.m_this.getApplicationContext();
				Resources r = context.getResources();
				int input_dialog = r.getIdentifier("input_dialog", "layout", context.getPackageName());
				int layout_root = r.getIdentifier("layout_root", "id", context.getPackageName());
				int editTextDialogUserInput = r.getIdentifier("editTextDialogUserInput", "id", context.getPackageName());
				
				Log.v("Leggiero", "Open Input Dialog Run ui thread Start");
				LayoutInflater inflater = (LayoutInflater) LeggieroActivity.m_this
						.getApplicationContext().getSystemService(
								Context.LAYOUT_INFLATER_SERVICE);
				View layout = inflater.inflate(input_dialog,
						(ViewGroup) LeggieroActivity.m_this
								.findViewById(layout_root));

				final EditText userInput = (EditText) layout
						.findViewById(editTextDialogUserInput);

				userInput.setText(defaultText, TextView.BufferType.EDITABLE);

				AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(
						LeggieroActivity.m_this);
				alertDialogBuilder.setView(layout);

				userInput.performClick();
				userInput.requestFocus();
				
				alertDialogBuilder
						.setCancelable(true)
						.setPositiveButton(acceptButtonName,
								new DialogInterface.OnClickListener() {
									public void onClick(DialogInterface dialog,
											int id) {
										LeggieroActivity.m_this.m_inputDialogText = userInput
												.getText().toString();
										LeggieroActivity.m_this.m_isInputDialogCanceled = false;
										if (LeggieroActivity.m_inputDialog != null)
										{										
											LeggieroActivity.m_inputDialog.cancel();
											LeggieroActivity.m_inputDialog = null;
										}
									}
								})
						.setNegativeButton(cancelButtonName,
								new DialogInterface.OnClickListener() {
									public void onClick(DialogInterface dialog,
											int id) {
										LeggieroActivity.m_this.m_isInputDialogCanceled = true;
										if (LeggieroActivity.m_inputDialog != null)
										{
											LeggieroActivity.m_inputDialog.cancel();
											LeggieroActivity.m_inputDialog = null;
										}
									}
								})
						.setOnCancelListener(
								new DialogInterface.OnCancelListener() {
									public void onCancel(DialogInterface dialog)
									{
										LeggieroActivity.m_this.m_isInputDialogCanceled = true;
										if (LeggieroActivity.m_inputDialog != null)
										{
											LeggieroActivity.m_inputDialog = null;
										}
									}
								}
							);
						
				Log.v("Leggiero", "Open Input Dialog Show Start");
				LeggieroActivity.m_inputDialog = alertDialogBuilder.create();
				LeggieroActivity.m_this.m_isInputDialogOpenRequested = false;
				LeggieroActivity.m_inputDialog.setTitle(title);
				LeggieroActivity.m_inputDialog.getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_VISIBLE);
				LeggieroActivity.m_inputDialog.show();				
				
                InputMethodManager imm = (InputMethodManager)LeggieroActivity.m_this.getApplicationContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                imm.showSoftInput(userInput, InputMethodManager.SHOW_IMPLICIT);
				Log.v("Leggeiro", "Open Input Dialog Show Complete");
			}
		});

		Log.v("Leggeiro", "Open Input Dialog End");
	}

	static public int IsOpenInputDialog() {
		if (LeggieroActivity.m_this.m_isInputDialogOpenRequested == true) {
			// to avoid waiting race condition
			return 1;
		}

		if (null != LeggieroActivity.m_inputDialog) {
			return 1;
		}
		
		return 0;
	}

	static public byte[] GetInputDialogText() {
		try {
			LeggieroActivity.m_this.m_inputDialogTextByteArray = LeggieroActivity.m_this.m_inputDialogText.getBytes("UTF-8") ;		
		}
		catch (Exception e)
		{
			Log.v("Leggeiro", "GetInputDialogText encoding failed.");
		}
		return LeggieroActivity.m_this.m_inputDialogTextByteArray;
	}

	static public int GetDialogClosedState() {
		if (LeggieroActivity.m_this.m_isInputDialogCanceled) {
			return 1;
		}

		return 0;
	}
}
