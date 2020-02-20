package net.ai.tmp.cloudstore;

import net.ai.DirectoriesDetail;
import net.ai.DirectoryView;
import net.ai.FileState;
import net.ai.FilesDetail;

import java.util.ArrayList;

public class DirectoryProxy extends DirectoryView
{
    private final DirectoryInterface owner;

    public DirectoryProxy(DirectoryInterface owner)
    {
        this.owner = owner;
    }

    @Override
    public void refreshList(ArrayList<Integer> folderPath, DirectoriesDetail dirs, FilesDetail files, ArrayList<String> newDirectoryNames, ArrayList<String> newFileNames)
    {
        owner.directoryRefreshList(folderPath, dirs, files, newDirectoryNames, newFileNames);
    }

    @Override
    public void fileStatesChanged(ArrayList<Integer> folderPath, ArrayList<Integer> fileIds, ArrayList<FileState> states)
    {
        owner.fileStatesChanged(folderPath, fileIds, states);
    }

    @Override
    public void fileChanged(ArrayList<Integer> folderPath, int fileId, int fileNameId, long size, int percentage, FileState state, byte[] thumbnail)
    {
        owner.directoryFileChanged(folderPath, fileId, fileNameId, size, percentage, state, thumbnail);
    }
}
