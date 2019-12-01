package com.aditya.hello;

import android.support.v7.widget.AppCompatTextView;
//import androidx.appcompat.app.AppCompatActivity;
import android.content.Context;
import android.view.Gravity;
import android.graphics.Color;

public class MyView extends AppCompatTextView
{
	public MyView(Context drawingContext)
	{
		super(drawingContext);
		setTextColor(Color.rgb(0,255,0));
		setTextSize(60);
		setGravity(Gravity.CENTER);
		setText("Hello World !!!");
	}
}
