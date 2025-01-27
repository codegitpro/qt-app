// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from portal.djinni

package net.ai;

import java.util.ArrayList;

public abstract class FolderView {
    public abstract void createFolderView(FolderViewmodel folderVm, FormViewmodel formVm);

    public abstract void createPhotoPreview(PreViewmodel vm, String fileName, String filePath);

    public abstract void createVideoPreview(PreViewmodel vm, String fileName, String filePath);

    public abstract void createTextPreview(PreViewmodel vm, String fileName, String filePath);

    public abstract void reload(int section, int row);

    public abstract void onReset(ArrayList<FolderItemDetail> folders, ArrayList<FileItemDetail> files);

    public abstract void onFileStatesChange(ArrayList<StateChange> stateChanges);

    public abstract void onFileRemove(int row);

    public abstract void onFolderRemove(int row);

    public abstract void onFileDetailChange(int row, String detail);

    public abstract void onClearSelection(ArrayList<Integer> folderIndexes, ArrayList<Integer> fileIndexes);

    public abstract void onFileChange(int row, FileItemDetail item);

    public abstract void onFolderChange(int row, FolderItemDetail item);

    public abstract void onSelectAll();

    public abstract void onFileSelect(int row, boolean selected);

    public abstract void onFolderSelect(int row, boolean selected);

    public abstract void onFileAdded(int row, FileItemDetail item);

    public abstract void onFolderAdded(int row, FolderItemDetail item);
}
