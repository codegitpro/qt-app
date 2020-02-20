package net.ai.tmp.cloudstore.directory;

import net.ai.FileState;
import net.ai.FilesDetail;

import java.util.ArrayList;
import java.util.List;

public class FilesDetailView
{
    private FilesDetail files;
    private int[] map;
    private int size = 0;

    int size()
    {
        return size;
    }

    int getId(int position)
    {
        return files.getIds().get(map[position]);
    }

    public ArrayList<Integer> ids()
    {
        return files.getIds();
    }

    int getPercentage(int position)
    {
        return files.getPercentages().get(map[position]);
    }

    int getTitle(int position)
    {
        return files.getTitles().get(map[position]);
    }

    long getFileSize(int position)
    {
        return files.getSizes().get(map[position]);
    }

    FileState getState(int position)
    {
        return files.getStates().get(map[position]);
    }

    byte[] getThumbnail(int position)
    {
        return files.getThumbnails().get(position);
    }

    void setFileSize(int position, long size)
    {
        files.getSizes().set(position, size);
    }

    void setPercentage(int position, int percentage)
    {
        files.getPercentages().set(position, percentage);
    }

    void setState(int position, FileState state)
    {
        files.getStates().set(position, state);
    }

    void setThumbnail(int position, byte[] thumbnail)
    {
        files.getThumbnails().set(position, thumbnail);
    }

    void set(FilesDetail files)
    {
        this.files = files;
        if (null == files)
        {
            size = 0;
            return;
        }
        if (null == map || map.length < files.getIds().size())
            map = new int[files.getIds().size()];
        List<FileState> states = files.getStates();
        int position = 0;
        for (int i = 0; i < files.getIds().size(); ++i)
        {
            if (FileState.DELETED == states.get(i))
                continue;
            map[position++] = i;
        }
        size = position;
    }

    public void setStates(ArrayList<Integer> fileIds, ArrayList<FileState> states)
    {
        if (fileIds.size() != files.getIds().size())
            return;
        for (int i = 0; i < fileIds.size(); ++i)
        {
            int fileId = fileIds.get(i);
            if (files.getIds().get(i) != fileId)
                continue;
            files.getStates().set(i, states.get(i));
        }
    }
}
