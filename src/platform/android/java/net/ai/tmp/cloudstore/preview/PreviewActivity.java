package net.ai.tmp.cloudstore.preview;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.support.annotation.Nullable;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageView;
import android.widget.MediaController;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.VideoView;

import net.ai.FileType;
import net.ai.tmp.cloudstore.GlobalApplication;
import net.ai.tmp.cloudstore.R;
import net.ai.tmp.cloudstore.Util;
import net.ai.tmp.cloudstore.implementation.IBinaryResult;
import net.ai.tmp.cloudstore.implementation.IBoolResult;
import net.ai.tmp.cloudstore.implementation.IStringResult;
import net.ai.tmp.cloudstore.visual.MyVideoView;
import net.ai.tmp.cloudstore.visual.Theme;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;

public class PreviewActivity extends AppCompatActivity implements SurfaceHolder.Callback, MediaPlayer.OnPreparedListener {
    FloatingActionButton deleteButton;
    FloatingActionButton saveButton;
    GlobalApplication app;

    private MediaPlayer player;

    private SurfaceView surfaceViewFrame;
    private SurfaceHolder holder;

    @SuppressLint("ResourceType")
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        app = (GlobalApplication)getApplicationContext();
        Theme theme = app.theme();
        theme.setStatusBarColour(getWindow());
        final Context ctx = this;

        ConstraintLayout layout = new ConstraintLayout(this);
        int id = 1;
        layout.setId(id++);
        final PreviewActivity self = this;


        ConstraintSet set = new ConstraintSet();

        Toolbar toolbar = theme.addToolbar(layout, this, id++, set, layout.getId());
        toolbar.setTitle(app.previewFileName());
        setSupportActionBar(toolbar);

        DisplayMetrics displayMetrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
        float px = theme.dipPx();
        int padding = theme.padding();
        int widthPx = Math.min((int)(px*400.0), displayMetrics.widthPixels);

