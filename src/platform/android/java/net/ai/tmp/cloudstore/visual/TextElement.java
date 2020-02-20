package net.ai.tmp.cloudstore.visual;

import android.widget.EditText;
import android.widget.TextView;

import net.ai.Element;

public class TextElement implements InteractiveElement
{
    private final Element el;
    private final TextView errorView;
    private final EditText view;

    public TextElement(Element el, TextView errorView, EditText view)
    {
        this.el = el;
        this.errorView = errorView;
        this.view = view;
    }

    @Override
    public void enable(boolean enabled)
    {
        view.setEnabled(enabled);
    }

    @Override
    public int groupId()
    {
        return el.getGroup();
    }

    @Override
    public void message(String error)
    {
        errorView.setText(error);
    }

    @Override
    public int fieldId()
    {
        return el.getInputId();
    }
}
