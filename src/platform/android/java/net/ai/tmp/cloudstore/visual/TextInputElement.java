package net.ai.tmp.cloudstore.visual;

import android.widget.EditText;

import net.ai.Element;

public class TextInputElement implements InputElement
{
    private final EditText view;
    private final Element element;

    public TextInputElement(Element element, EditText view)
    {
        this.view = view;
        this.element = element;
    }

    @Override
    public Integer inputId()
    {
        return element.getInputId();
    }

    @Override
    public String value()
    {
        return view.getText().toString();
    }

    @Override
    public void setValue(String val)
    {
        view.setText(val);
    }
}
