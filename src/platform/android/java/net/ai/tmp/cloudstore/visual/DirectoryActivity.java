package net.ai.tmp.cloudstore.visual;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.NavigationView;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import net.ai.DirectoriesDetail;
import net.ai.FileState;
import net.ai.FilesDetail;
import net.ai.Portal;
import net.ai.tmp.cloudstore.ContainerActivity;
import net.ai.tmp.cloudstore.DirectoryInterface;
import net.ai.tmp.cloudstore.DirectoryProxy;
import net.ai.tmp.cloudstore.GlobalApplication;
import net.ai.tmp.cloudstore.Gui;
import net.ai.tmp.cloudstore.R;
import net.ai.tmp.cloudstore.Util;
import net.ai.tmp.cloudstore.directory.DirectoryAction;
import net.ai.tmp.cloudstore.directory.DirectoryAdaptor;
import net.ai.tmp.cloudstore.preview.PreviewActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

public class DirectoryActivity extends AppCompatActivity implements Gui, DirectoryInterface, DirectoryAction, MenuItem.OnMenuItemClickListener
{
    private DirectoryProxy proxy = null;

    private Theme theme;
    private Portal portal;

    private int directoryId = 0;
    private String title;
    private DirectoryAdaptor adapter = null;
    private DrawerLayout drawer;

    FloatingActionButton deleteButton;
    FloatingActionButton downloadButton;
    GlobalApplication app;

    boolean showDelete = false;
    boolean showDownload = false;
    private int selected = 0;
    private MenuItem uploadMenu;
    private MenuItem createFolderMenu;
    private MenuItem clearMenu;
    private MenuItem renameMenu;
    private MenuItem selectAllMenu;
    private ConstraintLayout layout;

    @Override
    public void onBackPressed() {
        if (0 != directoryId)
        {
            super.onBackPressed();
            return;
        }
        moveTaskToBack(true);
        android.os.Process.killProcess(android.os.Process.myPid());
        System.exit(1);
    }

