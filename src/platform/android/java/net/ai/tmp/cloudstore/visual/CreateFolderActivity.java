package net.ai.tmp.cloudstore.visual;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.InputType;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import net.ai.Message;
import net.ai.Portal;
import net.ai.Status;
import net.ai.tmp.cloudstore.GlobalApplication;

import java.util.ArrayList;
import java.util.HashMap;

public class CreateFolderActivity extends AppCompatActivity implements Form
{
    private int directoryId = 0;
    private String title;
    private GenericForm form;
    private TextView errorView;
    private EditText editView;
    private Button button;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (null != savedInstanceState)
        {
            directoryId = savedInstanceState.getInt("directoryId");
            title = savedInstanceState.getString("title");
        }
        else
        {
            Intent intent = getIntent();
            if (null != intent)
            {
                Bundle args = intent.getExtras();
                if (null != args)
                {
                    directoryId = args.getInt("directoryId", 0);
                    title = args.getString("title");
                }
            }
        }

        final GlobalApplication app = (GlobalApplication)getApplicationContext();
        form = new GenericForm(title, true, 0, 0, null, null);
        form.destination = this;
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
        toolbar.setTitle("Create Folder");
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
        final CreateFolderActivity self = this;


        errorView = new TextView(ctx);
        panel.addView(errorView);
        errorView.setId(id++);
        errorView.setTextColor(theme.errorColour());
        errorView.setPadding(padding, padding, padding, padding);

        editView = new EditText(ctx);
        panel.addView(editView);
        editView.setId(id++);
        editView.setPadding(padding, padding, padding, padding);
        editView.setTextColor(theme.textColour());
        editView.setSingleLine(true);

        editView.setHint("Folder name");


        {
            ImageView divider = new ImageView(this);
            panel.addView(divider, widthPx, padding);
        }

        {
            button = new Button(ctx);
            panel.addView(button, buttonWidth, LinearLayout.LayoutParams.WRAP_CONTENT);
            button.setText("Create Folder");
            button.setOnClickListener(new View.OnClickListener()
            {
                @Override
                public void onClick(View v)
                {
                    errorView.setText("");
                    portal.createFolder(directoryId, editView.getText().toString(), form);
                }
            });
            button.setBackgroundColor(theme.buttonBackgroundColour());
            button.setTextColor(theme.buttonTextColour() );
            button.setPadding(padding, 0, padding, 0);
        }

        {
            set.applyTo(layout);
            ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
            layout.setLayoutParams(args);
        }

        ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        setContentView(layout, args);
    }

    @Override
    public void actionResult(final Status outcome, final ArrayList<Message> lines)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (Status.SUCCESS == outcome)
                {
                    finish();
                    return;
                }
                String s = "";
                for (Message msg : lines)
                {
                    s += msg.getContent() + "\n";
                }
                errorView.setText(s);
            }
        });
    }

    @Override
    public void progress(int position, int total)
    {

    }

    @Override
    public void enable(int group)
    {
        editView.setEnabled(true);
        button.setEnabled(true);
    }

    @Override
    public void disable(int group)
    {
        editView.setEnabled(false);
        button.setEnabled(false);
    }

    @Override
    public HashMap<Integer, String> input() {
        return null;
    }
}
