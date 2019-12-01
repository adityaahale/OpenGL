package com.aditya.withoutobject;

import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.AppCompatTextView;
//import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Window;
import android.view.View;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.view.Gravity;
import android.content.Context;

public class MainActivity extends AppCompatActivity {
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		this.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION|View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		//this.getWindow().getDecorView().setSystemUiVisibility(View.GONE);
		this.supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
		
		//make fullscreen
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		//landscape
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		this.getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		
		//our view
		//myView = new MyView(this);
		AppCompatTextView myView = new AppCompatTextView(this);
		myView.setTextColor(Color.rgb(0,255,0));
		myView.setTextSize(60);
		myView.setGravity(Gravity.CENTER);
		myView.setText("Hello World !!!");
		setContentView(myView);
    }
	
	@Override
	protected void onPause()
	{
		super.onPause();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
	}
}
