package net.ai.tmp.cloudstore;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.design.widget.NavigationView;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

import net.ai.Portal;
import net.ai.tmp.cloudstore.preview.PreviewActivity;
import net.ai.tmp.cloudstore.visual.DirectoryActivity;
import net.ai.tmp.cloudstore.visual.FormActivity;
import net.ai.tmp.cloudstore.visual.SettingsActivity;
import net.ai.tmp.cloudstore.visual.Theme;

public class ContainerActivity extends AppCompatActivity implements Gui
{
    static
    {
        System.loadLibrary("cloudstore");
    }

    private Portal portal;

    @SuppressLint("ResourceType")
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        GlobalApplication app = (GlobalApplication)getApplicationContext();
        portal = app.onCreate(this);
        Theme theme = app.theme();
        theme.setStatusBarColour(getWindow());

        final DrawerLayout drawer = new DrawerLayout(this);
        drawer.setFitsSystemWindows(true);

        ConstraintLayout layout = new ConstraintLayout(this);
        layout.setLayoutParams(new DrawerLayout.LayoutParams(DrawerLayout.LayoutParams.MATCH_PARENT, DrawerLayout.LayoutParams.MATCH_PARENT));
        drawer.addView(layout);

        int id = 1;
        layout.setId(id++);

        {
            ConstraintSet set = new ConstraintSet();

            set.constrainWidth(layout.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);
            set.constrainHeight(layout.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);

            Toolbar toolbar = theme.addToolbar(layout, this, id++, set, layout.getId());
            toolbar.setTitle("Blomp");
            setSupportActionBar(toolbar);

            set.applyTo(layout);
        }

        {
            NavigationView nav = new NavigationView(this);
            nav.setId(id++);
            DrawerLayout.LayoutParams lp = new DrawerLayout.LayoutParams(DrawerLayout.LayoutParams.WRAP_CONTENT, DrawerLayout.LayoutParams.MATCH_PARENT);
            lp.gravity = Gravity.START;
            drawer.addView(nav, lp);
            Menu menu = nav.getMenu();

            {
                MenuItem aboutMenu = menu.add(1,Menu.NONE,Menu.NONE,"Settings");
                nav.setItemTextColor(theme.menuColours());
                aboutMenu.setIcon(R.drawable.ic_settings_black_24dp);
                final ContainerActivity self = this;
                aboutMenu.setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {
                        drawer.closeDrawers();
                        startActivity(new Intent(self, SettingsActivity.class));
                        return true;
                    }
                });
            }

            {
                MenuItem aboutMenu = menu.add(1,Menu.NONE,Menu.NONE,"About");
                nav.setItemTextColor(theme.menuColours());
                aboutMenu.setIcon(R.drawable.ic_info_outline_black_24dp);
                aboutMenu.setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {
                        portal.about();
                        drawer.closeDrawers();
                        return true;
                    }
                });
            }
        }


        switch (app.activeScreenType())
        {
            case Form:
            {
                createForm(false);
                break;
            }
            case Directory:
            {
                showCloudstore();
                break;
            }
            case Preview:
            {
                preview();
                break;
            }
            default: break;
        }

        ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        setContentView(drawer, args);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        GlobalApplication app = (GlobalApplication)getApplicationContext();
        portal = null;
        app.onDestroy(this);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();
    }

    @SuppressLint("ResourceType")
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
        Intent intent = new Intent(this, FormActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(intent);
        finish();
    }

    @Override
    public void preview()
    {
        final Intent intent = new Intent(this, PreviewActivity.class);
        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                startActivity(intent);
            }
        });
    }

    @Override
    public Resources resources()
    {
        return getResources();
    }

    @Override
    public void showMessage(String message)
    {
        Toast toast = Toast.makeText(this, message, Toast.LENGTH_SHORT);
        toast.show();
    }

    @Override
    public Context context() {
        return this;
    }

}