    @SuppressLint("ResourceType")
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState)
    {
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

        if (null == title)
            title = "Blomp";

        app = (GlobalApplication)getApplicationContext();
        app.onCreate(this);
        theme = app.theme();
        theme.setStatusBarColour(getWindow());
        portal = app.getPortal();
        if (null == proxy)
            proxy = new DirectoryProxy(this);
        adapter = new DirectoryAdaptor(portal,this, app.allDirectoryNames(), app.allFileNames(), theme);
        final Context ctx = this;

        drawer = new DrawerLayout(this);
        drawer.setFitsSystemWindows(true);

        layout = new ConstraintLayout(this);
        layout.setLayoutParams(new DrawerLayout.LayoutParams(DrawerLayout.LayoutParams.MATCH_PARENT, DrawerLayout.LayoutParams.MATCH_PARENT));
        drawer.addView(layout);

        int id = 1;
        layout.setId(id++);

        {
            ConstraintSet set = new ConstraintSet();

            Toolbar toolbar = theme.addToolbar(layout, this, id++, set, layout.getId());
            toolbar.setTitle(title);
            setSupportActionBar(toolbar);

            RecyclerView list = new RecyclerView(ctx);
            list.setId(id++);
            layout.addView(list);
            set.constrainWidth(list.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);
            set.connect(list.getId(), ConstraintLayout.LayoutParams.TOP, layout.getId(), ConstraintLayout.LayoutParams.TOP, theme.actionBarSize(ctx));
            set.connect(list.getId(), ConstraintLayout.LayoutParams.BOTTOM, layout.getId(), ConstraintLayout.LayoutParams.BOTTOM, 0);

            list.setLayoutManager(new GridLayoutManager(ctx, theme.gridSpanCount(this)));
            list.setAdapter(adapter);

            {
                deleteButton = new FloatingActionButton(ctx);
                deleteButton.setId(id++);
                layout.addView(deleteButton);
                set.connect(deleteButton.getId(), ConstraintLayout.LayoutParams.BOTTOM, layout.getId(), ConstraintLayout.LayoutParams.BOTTOM, theme.buttonMargin());
                set.connect(deleteButton.getId(), ConstraintLayout.LayoutParams.RIGHT, layout.getId(), ConstraintLayout.LayoutParams.RIGHT, theme.deleteButtonRightOffset());
                set.constrainWidth(deleteButton.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
                set.constrainHeight(deleteButton.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
                deleteButton.setImageDrawable(ContextCompat.getDrawable(ctx, R.drawable.ic_delete_forever_white_24dp));
                deleteButton.setBackgroundTintList(theme.deleteButtonColours());
                deleteButton.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View v)
                    {
                        new AlertDialog.Builder(ctx)
                                .setTitle("Title")
                                .setMessage("Do you really want to delete the selection?")
                                .setIcon(android.R.drawable.ic_dialog_alert)
                                .setPositiveButton("Yes", new DialogInterface.OnClickListener()
                                {

                                    public void onClick(DialogInterface dialog, int whichButton)
                                    {
                                        adapter.deleteAll();
                                        downloadButton.hide();
                                        deleteButton.hide();
                                    }
                                })
                                .setNegativeButton("No", null).show();
                        
                    }
                });
                deleteButton.hide();
            }
            {
                downloadButton = new FloatingActionButton(ctx);
                downloadButton.setId(id++);
                layout.addView(downloadButton);
                set.connect(downloadButton.getId(), ConstraintLayout.LayoutParams.BOTTOM, layout.getId(), ConstraintLayout.LayoutParams.BOTTOM, theme.buttonMargin());
                set.connect(downloadButton.getId(), ConstraintLayout.LayoutParams.RIGHT, layout.getId(), ConstraintLayout.LayoutParams.RIGHT, theme.buttonMargin());
                set.constrainWidth(downloadButton.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
                set.constrainHeight(downloadButton.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
                downloadButton.setImageDrawable(ContextCompat.getDrawable(ctx, R.drawable.ic_cloud_download_white_24dp));
                downloadButton.setBackgroundTintList(theme.downloadButtonColours());
                downloadButton.setOnClickListener(new View.OnClickListener()
                {
                    @Override
                    public void onClick(View view)
                    {
                        adapter.downloadAll();
                        deleteButton.hide();
                        downloadButton.hide();
                    }
                });
                downloadButton.hide();
            }

            set.applyTo(layout);
        }

        {
            NavigationView nav = new NavigationView(this);
            nav.setId(id++);
            DrawerLayout.LayoutParams lp = new DrawerLayout.LayoutParams(DrawerLayout.LayoutParams.WRAP_CONTENT, DrawerLayout.LayoutParams.MATCH_PARENT);
            lp.gravity = Gravity.START;
            drawer.addView(nav, lp);

            {
                final ImageView imageView = new ImageView(this);
                imageView.setId(id++);
                imageView.setImageResource(R.drawable.blomp_banner);
                imageView.setPadding(theme.padding(), 0, theme.padding(), 0);
                nav.addHeaderView(imageView);

                ActionBar actionbar = getSupportActionBar();
                actionbar.setDisplayHomeAsUpEnabled(true);
                Drawable img = getResources().getDrawable(R.drawable.ic_menu_black_24dp);
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
                {
                    img.setTintList(theme.downloadButtonColours());
                }

                actionbar.setHomeAsUpIndicator(img);
            }

            Menu menu = nav.getMenu();

            {
                MenuItem item = menu.add(1,Menu.NONE,Menu.NONE,"Settings");
                nav.setItemTextColor(theme.menuColours());
                item.setIcon(R.drawable.ic_settings_black_24dp);
                final DirectoryActivity self = this;
                item.setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {
                        drawer.closeDrawers();
                        startActivity(new Intent(self, SettingsActivity.class));
                        return true;
                    }
                });
            }

            if (getPackageManager().hasSystemFeature(PackageManager.FEATURE_TELEPHONY))
            {
                MenuItem item = menu.add(1,Menu.NONE,Menu.NONE,"Invite Message");
                nav.setItemTextColor(theme.menuColours());
                item.setIcon(R.drawable.ic_share);
                final DirectoryActivity self = this;
                item.setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {
                        drawer.closeDrawers();
                        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse("sms:"));
                        intent.putExtra("sms_body", "Try Blomp for free cloud storage! Click https://sms.blomp.com");
                        startActivity(intent);
                        return true;
                    }
                });
            }

            {
                MenuItem item = menu.add(1,Menu.NONE,Menu.NONE,"About");
                nav.setItemTextColor(theme.menuColours());
                item.setIcon(R.drawable.ic_info_outline_black_24dp);
                item.setOnMenuItemClickListener(new MenuItem.OnMenuItemClickListener() {
                    @Override
                    public boolean onMenuItemClick(MenuItem menuItem) {
                        portal.about();
                        drawer.closeDrawers();
                        return true;
                    }
                });
            }
        }

        ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        setContentView(drawer, args);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {
            case android.R.id.home:
                drawer.openDrawer(GravityCompat.START);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        portal.watchDirectory(directoryId, proxy);
        if (showDelete)
            deleteButton.show();
        else
            deleteButton.hide();
        if (showDownload)
            downloadButton.show();
        else
            downloadButton.hide();
    }

    @Override
    protected void onStop()
    {
        super.onStop();
        portal.unwatchDirectory(directoryId);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        outState.putInt("directoryId", directoryId);
        outState.putString("title", title);
    }

    private static final int UploadMenuId = 1;
    private static final int ClearMenuId = 2;
    private static final int SelectAllMenuId = 3;
    private static final int CreateFolderMenuId = 4;
    private static final int RenameMenuId = 5;

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        uploadMenu = menu.add(Menu.NONE, UploadMenuId, Menu.NONE, "Upload");
        uploadMenu.setIcon(R.drawable.ic_cloud_upload_black_24dp);
        uploadMenu.setEnabled(true);
        uploadMenu.setVisible(true);
        uploadMenu.setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        uploadMenu.setOnMenuItemClickListener(this);

        clearMenu = menu.add(Menu.NONE, ClearMenuId, Menu.NONE, "Clear Selection");
        clearMenu.setIcon(R.drawable.ic_clear_black_24dp);
        clearMenu.setEnabled(true);
        clearMenu.setVisible(false);
        clearMenu.setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        clearMenu.setOnMenuItemClickListener(this);

        renameMenu = menu.add(Menu.NONE, RenameMenuId, Menu.NONE, "Rename");
        renameMenu.setEnabled(true);
        renameMenu.setVisible(false);
        renameMenu.setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        renameMenu.setOnMenuItemClickListener(this);

        selectAllMenu = menu.add(Menu.NONE, SelectAllMenuId, Menu.NONE, "Select All");
        selectAllMenu.setIcon(R.drawable.ic_done_all_black_24dp);
        selectAllMenu.setEnabled(true);
        selectAllMenu.setVisible(false);
        selectAllMenu.setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        selectAllMenu.setOnMenuItemClickListener(this);

        createFolderMenu = menu.add(Menu.NONE, CreateFolderMenuId, Menu.NONE, "Create Folder");
        createFolderMenu.setIcon(R.drawable.ic_add_black_24dp);
        createFolderMenu.setEnabled(true);
        createFolderMenu.setVisible(true);
        createFolderMenu.setShowAsActionFlags(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        createFolderMenu.setOnMenuItemClickListener(this);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            uploadMenu.setIconTintList(theme.downloadButtonColours());
            clearMenu.setIconTintList(theme.downloadButtonColours());
            selectAllMenu.setIconTintList(theme.downloadButtonColours());
            createFolderMenu.setIconTintList(theme.downloadButtonColours());
        }
        return true;
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        GlobalApplication app = (GlobalApplication)getApplicationContext();
        app.onDestroy(this);
    }

    private int folderIdFromPath(ArrayList<Integer> folderPath)
    {
        return folderPath.isEmpty() ? 0 : folderPath.get(folderPath.size()-1).intValue();
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
        Intent intent = new Intent(this, FormActivity.class);
        if (clearBackstack)
        {
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);
            finish();
        }
        else
            startActivity(intent);
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
    public void showMessage(final String message)
    {
        final DirectoryActivity self = this;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast toast = Toast.makeText(self, message, Toast.LENGTH_SHORT);
                toast.show();

            }
        });
    }

    @Override
    public Context context() {
        return this;
    }

    private static final int PICK_IMAGE_ID = 234; // the number doesn't matter
    @Override
    public boolean onMenuItemClick(MenuItem menuItem)
    {
        switch (menuItem.getItemId())
        {
            case UploadMenuId:
            {
                if
                (
                        ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                                || ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED
                )
                {
                    ActivityCompat.requestPermissions(this, new String[]
                            {
                                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                                    Manifest.permission.READ_EXTERNAL_STORAGE
                            }, PICK_IMAGE_ID);
                    return true;
                }
                Intent getIntent = new Intent(Intent.ACTION_GET_CONTENT);
                getIntent.setType("*/*");
                getIntent.putExtra(Intent.EXTRA_ALLOW_MULTIPLE, true);
                startActivityForResult(getIntent, PICK_IMAGE_ID);
                break;
            }
            case ClearMenuId:
            {
                adapter.clearSelection();
                hasSelection(0);
                break;
            }
            case SelectAllMenuId:
            {
                adapter.selectAll();
                break;
            }
            case CreateFolderMenuId:
            {
                Bundle b = new Bundle();
                b.putInt("directoryId", directoryId);
                b.putString("title", title);

                final Intent intent = new Intent(this, CreateFolderActivity.class);
                intent.putExtras(b);
                startActivity(intent);
                break;
            }
            case RenameMenuId:
            {
                int selectedFileId = adapter.currentSelectedFileId();
                int selectedFolderId = adapter.currentSelectedFolderId();

                if (-1 == selectedFileId ^ -1 == selectedFolderId)
                {
                    adapter.clearSelection();
                    hasSelection(0);

                    Bundle b = new Bundle();
                    b.putInt("directoryId", selectedFolderId);
                    b.putInt("fileId", selectedFileId);

                    b.putString("title", adapter.fileOrFolderName(selectedFileId, selectedFolderId));

                    final Intent intent = new Intent(this, RenameActivity.class);
                    intent.putExtras(b);
                    startActivity(intent);
                }

                break;
            }
        }

        return true;

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, final @Nullable Intent data)
    {
        if (PICK_IMAGE_ID == requestCode && Activity.RESULT_OK == resultCode && null != data)
        {
            final DirectoryActivity self = this;
            new Thread(new Runnable()
            {
                @Override
                public void run()
                {
                    final ContentResolver cr = self.getContentResolver();

                    Uri[] uris = Util.contentUris(data);

                    for (int i = 0; i < uris.length; ++i)
                    {
                        Uri uri = uris[i];

                        String friendlyName = Util.friendlyName(cr, uri);

                        try
                        {
                            String tmpfile;
                            {
                                File outputDir = self.getApplicationContext().getCacheDir(); // context being the Activity pointer
                                File outputFile = File.createTempFile("prefix", "extension", outputDir);
                                tmpfile = outputFile.getAbsolutePath();
                            }

                            Util.io_copy_and_close(self.getContentResolver().openInputStream(uri), new FileOutputStream(tmpfile));

                            ArrayList<String> filenames = new ArrayList<>();
                            filenames.add(friendlyName);
                            ArrayList<String> paths = new ArrayList<>();
                            paths.add(tmpfile);
                            ArrayList<byte[]> thumbnails = new ArrayList<>();

                            thumbnails.add(app.io().thumbnail(tmpfile, Util.fileTypeFromMime(cr.getType(uri))));
                            portal.uploadFilesOnly(directoryId, filenames, paths, thumbnails);
                        }
                        catch (IOException e)
                        {
                            e.printStackTrace();
                        }
                    }
                }
            }).start();
        }

    }

    @Override
    public void directoryRefreshList
    (
        final ArrayList<Integer> folderPath,
        final DirectoriesDetail dirs,
        final FilesDetail files,
        final ArrayList<String> newDirectoryNames,
        final ArrayList<String> newFileNames
    )
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                app.allDirectoryNames().addAll(newDirectoryNames);
                app.allFileNames().addAll(newFileNames);
                if (folderIdFromPath(folderPath) != directoryId)
                    return;
                adapter.update(dirs, files);
            }
        });
    }

    @Override
    public void fileStatesChanged
    (
        ArrayList<Integer> folderPath,
        final ArrayList<Integer> fileIds,
        final ArrayList<FileState> states
    )
    {
        if (folderIdFromPath(folderPath) != directoryId)
            return;
        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                adapter.fileStatesChanged(fileIds, states);
            }
        });
    }

    @Override
    public void directoryFileChanged
    (
        ArrayList<Integer> folderPath,
        final int fileId,
        final int fileNameId,
        final long size,
        final int percentage,
        final FileState state,
        final byte[] thumbnail
    )
    {

        runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                adapter.updateFile(fileId, size, percentage, state, thumbnail);
            }
        });
    }

    @Override
    public void folderClicked(int folderId, String title)
    {
        Bundle b = new Bundle();
        b.putInt("directoryId", folderId);
        b.putString("title", title);

        final Intent intent = new Intent(this, DirectoryActivity.class);
        intent.putExtras(b);
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
    public void hasSelection(int selected)
    {
        this.selected = selected;
        if (0 == selected)
        {
            downloadButton.hide();
            deleteButton.hide();
            uploadMenu.setVisible(true);
            createFolderMenu.setVisible(true);
            clearMenu.setVisible(false);
            selectAllMenu.setVisible(false);
        }
        else
        {
            downloadButton.show();
            deleteButton.show();
            uploadMenu.setVisible(false);
            createFolderMenu.setVisible(false);
            clearMenu.setVisible(true);
            selectAllMenu.setVisible(true);
        }
        renameMenu.setVisible(1 == selected);
    }

    @Override
    public void fileClicked(int fileId)
    {
        portal.preview(fileId);
    }
}
