package net.ai.tmp.cloudstore.visual;

import android.content.Context;
import android.media.MediaPlayer;
import android.util.AttributeSet;
import android.widget.VideoView;

public class MyVideoView extends VideoView
{
    public MyVideoView(Context context)
    {
        super(context);
    }

    public MyVideoView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public MyVideoView(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }

    @Override
    public void setOnPreparedListener(MediaPlayer.OnPreparedListener l) {
        super.setOnPreparedListener(l);
    }
}
