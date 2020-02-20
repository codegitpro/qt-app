package net.ai.tmp.cloudstore.preview;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import net.ai.tmp.cloudstore.R;
import net.ai.tmp.cloudstore.Util;

import java.io.FileInputStream;

public class TextPreviewFragment extends Fragment
{
    private PreviewState state;

    @Override
    public void onAttach(Context context)
    {
        super.onAttach(context);
        state = (PreviewState)context;
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View rootView = inflater.inflate(R.layout.text_preview, container, false);
        TextView textView = (TextView)rootView.findViewById(R.id.text_body);

        try
        {
            textView.setText(Util.readStringFromFile(state.filePath()));
        }
        catch (Exception e)
        {
            textView.setText("An error occurred reading " + state.filePath());
        }
        return rootView;
    }
}
