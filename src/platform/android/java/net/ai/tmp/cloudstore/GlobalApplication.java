package net.ai.tmp.cloudstore;

import android.app.Application;
import android.content.res.Resources;

import net.ai.BoolResult;
import net.ai.FileType;
import net.ai.Portal;
import net.ai.tmp.cloudstore.implementation.BoolResultHolder;
import net.ai.tmp.cloudstore.implementation.IBinaryResult;
import net.ai.tmp.cloudstore.implementation.IBoolResult;
import net.ai.tmp.cloudstore.implementation.MainGui;
import net.ai.tmp.cloudstore.implementation.MainOsIo;
import net.ai.tmp.cloudstore.implementation.MainWorker;
import net.ai.tmp.cloudstore.visual.GenericForm;
import net.ai.tmp.cloudstore.visual.Theme;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class GlobalApplication extends Application
{
    private Portal portal = null;
    private final ArrayList<String> allDirectoryNames = new ArrayList<>();
    private final ArrayList<String> allFileNames = new ArrayList<>();
    private final MainGui gui = new MainGui();
    private MainOsIo io;

    private Theme theme;
    public boolean writeStoragePermissionDenied = false;

    public MainOsIo io() { return io; }
    public GenericForm getLastForm() { return gui.getLastForm(); }
    public MainGui.ActiveScreenType activeScreenType()
    {
        return gui.activeScreenType();
    }

    public FileType previewType() { return gui.previewType(); }
    public String previewPath() { return gui.previewPath(); }
    public int previewFileId() { return gui.previewFileId(); }
    public String previewFileName() { return gui.previewFileName(); }

    public Theme theme()
    {
        return theme;
    }
    public void fileReadbinary(final String path, final IBinaryResult result)
    {
        io.fileReadbinary(path, result);
    }
    public Portal getPortal() { return portal; }
    public Portal onCreate(Gui activity)
    {
        gui.onCreate(activity);
        if (null == portal)
        {
            File f = getFilesDir();
            String path = f.getAbsolutePath();
            MainWorker worker = new MainWorker();
            io = new MainOsIo(activity.context(), path);
            theme = new Theme(activity.resources());
            portal = Portal.create(worker, path, (int)(40f*theme.dipPx()), io, gui);
        }

        return portal;
    }

    public void onDestroy(Gui activity)
    {
        gui.onDestroy(activity);
    }

    public List<String> allDirectoryNames() { return allDirectoryNames; }
    public List<String> allFileNames() { return allFileNames; }

    public void signingOut()
    {
        gui.signingOut();
    }

    public void copyFile(final String currentPath, final String newPath, IBoolResult result)
    {
        BoolResultHolder holder = new BoolResultHolder(result);
        io.copyFile(currentPath, newPath, holder);
    }
}
