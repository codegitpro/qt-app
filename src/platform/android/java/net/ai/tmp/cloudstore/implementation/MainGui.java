package net.ai.tmp.cloudstore.implementation;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.net.Uri;
import android.widget.Toast;

import net.ai.Element;
import net.ai.FileType;
import net.ai.IntResult;
import net.ai.OsForm;
import net.ai.OsGui;
import net.ai.VoidResult;
import net.ai.tmp.cloudstore.Gui;
import net.ai.tmp.cloudstore.visual.GenericForm;

import java.util.ArrayList;
import java.util.List;

public final class MainGui extends OsGui
{
    private GenericForm lastForm;

    private FileType previewType;
    private String previewPath;
    private int previewFileId;
    private String previewFileName;
    private boolean signingOut = false;
    private List<Gui> guis = new ArrayList<>();

    public FileType previewType() { return previewType; }
    public String previewPath() { return previewPath; }
    public int previewFileId() { return previewFileId; }
    public String previewFileName() { return previewFileName; }

    public void signingOut()
    {
        signingOut = true;
    }

    public enum ActiveScreenType
    {
        Form,
        Directory,
        Preview,
        None
    }

    public GenericForm getLastForm()
    {
        GenericForm r = lastForm;
        lastForm = null;
        return r;
    }

    public ActiveScreenType activeScreenType()
    {
        if (null != lastForm)
            return ActiveScreenType.Form;
        return ActiveScreenType.None;
    }

    public void onDestroy(Gui activity)
    {
        guis.remove(activity);
    }

    public void onCreate(Gui activity)
    {
        guis.add(activity);
        if (null != lastForm)
        {
            activity.createForm(signingOut);
            signingOut = false;
        }
    }

    @Override
    public void showCloudstore()
    {
        if (guis.isEmpty())
            return;

        Gui handover = guis.get(guis.size()-1);
        handover.showCloudstore();
    }

    @Override
    public void guiThread(final VoidResult cb)
    {
        if (guis.isEmpty())
            return;

        Gui handover = guis.get(guis.size()-1);
        handover.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                cb.onResult();
            }
        });
    }

    @Override
    public void back()
    {

    }

    @Override
    public void openLinkInBrowser(String url)
    {
        if (guis.isEmpty())
            return;
        Gui handover = guis.get(guis.size()-1);
        try
        {
            Intent myIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
            handover.startActivity(myIntent);
        }
        catch (ActivityNotFoundException e)
        {
            e.printStackTrace();
        }

    }

    @Override
    public OsForm createForm(String title, boolean startHierarchy, int width, int height, IntResult action, ArrayList<Element> elements)
    {
        lastForm = new GenericForm(title, startHierarchy, width, height, action, elements);

        if (!guis.isEmpty())
        {
            Gui handover = guis.get(guis.size()-1);
            handover.createForm(signingOut);
            signingOut = false;
        }

        return lastForm;
    }

    @Override
    public void showMessage(String message)
    {
        Gui handover = guis.get(guis.size()-1);
        handover.showMessage(message);
    }

    @Override
    public void preview(FileType type, String path, int fileId, String fileName)
    {
        previewType = type;
        previewPath = path;
        previewFileId = fileId;
        previewFileName = fileName;

        if (guis.isEmpty())
            return;

        Gui handover = guis.get(guis.size()-1);
        handover.preview();
    }

    @Override
    public void reset()
    {

    }
}
