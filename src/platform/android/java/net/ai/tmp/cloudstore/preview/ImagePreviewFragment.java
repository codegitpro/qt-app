package net.ai.tmp.cloudstore.preview;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import net.ai.tmp.cloudstore.GlobalApplication;
import net.ai.tmp.cloudstore.R;
import net.ai.tmp.cloudstore.Util;
import net.ai.tmp.cloudstore.implementation.IBinaryResult;

public class ImagePreviewFragment extends Fragment
{
    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        Context ctx = container.getContext();
        ConstraintLayout layout = new ConstraintLayout(ctx);
        int id = 1;
        layout.setId(id++);

        final ImageView imageView = new ImageView(ctx);
        imageView.setId(id++);
        layout.addView(imageView);

        GlobalApplication app = (GlobalApplication)getActivity().getApplicationContext();
        app.fileReadbinary(app.previewPath(), new IBinaryResult()
        {
            @Override
            public void onResult(byte[] data)
            {
                if (null == data || 0 == data.length)
                    return;
                final Bitmap bmp = BitmapFactory.decodeByteArray(data, 0, data.length);
                getActivity().runOnUiThread(new Runnable()
                {
                    @Override
                    public void run()
                    {
                        imageView.setImageBitmap(bmp);
                    }
                });
            }
        });

        ConstraintSet set = new ConstraintSet();
        set.constrainHeight(imageView.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);
        set.constrainWidth(imageView.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);

        set.applyTo(layout);
        ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, ConstraintLayout.LayoutParams.MATCH_PARENT);
        layout.setLayoutParams(args);
        return layout;
    }
}
