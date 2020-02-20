package net.ai.tmp.cloudstore.visual;

import net.ai.Message;
import net.ai.Status;

import java.util.ArrayList;
import java.util.HashMap;

public interface Form
{
    void actionResult(Status outcome, ArrayList<Message> lines);

    void progress(int position, int total);

    void enable(int group);

    void disable(int group);

    HashMap<Integer, String> input();
}