        switch (app.previewType())
        {
            case IMAGE:
            {
                final ImageView view = new ImageView(ctx);
                view.setId(id++);
                layout.addView(view);
                app.fileReadbinary(app.previewPath(), new IBinaryResult()
                {
                    @Override
                    public void onResult(byte[] data)
                    {
                        if (null == data || 0 == data.length)
                            return;
                        final Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
                        self.runOnUiThread(new Runnable()
                        {
                            @Override
                            public void run()
                            {
                                view.setImageBitmap(bmp);
                            }
                        });
                    }
                });

                set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, toolbar.getId(), ConstraintLayout.LayoutParams.BOTTOM);
                set.connect(view.getId(), ConstraintLayout.LayoutParams.BOTTOM, layout.getId(), ConstraintLayout.LayoutParams.BOTTOM);
                set.constrainWidth(view.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);

                break;
            }
            case VIDEO:
            {
                SurfaceView view = new SurfaceView(ctx);
                view.setId(id++);
                layout.addView(view);

                view.setClickable(false);

                surfaceViewFrame = view;
                holder = surfaceViewFrame.getHolder();
                holder.addCallback(self);
                holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

                set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, toolbar.getId(), ConstraintLayout.LayoutParams.BOTTOM);
                set.connect(view.getId(), ConstraintLayout.LayoutParams.BOTTOM, layout.getId(), ConstraintLayout.LayoutParams.BOTTOM);
                set.connect(view.getId(), ConstraintLayout.LayoutParams.LEFT, layout.getId(), ConstraintLayout.LayoutParams.LEFT);
                set.connect(view.getId(), ConstraintLayout.LayoutParams.RIGHT, layout.getId(), ConstraintLayout.LayoutParams.RIGHT);
                break;
            }
            default:
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
                set.connect(view.getId(), ConstraintLayout.LayoutParams.TOP, toolbar.getId(), ConstraintLayout.LayoutParams.BOTTOM, 0);
                set.constrainHeight(view.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);

                layout.addView(view);
                view.setText("This file cannot be previewed in the app. You may want to save it to your library and work with it from there.");
                view.setTextSize(20);
                view.setTextColor(theme.textColour());
                break;
            }
        }

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
                                    ArrayList<Integer> fileIds = new ArrayList<>();
                                    fileIds.add(app.previewFileId());
                                    app.getPortal().deleteAll(new ArrayList<Integer>(), fileIds);
                                    finish();
                                }
                            })


                            .setNegativeButton("No", null).show();



                }
            });
            deleteButton.show();
        }

        if (!app.writeStoragePermissionDenied)
        {
            saveButton = new FloatingActionButton(ctx);
            saveButton.setId(id++);
            layout.addView(saveButton);
            set.connect(saveButton.getId(), ConstraintLayout.LayoutParams.BOTTOM, layout.getId(), ConstraintLayout.LayoutParams.BOTTOM, theme.buttonMargin());
            set.connect(saveButton.getId(), ConstraintLayout.LayoutParams.RIGHT, layout.getId(), ConstraintLayout.LayoutParams.RIGHT, theme.buttonMargin());
            set.constrainWidth(saveButton.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
            set.constrainHeight(saveButton.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);
            saveButton.setImageDrawable(ContextCompat.getDrawable(ctx, R.drawable.ic_save_white_24dp));
            saveButton.setBackgroundTintList(theme.downloadButtonColours());
            saveButton.setOnClickListener(new View.OnClickListener()
            {
                @Override
                public void onClick(View view)
                {
                    if (app.writeStoragePermissionDenied)
                        return;
                    if (Util.verifyStoragePermissions(self, PERMISSION_WRITE_STORAGE))
                        savePreviewFile();

                }
            });
            saveButton.show();
        }

        set.applyTo(layout);
        ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        setContentView(layout, args);
    }

    private static final int PERMISSION_WRITE_STORAGE = 1;

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode)
        {
            case PERMISSION_WRITE_STORAGE:
            {
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)
                {
                    savePreviewFile();
                }
                else
                {
                    app.writeStoragePermissionDenied = true;
                    saveButton.hide();
                }
                return;
            }
        }
    }

    private static String directoryNameFor(FileType type)
    {
        switch (type)
        {
            case IMAGE: return "pictures";
            case VIDEO: return "movies";
            case TEXT:
            {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
                    return "documents";

                return "downloads";
            }
            default: return "downloads";
        }
    }

    private static String directoryFor(FileType type)
    {
        switch (type)
        {
            case IMAGE: return Environment.DIRECTORY_PICTURES;
            case VIDEO: return Environment.DIRECTORY_MOVIES;
            case TEXT:
            {
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
                    return Environment.DIRECTORY_DOCUMENTS;

                return Environment.DIRECTORY_DOWNLOADS;
            }
            default: return Environment.DIRECTORY_DOWNLOADS;
        }
    }

    private void savePreviewFile()
    {
        File file = new File(Environment.getExternalStoragePublicDirectory(directoryFor(app.previewType())), app.previewFileName());
        final String currentPhotoPath = file.getAbsolutePath();
        final PreviewActivity self = this;
        app.io().copyFile(app.previewPath(), currentPhotoPath, new IStringResult()
        {
            @Override
            public void onResult(final String error)
            {
                self.runOnUiThread(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        if (error.isEmpty())
                        {
                            Intent mediaScanIntent = new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE);
                            File f = new File(currentPhotoPath);
                            Uri contentUri = Uri.fromFile(f);
                            mediaScanIntent.setData(contentUri);
                            self.sendBroadcast(mediaScanIntent);
                            Toast toast = Toast.makeText(self, "File copied to " + directoryNameFor(app.previewType()), Toast.LENGTH_SHORT);
                            toast.show();
                            finish();
                        }
                        else
                        {
                            Toast toast = Toast.makeText(self, "Failed to copy file: " + error, Toast.LENGTH_SHORT);
                            toast.show();
                        }
                    }
                });
            }
        });

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId())
        {
            case R.id.action_delete_file:
            {
                GlobalApplication app = (GlobalApplication)getApplicationContext();

                ArrayList<Integer> fileIds = new ArrayList<>();
                fileIds.add(app.previewFileId());
                app.getPortal().deleteAll(new ArrayList<Integer>(), fileIds);
                finish();
                return true;
            }
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        final PreviewActivity self = this;
        new Thread(new Runnable()
        {
            public void run()
            {
                try
                {
                    player = new MediaPlayer();
                    player.setOnPreparedListener(self);
                    player.setScreenOnWhilePlaying(true);
                    player.setDisplay(holder);

                    player.setDataSource(app.previewPath());
                    player.prepare();
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                }
            }
        }).start();

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void onPrepared(MediaPlayer mediaPlayer)
    {

        int videoWidth = player.getVideoWidth();
        int videoHeight = player.getVideoHeight();
        float videoProportion = (float) videoWidth / (float) videoHeight;
        int screenWidth = surfaceViewFrame.getWidth();
        int screenHeight = surfaceViewFrame.getHeight();

        float screenProportion = (float) screenWidth / (float) screenHeight;
        android.view.ViewGroup.LayoutParams lp = surfaceViewFrame.getLayoutParams();

        if (videoProportion > screenProportion)
        {
            lp.width = screenWidth;
            lp.height = (int) ((float) screenWidth / videoProportion);
        }
        else
        {
            lp.width = (int) (videoProportion * (float) screenHeight);
            lp.height = screenHeight;
        }
        surfaceViewFrame.setLayoutParams(lp);

        if (!player.isPlaying())
        {
            player.start();
        }
        surfaceViewFrame.setClickable(true);

    }

    @Override
    protected void onStop() {
        super.onStop();
        if (null != player && player.isPlaying())
        {
            player.stop();
        }
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();
    }
}
