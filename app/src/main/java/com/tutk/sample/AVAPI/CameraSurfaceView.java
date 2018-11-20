package com.tutk.sample.AVAPI;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class CameraSurfaceView extends SurfaceView implements SurfaceHolder.Callback ,Runnable{
    private SurfaceHolder mHolder;
    private Canvas mCanvas;//绘图的画布
    private volatile boolean mIsDrawing;//控制绘画线程的标志位
    private Paint mPaint;
    private Path mPath;



    public CameraSurfaceView(Context context) {
        super(context);
        initView();
    }

    public CameraSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView();
    }

    public CameraSurfaceView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initView();
    }

    private void initView() {
        mPaint = new Paint();
        mPaint.setColor(Color.GREEN);//画笔颜色
        mPaint.setAntiAlias(true);//抗锯齿
        mPaint.setStrokeWidth(6);//画笔宽度
        mPaint.setStyle(Paint.Style.STROKE);//空心

        mPath = new Path();

        mHolder = getHolder();//获取SurfaceHolder对象
        mHolder.addCallback(this);//注册SurfaceHolder的回调方法
        setFocusable(true);// 能否用键盘获得焦点
        setFocusableInTouchMode(true);//能否通过触摸获得焦点
        this.setKeepScreenOn(true);//屏幕常亮
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        mIsDrawing = true;
        new Thread(this).start();
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {
        mIsDrawing = false;
    }


    public static final int TIME_IN_FRAME = 30;

    @Override
    public void run() {
        long start;
        int x = 0, y = 0;
        drawSth();
        while (mIsDrawing) {
            start = SystemClock.uptimeMillis();//开始绘制
            //绘制sin(x)函数
            x += 1;
            y = (int) (100 * Math.sin(x * 2 * Math.PI / 180) + 400);
            mPath.lineTo(x, y);
            drawSth();
            while (SystemClock.uptimeMillis() - start <= TIME_IN_FRAME) {
                Thread.yield();//线程让出
            }
        }
    }

    private void drawSth() {
        try {
            mCanvas = mHolder.lockCanvas(); //拿到当前画布 然后锁定
            mCanvas.drawColor(Color.GRAY);//画布颜色
            mCanvas.drawPath(mPath, mPaint);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (mCanvas != null) {
                mHolder.unlockCanvasAndPost(mCanvas);//保证每次都将绘图的内容提交
            }
        }
    }


}
