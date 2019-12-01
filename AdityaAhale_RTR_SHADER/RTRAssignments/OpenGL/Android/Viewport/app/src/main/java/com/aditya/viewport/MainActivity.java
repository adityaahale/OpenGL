package com.aditya.viewport;


import android.support.v7.app.AppCompatActivity;
//import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Window;
import android.view.View;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.graphics.Color;

public class MainActivity extends AppCompatActivity {
	
	private GLESView glesView;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
		
    	System.out.println("RTR: Im in onCreate");
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		this.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION|View.SYSTEM_UI_FLAG_HIDE_NAVIGATION|View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
		//this.getWindow().getDecorView().setSystemUiVisibility(View.GONE);
		this.supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
		
		//make fullscreen
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		//landscape
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		
		//our view
		glesView = new GLESView(this);
		setContentView(glesView);
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
