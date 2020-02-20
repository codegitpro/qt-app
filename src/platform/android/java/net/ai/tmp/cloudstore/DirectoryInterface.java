package net.ai.tmp.cloudstore;

import net.ai.DirectoriesDetail;
import net.ai.FileState;
import net.ai.FilesDetail;

import java.util.ArrayList;

public interface DirectoryInterface
{
    void directoryRefreshList(ArrayList<Integer> folderPath, DirectoriesDetail dirs, FilesDetail files, ArrayList<String> newDirectoryNames, ArrayList<String> newFileNames);

    void fileStatesChanged(ArrayList<Integer> folderPath, ArrayList<Integer> fileIds, ArrayList<FileState> states);

    void directoryFileChanged(ArrayList<Integer> folderPath, int fileId, int fileNameId, long size, int percentage, FileState state, byte[] thumbnail);
}
