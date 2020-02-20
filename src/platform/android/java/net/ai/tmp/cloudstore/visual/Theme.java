package net.ai.tmp.cloudstore.visual;

import android.app.Activity;
import android.content.Context;
import android.content.res.ColorStateList;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.Window;
import android.view.WindowManager;

public class Theme
{
    private final int darkerBlue = 0xff4AAEFF;
    private final int purple = 0xff9843D2;
    private final int lightBlue = 0xffD7EDFF;
    private final int dirtyBlue = 0xff7CABC9;
    private final int darkGrey = 0xff030303;
    private final int lightGrey = 0xffe0e0e0;
    private final int red = 0xffff0000;
    private final float dipPx;
    private final Resources resources;

    public Theme(Resources resources)
    {
        dipPx = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP,1, resources.getDisplayMetrics());
        this.resources = resources;
    }

    public ColorStateList downloadButtonColours()
    {
        int[][] states = new int[][]
                {
                        new int[] { android.R.attr.state_enabled} // enabled
                };

        int[] colors = new int[]
                {
                        purple
                };

        return new ColorStateList(states, colors);
    }
    public ColorStateList menuColours()
    {
        int[][] states = new int[][]
                {
                        new int[] { android.R.attr.state_enabled} // enabled
                };

        int[] colors = new int[]
                {
                        darkGrey
                };

        return new ColorStateList(states, colors);

    }
    public ColorStateList deleteButtonColours()
    {
        int[][] states = new int[][]
                {
                        new int[] { android.R.attr.state_enabled} // enabled
                };

        int[] colors = new int[]
                {
                        Color.RED
                };

        return new ColorStateList(states, colors);
    }
    public float dipPx() { return dipPx; }
    public int padding() { return (int)(5f*dipPx); }
    public int buttonWidth() { return (int)(300f * dipPx); }
    public int buttonMargin() { return (int)(16f*dipPx); }
    public int deleteButtonRightOffset() { return (int)(88f*dipPx); }

    public void setStatusBarColour(Window window)
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
        {
            window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.setStatusBarColor(darkerBlue);
        }
    }

    public int actionBarSize(Context ctx)
    {
        final TypedArray styledAttributes = ctx.getTheme().obtainStyledAttributes(new int[] { android.R.attr.actionBarSize });
        return (int) styledAttributes.getDimension(0, 0);
    }

    public Toolbar addToolbar(ConstraintLayout layout, Context ctx, final int id, ConstraintSet set, int topId)
    {
        Toolbar view = new Toolbar(ctx);
        layout.addView(view);
        view.setId(id);

        set.constrainWidth(view.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);
        set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, topId, ConstraintLayout.LayoutParams.TOP, 0);
        set.constrainHeight(view.getId(), actionBarSize(ctx));

        view.setBackgroundColor(lightBlue);
        view.setTitleTextColor(darkerBlue);
        return view;
    }

    public int buttonBackgroundColour() { return dirtyBlue; }
    public int buttonTextColour() { return 0xffffffff; }
    public int labelColour() { return purple; }
    public int textAccentColour() { return purple; }

    public Drawable createThumbnail(byte[] b)
    {
        return new BitmapDrawable(resources,BitmapFactory.decodeByteArray(b, 0, b.length));
    }

    public int gridSpanCount(Activity activity)
    {
        DisplayMetrics displayMetrics = new DisplayMetrics();
        activity.getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        return Math.max((int)(displayMetrics.widthPixels/Math.floor(dipPx*300f)), 1);
    }

    public int textColour()
    {
        return darkGrey;
    }

    public int errorColour()
    {
        return red;
    }
}
