package net.ai.tmp.cloudstore.visual;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.InputType;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import net.ai.Element;
import net.ai.Message;
import net.ai.Status;
import net.ai.tmp.cloudstore.GlobalApplication;
import net.ai.tmp.cloudstore.Gui;

import java.util.ArrayList;
import java.util.HashMap;

public class FormActivity extends AppCompatActivity implements Gui, Form
{
    private GenericForm form;
    private Theme theme;
    private final ArrayList<InputElement> inputs = new ArrayList<>();
    private final ArrayList<InteractiveElement> interactives = new ArrayList<>();

    private static TextView addErrorView(ConstraintLayout layout, Context ctx, final int id, int padding, ConstraintSet set, int widthPx, int topId)
    {
        TextView view = new TextView(ctx);
        layout.addView(view);
        view.setId(id);

        view.setPadding(padding, padding, padding, padding);

        set.constrainWidth(view.getId(), widthPx);
        set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, topId, ConstraintLayout.LayoutParams.BOTTOM, 0);
        set.constrainHeight(view.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
        return view;
    }

    @SuppressLint("ResourceType")
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        GlobalApplication app = (GlobalApplication)getApplicationContext();
        app.onCreate(this);
        form = app.getLastForm();
        form.destination = this;
        Theme theme = app.theme();
        theme.setStatusBarColour(getWindow());
        Context ctx = this;
        ConstraintLayout layout = new ConstraintLayout(this);
        int id = 1;
        layout.setId(id++);

