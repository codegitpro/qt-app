package net.ai.tmp.cloudstore.directory;

import android.content.Context;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import net.ai.tmp.cloudstore.DirectoryState;
import net.ai.tmp.cloudstore.R;

public class DirectoryFragment extends Fragment
{
    private DirectoryState state = null;

    @Override
    public void onAttach(Context context)
    {
        super.onAttach(context);
        state = (DirectoryState)context;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState)
    {
        View rootView = inflater.inflate(R.layout.directory_frag, container, false);

        RecyclerView list = rootView.findViewById(R.id.directory_list);
        list.setLayoutManager(new LinearLayoutManager(getActivity()));
        list.setAdapter(state.adapter());

        return rootView;
    }
}
