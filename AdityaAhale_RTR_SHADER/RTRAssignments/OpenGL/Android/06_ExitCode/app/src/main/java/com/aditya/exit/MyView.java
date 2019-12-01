package com.aditya.exit;

import android.support.v7.widget.AppCompatTextView;
//import androidx.appcompat.app.AppCompatActivity;
import android.content.Context;
import android.view.Gravity;
import android.graphics.Color;
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

public class MyView extends AppCompatTextView implements OnGestureListener, OnDoubleTapListener
{
	private GestureDetector gestureDetector;

	public MyView(Context drawingContext)
	{
		super(drawingContext);
		setTextColor(Color.rgb(0,255,0));
		setTextSize(60);
		setGravity(Gravity.CENTER);
		setText("Hello World !!!");

		gestureDetector = new GestureDetector(drawingContext,this,null,false);
		gestureDetector.setOnDoubleTapListener(this);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		//code
		int eventaction = event.getAction();
		if(!gestureDetector.onTouchEvent(event))
			super.onTouchEvent(event);
		return(true);
	}

	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		//code
		setText("Double Tap");
		return(true);
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		//code
		return(true);
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		//code
		setText("Single Tap");
		return(true);
	}

	@Override
	public boolean onDown(MotionEvent e)
	{
		//code
		return(true);
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		//code
		return(true);
	}

	@Override
	public void onLongPress(MotionEvent e)
	{
		//code
		setText("Long Press");
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		//code
		// setText("Scroll");
		System.exit(0);
		return(true);
	}

	@Override
	public void onShowPress(MotionEvent e)
	{
		//code
	}

	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		//code
		return(true);
	}

}
