package net.ai.tmp.cloudstore.visual;

import android.content.Context;
import android.graphics.Color;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import net.ai.Portal;
import net.ai.tmp.cloudstore.GlobalApplication;

public class SettingsActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        final GlobalApplication app = (GlobalApplication)getApplicationContext();
        final Portal portal = app.getPortal();
        Theme theme = app.theme();
        theme.setStatusBarColour(getWindow());

        Context ctx = this;
        ConstraintLayout layout = new ConstraintLayout(ctx);
        int id = 1;
        layout.setId(id++);

        ConstraintSet set = new ConstraintSet();
        int topId = layout.getId();
        Toolbar toolbar = theme.addToolbar(layout, ctx, id++, set, topId);
        toolbar.setTitle("Settings");
        topId = toolbar.getId();

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);

        float px = theme.dipPx();
        int padding = theme.padding();
        int buttonWidth = theme.buttonWidth();
        int widthPx = Math.min((int)(400f*px), displayMetrics.widthPixels);

        LinearLayout panel = new LinearLayout(ctx);
        panel.setId(id++);
        layout.addView(panel);
        set.constrainHeight(panel.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
        set.constrainWidth(panel.getId(), widthPx);
        set.connect(panel.getId(), ConstraintLayout.LayoutParams.TOP, topId, ConstraintLayout.LayoutParams.BOTTOM);
        set.connect(panel.getId(), ConstraintLayout.LayoutParams.START, layout.getId(), ConstraintLayout.LayoutParams.START);
        set.connect(panel.getId(), ConstraintLayout.LayoutParams.END, layout.getId(), ConstraintLayout.LayoutParams.END);
        set.connect(panel.getId(), ConstraintLayout.LayoutParams.BOTTOM, layout.getId(), ConstraintLayout.LayoutParams.BOTTOM);
        panel.setOrientation(LinearLayout.VERTICAL);
        panel.setHorizontalGravity(Gravity.CENTER);
        final SettingsActivity self = this;
        {
            Button view = new Button(ctx);
            panel.addView(view, buttonWidth, LinearLayout.LayoutParams.WRAP_CONTENT);
            view.setText("Cancel network operations");
            view.setOnClickListener(new View.OnClickListener()
            {
                @Override
                public void onClick(View v)
                {
                    portal.cancelNetworkOperations();
                    self.finish();

                }
            });
            view.setBackgroundColor(theme.buttonBackgroundColour());
            view.setTextColor(theme.buttonTextColour() );
            view.setPadding(padding, 0, padding, 0);
        }
        {
            ImageView divider = new ImageView(this);
            panel.addView(divider, widthPx, padding);
        }
        {
            Button view = new Button(ctx);
            panel.addView(view, buttonWidth, LinearLayout.LayoutParams.WRAP_CONTENT);
            view.setText("Sign out");
            view.setOnClickListener(new View.OnClickListener()
            {
                @Override
                public void onClick(View v)
                {
                    app.signingOut();
                    portal.signOut();
                    self.finish();
                }
            });
            view.setBackgroundColor(theme.buttonBackgroundColour());
            view.setTextColor(theme.buttonTextColour() );
            view.setPadding(padding, 0, padding, 0);
        }

        {
            set.applyTo(layout);
            ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
            layout.setLayoutParams(args);
        }

        ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        setContentView(layout, args);
    }
}
