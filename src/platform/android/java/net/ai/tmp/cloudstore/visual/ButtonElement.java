package net.ai.tmp.cloudstore.visual;

import android.widget.Button;
import android.widget.TextView;

import net.ai.Element;

public class ButtonElement implements InteractiveElement
{
    private final Element el;
    private final TextView errorView;
    private final Button view;

    public ButtonElement(Element el, TextView errorView, Button view)
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
