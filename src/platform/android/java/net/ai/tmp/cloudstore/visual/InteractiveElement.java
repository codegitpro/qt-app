package net.ai.tmp.cloudstore.visual;

public interface InteractiveElement
{
    void enable(boolean enabled);
    int groupId();
    void message(String error);
    int fieldId();
}
