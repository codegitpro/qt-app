package net.ai.tmp.cloudstore.visual;

import android.support.v4.util.Pair;

import net.ai.Element;
import net.ai.IntResult;
import net.ai.Message;
import net.ai.OsForm;
import net.ai.Status;

import java.util.ArrayList;
import java.util.HashMap;

public class GenericForm extends OsForm
{
    public final HashMap<Integer, Boolean> enabled = new HashMap<>();
    public Form destination;

    public final String title;
    public final boolean startHierarchy;
    public final int width;
    public final int height;
    public final IntResult action;
    public final ArrayList<Element> elements;

    public GenericForm(String title, boolean startHierarchy, int width, int height, net.ai.IntResult action, ArrayList<Element> elements)
    {
        this.title = title;
        this.startHierarchy = startHierarchy;
        this.width = width;
        this.height = height;
        this.action = action;
        this.elements = elements;
    }

    @Override
    public void actionResult(Status outcome, ArrayList<Message> lines)
    {
        if (null != destination)
            destination.actionResult(outcome, lines);
    }

    @Override
    public void progress(int position, int total)
    {
        if (null != destination)
            destination.progress(position, total);
    }

    @Override
    public void enable(int group)
    {
        if (null != destination)
            destination.enable(group);
        else
            enabled.put(Integer.valueOf(group), Boolean.TRUE);
    }

    @Override
    public void disable(int group)
    {
        if (null != destination)
            destination.disable(group);
        else
            enabled.put(Integer.valueOf(group), Boolean.FALSE);
    }

    @Override
    public HashMap<Integer, String> input()
    {
        return null != destination ? destination.input() : null;
    }
}
