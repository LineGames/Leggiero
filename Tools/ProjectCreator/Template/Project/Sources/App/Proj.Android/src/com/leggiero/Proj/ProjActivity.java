package com.leggiero.${{ProgramName}};

import android.util.Log;

import com.leggiero.Engine.LeggieroActivity;


// ${{ProgramName}} Activity
public class ${{ProgramName}}Activity extends LeggieroActivity {

	//////////////////////////////////////////////////////////////////////////////// Overriding Events

	@Override
	protected void onGameAppCreation() {
		Log.d("${{ProgramName}}", "onGameAppCreation");
	}

	@Override
	protected void onGameAppCreated() {
		Log.d("${{ProgramName}}", "onGameAppCreated");
	}

}