        ConstraintSet set = new ConstraintSet();
        int topId = layout.getId();
        Toolbar toolbar = theme.addToolbar(layout, ctx, id++, set, topId);
        toolbar.setTitle(form.title);
        topId = toolbar.getId();

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);

        float px = theme.dipPx();
        int padding = theme.padding();
        int buttonWidth = theme.buttonWidth();
        int widthPx = Math.min((int)(px*(float)form.width), displayMetrics.widthPixels);

        for (int i = 0; i < form.elements.size(); ++i)
        {
            final Element el = form.elements.get(i);
            switch (el.getType())
            {
                case BUTTON:
                {
                    Button view = new Button(ctx);
                    TextView errorView = addErrorView(layout, ctx, id++, padding, set, widthPx, topId);
                    interactives.add(new ButtonElement(el, errorView, view));
                    view.setId(id++);
                    set.centerHorizontally(view.getId(),layout.getId());
                    set.constrainWidth(view.getId(), buttonWidth);
                    set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, errorView.getId(), ConstraintLayout.LayoutParams.BOTTOM, 0);

                    view.setText(el.getTitle());
                    view.setOnClickListener(new View.OnClickListener()
                    {
                        @Override
                        public void onClick(View v)
                        {
                            for (int i = 0; i < interactives.size(); ++i)
                            {
                                InteractiveElement el = interactives.get(i);
                                el.message("");
                            }

                            form.action.onResult(el.getInputId());

                        }
                    });
                    layout.addView(view);
                    view.setBackgroundColor(theme.buttonBackgroundColour());
                    view.setTextColor(theme.buttonTextColour() );
                    view.setPadding(padding, 0, padding, 0);
                    topId = view.getId();
                    break;
                }
                case BOOL_INPUT:
                {
                    CheckBox view = new CheckBox(ctx);
                    TextView errorView = addErrorView(layout, ctx, id++, padding, set, widthPx, topId);
                    interactives.add(new BooleanElement(el, errorView, view));
                    inputs.add(new BooleanInputElement(el, view));
                    view.setId(id++);
                    view.setPadding(padding, padding, padding, padding);
                    view.setTextColor(0xff000000);

                    set.constrainWidth(view.getId(), widthPx);
                    set.centerHorizontally(view.getId(),layout.getId());
                    set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, errorView.getId(), ConstraintLayout.LayoutParams.BOTTOM, 0);
                    set.constrainHeight(view.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);

                    layout.addView(view);
                    view.setText(el.getTitle());
                    view.setChecked("true".equals(el.getValue()));
                    topId = view.getId();

                    break;
                }
                case LABEL:
                {

                    TextView view = new TextView(ctx);
                    view.setId(id++);

                    view.setPadding(padding, padding, padding, padding);
                    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1)
                    {
                        view.setTextAlignment(TextView.TEXT_ALIGNMENT_CENTER);
                    }
                    set.constrainWidth(view.getId(), widthPx);
                    set.centerHorizontally(view.getId(),layout.getId());
                    set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, topId, ConstraintLayout.LayoutParams.BOTTOM, 0);
                    set.constrainHeight(view.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);

                    layout.addView(view);
                    view.setText(el.getTitle());
                    view.setTextSize(20);
                    view.setTextColor(theme.labelColour());

                    topId = view.getId();
                    break;
                }
                case TEXT:
                {
                    TextView view = new TextView(ctx);
                    view.setId(id++);

                    view.setPadding(padding, padding, padding, padding);

                    set.constrainWidth(view.getId(), widthPx);
                    set.centerHorizontally(view.getId(),layout.getId());
                    set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, topId, ConstraintLayout.LayoutParams.BOTTOM, 0);
                    set.constrainHeight(view.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);

                    layout.addView(view);
                    view.setText(el.getTitle());
                    view.setTextSize(14);

                    topId = view.getId();
                    break;
                }
                default:
                {
                    EditText view = new EditText(ctx);
                    TextView errorView = addErrorView(layout, ctx, id++, padding, set, widthPx, topId);
                    interactives.add(new TextElement(el, errorView, view));
                    inputs.add(new TextInputElement(el, view));
                    view.setId(id++);
                    view.setPadding(padding, padding, padding, padding);
                    view.setTextColor(theme.textColour());
                    switch (el.getType())
                    {
                        case EMAIL_INPUT: view.setInputType(InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS); break;
                        case PASSWORD_INPUT: view.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD); break;
                    }
                    view.setText(el.getValue());

                    set.constrainWidth(view.getId(), widthPx);
                    set.centerHorizontally(view.getId(),layout.getId());
                    set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, errorView.getId(), ConstraintLayout.LayoutParams.BOTTOM, 0);
                    set.constrainHeight(view.getId(), (int)(44f*px));

                    layout.addView(view);
                    view.setHint(el.getTitle());
                    topId = view.getId();
                    break;
                }
            }
        }

        set.applyTo(layout);

        ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        setContentView(layout, args);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        if (this == form.destination)
            form.destination = null;
        GlobalApplication app = (GlobalApplication)getApplicationContext();
        app.onDestroy(this);
    }

    @Override
    public void showCloudstore()
    {
        Intent intent = new Intent(this, DirectoryActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_NEW_TASK);

        startActivity(intent);
        finish();
    }

    @Override
    public void createForm(boolean clearBackstack)
    {

    }

    @Override
    public void preview()
    {

    }

    @Override
    public Resources resources()
    {
        return getResources();
    }

    @Override
    public void showMessage(String message)
    {
    }

    @Override
    public Context context() {
        return this;
    }

    @Override
    public void actionResult(final Status outcome, final ArrayList<Message> lines)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (Status.SUCCESS == outcome)
                {
                    return;
                }
                if (Status.FAIL == outcome)
                {
                    for (int m = 0; m < lines.size(); ++m)
                    {
                        Message message = lines.get(m);
                        for (int i = 0; i < interactives.size(); ++i)
                        {
                            InteractiveElement el = interactives.get(i);
                            if (el.fieldId() == message.getField())
                                el.message(message.getContent());
                        }
                    }
                    return;
                }
            }
        });
    }

    @Override
    public void progress(int position, int total) {

    }

    @Override
    public void enable(int group)
    {
        for (int i = 0; i < interactives.size(); ++i)
        {
            InteractiveElement el = interactives.get(i);
            if (el.groupId() == group)
                el.enable(true);
        }

    }

    @Override
    public void disable(int group)
    {
        for (int i = 0; i < interactives.size(); ++i)
        {
            InteractiveElement el = interactives.get(i);
            if (el.groupId() == group)
                el.enable(false);
        }

    }

    @Override
    public HashMap<Integer, String> input()
    {
        HashMap<Integer, String> r = new HashMap<>();
        for (int i = 0; i < inputs.size(); ++i)
        {
            InputElement el = inputs.get(i);
            r.put(el.inputId(), el.value());
        }
        return r;
    }
}
