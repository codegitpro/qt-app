package net.ai.tmp.cloudstore.visual;

import android.widget.CheckBox;

import net.ai.Element;

public class BooleanInputElement implements InputElement
{
    private final CheckBox view;
    private final Element element;

    public BooleanInputElement(Element el, CheckBox view)
    {
        this.element = el;
        this.view = view;
    }

    @Override
    public Integer inputId()
    {
        return element.getInputId();
    }

    @Override
    public String value()
    {
        return view.isChecked() ? "true" : "false";
    }

    @Override
    public void setValue(String val)
    {
        view.setChecked("true".equals(val));
    }
}
