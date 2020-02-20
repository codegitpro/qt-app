package net.ai.tmp.cloudstore.directory;

public interface DirectoryAction
{
    void folderClicked(int folderId, String title);
    void hasSelection(int selected);
    void fileClicked(int fileId);
}
