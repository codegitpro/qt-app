package net.ai.tmp.cloudstore.directory;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.support.annotation.NonNull;
import android.support.constraint.ConstraintLayout;
import android.support.constraint.ConstraintSet;
import android.support.v7.widget.RecyclerView;
import android.text.Layout;
import android.text.TextUtils;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import net.ai.DirectoriesDetail;
import net.ai.FileState;
import net.ai.FilesDetail;
import net.ai.Portal;
import net.ai.tmp.cloudstore.R;
import net.ai.tmp.cloudstore.Util;
import net.ai.tmp.cloudstore.visual.Theme;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class DirectoryAdaptor extends RecyclerView.Adapter<DirectoryAdaptor.ViewHolder>
{
    private final Portal portal;
    private final DirectoryAction action;
    private final List<String> allDirectoryNames;
    private final List<String> allFileNames;
    private final Theme theme;

    private DirectoriesDetail dirs;
    private FilesDetailView files = new FilesDetailView();
    private Set<Integer> selectedFileIds = new HashSet<>();
    private Set<Integer> selectedFolderIds = new HashSet<>();

    public DirectoryAdaptor(Portal portal, DirectoryAction action, List<String> allDirectoryNames, List<String> allFileNames, Theme theme)
    {
        this.portal = portal;
        this.action = action;
        this.allDirectoryNames = allDirectoryNames;
        this.allFileNames = allFileNames;
        this.theme = theme;
    }

    public void downloadAll()
    {
        if (selectedFolderIds.isEmpty() && selectedFileIds.isEmpty())
            return;
        ArrayList<Integer> fileIds = new ArrayList<>();
        for (Integer i : selectedFileIds)
            fileIds.add(i);
        ArrayList<Integer> folderIds = new ArrayList<>();
        for (Integer i : selectedFolderIds)
            folderIds.add(i);
        portal.downloadAll(folderIds, fileIds);
        selectedFileIds.clear();
        selectedFolderIds.clear();
        action.hasSelection(0);
        this.notifyDataSetChanged();

    }

    public void deleteAll()
    {
        if (selectedFolderIds.isEmpty() && selectedFileIds.isEmpty())
            return;
        ArrayList<Integer> fileIds = new ArrayList<>();
        for (Integer i : selectedFileIds)
            fileIds.add(i);
        ArrayList<Integer> folderIds = new ArrayList<>();
        for (Integer i : selectedFolderIds)
            folderIds.add(i);
        portal.deleteAll(folderIds, fileIds);
        selectedFileIds.clear();
        selectedFolderIds.clear();
        action.hasSelection(0);
        this.notifyDataSetChanged();

    }

    public void fileStatesChanged(ArrayList<Integer> fileIds, ArrayList<FileState> states)
    {
        files.setStates(fileIds, states);
        this.notifyDataSetChanged();
    }

    public void update(DirectoriesDetail dirs, FilesDetail files)
    {
        this.dirs = dirs;
        this.files.set(files);
        this.notifyDataSetChanged();
    }

    public void updateFile(int fileId, long size, int percentage, FileState state, byte[] thumbnail)
    {
        for (int i = 0; i < files.size(); ++i)
        {
            if (files.getId(i) != fileId)
                continue;

            files.setFileSize(i, size);
            files.setPercentage(i, percentage);
            files.setState(i, state);
            files.setThumbnail(i, thumbnail);
            notifyItemChanged(i + dirs.getIds().size());
            return;
        }
    }

    @Override
    public int getItemViewType(int position)
    {
        int dirCount = dirs.getIds().size();
        return position < dirCount ? 0 : 1;
    }

    public String fileOrFolderName(int fileId, int folderId)
    {
        if (-1 != fileId)
        {
            for (int i = 0; i < files.ids().size(); ++i)
            {
                if (files.ids().get(i).intValue() == fileId)
                    return allFileNames.get(files.getTitle(i));
            }
        }
        else if (-1 != folderId)
        {
            for (int i = 0; i < dirs.getIds().size(); ++i)
            {
                if (dirs.getIds().get(i).intValue() == folderId)
                    return allDirectoryNames.get(dirs.getTitles().get(i));
            }
        }
        return "";
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position)
    {

        int dirCount = dirs.getIds().size();

        if (position < dirCount)
        {
            String name = allDirectoryNames.get(dirs.getTitles().get(position));
            holder.name().setText(name);
            Integer folderId = dirs.getIds().get(position);
            if (selectedFolderIds.contains(folderId))
                holder.icon().setImageResource(R.drawable.ic_check_black_24dp);
            else
                holder.icon().setImageResource(R.drawable.ic_folder_open_black_24dp);
        }
        else
        {
            position -= dirCount;

            String name = allFileNames.get(files.getTitle(position));
            holder.name().setText(name);
            int percentage = files.getPercentage(position);
            FileState state = files.getState(position);
            Integer fileId = files.getId(position);

            switch (state)
            {
                case DELETING:
                {
                    holder.icon().setImageResource(R.drawable.ic_delete_forever_black_24dp);
                    break;
                }
                case DOWNLOADING:
                {
                    holder.aside().setText("" + percentage + " %");
                    holder.icon().setImageResource(R.drawable.ic_cloud_download_black_24dp);
                    break;
                }
                case UPLOADING:
                {
                    holder.aside().setText("" + percentage + " %");
                    holder.icon().setImageResource(R.drawable.ic_cloud_upload_black_24dp);
                    break;
                }
                default:
                {
                    holder.aside().setText(Util.compactSize(files.getFileSize(position)));
                    if (100 == percentage)
                        holder.icon().setImageResource(R.drawable.ic_cloud_done_black_24dp);
                    else if (selectedFileIds.contains(fileId))
                        holder.icon().setImageResource(R.drawable.ic_check_black_24dp);
                    else {
                        byte[] thumbnail = files.getThumbnail(position);
                        if (null == thumbnail || 0 == thumbnail.length)
                        {
                            if (FileState.LOCAL == state)
                                holder.icon().setImageResource(R.drawable.ic_cloud_done_black_24dp);
                            else
                                holder.icon().setImageDrawable(null);
                        }
                        else {
                            Drawable img = theme.createThumbnail(thumbnail);
                            holder.icon().setImageDrawable(img);
                        }

                    }
                    break;
                }
            }
        }
    }

    @Override
    public int getItemCount()
    {
        if (null == dirs)
            return files.size();
        return dirs.getIds().size() + files.size();
    }

    private void onLongClick(int position)
    {
        selectItem(position);
    }

    public int currentSelectedFileId()
    {
        return 1 == selectedFileIds.size() ? selectedFileIds.iterator().next().intValue() : -1;
    }

    public int currentSelectedFolderId()
    {
        return 1 == selectedFolderIds.size() ? selectedFolderIds.iterator().next().intValue() : -1;
    }

    private void selectItem(int position)
    {
        int dirCount = dirs.getIds().size();
        if (position < dirCount)
        {
            Integer folderId = dirs.getIds().get(position);
            if (selectedFolderIds.contains(folderId))
                selectedFolderIds.remove(folderId);
            else
                selectedFolderIds.add(folderId);
        }
        else
        {
            Integer fileId = files.getId(position - dirCount);
            if (selectedFileIds.contains(fileId))
                selectedFileIds.remove(fileId);
            else
                selectedFileIds.add(fileId);
        }

        action.hasSelection(selectedFileIds.size() + selectedFolderIds.size());
        notifyItemChanged(position);
    }

    public void onClick(int position)
    {
        if (!selectedFolderIds.isEmpty() || !selectedFileIds.isEmpty())
        {
            selectItem(position);
            return;
        }

        int dirCount = dirs.getIds().size();
        if (position < dirCount)
        {
            Integer folderId = dirs.getIds().get(position);
            action.folderClicked(folderId, allDirectoryNames.get(dirs.getTitles().get(position)));
            return;
        }

        Integer fileId = files.getId(position - dirCount);
        if (files.getState(position - dirCount) != FileState.LOCAL)
        {
            selectItem(position);
            return;
        }

        action.fileClicked(fileId);
    }

    public String fileName(int fileNameId)
    {
        for (int i = 0; i < files.size(); ++i)
        {
            if (files.getId(i) == fileNameId)
                return allFileNames.get(files.getTitle(i));
        }
        return "";
    }

    public void clearSelection()
    {
        if (selectedFolderIds.isEmpty() && selectedFileIds.isEmpty())
            return;
        selectedFileIds.clear();
        selectedFolderIds.clear();
        notifyDataSetChanged();
    }

    public void selectAll()
    {
        selectedFileIds.clear();
        for (Integer id: files.ids())
        {
            selectedFileIds.add(id);
        }
        selectedFolderIds.clear();
        for (Integer id: dirs.getIds())
        {
            selectedFolderIds.add(id);
        }
        notifyDataSetChanged();
    }

    public static class  ViewHolder extends RecyclerView.ViewHolder
    {
        private final TextView name;
        private final TextView aside;
        private final View itemView;
        private final ImageView icon;
        public ViewHolder(View itemView, final DirectoryAdaptor owner)
        {
            super(itemView);
            this.itemView = itemView;
            name = (TextView)itemView.findViewById(CellTextId);
            aside = (TextView)itemView.findViewById(CellAsideId);
            icon = (ImageView)itemView.findViewById(CellIconId);
            itemView.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v)
                {
                    owner.onClick(getAdapterPosition());
                }
            });
            itemView.setOnLongClickListener(new View.OnLongClickListener() {
                @Override
                public boolean onLongClick(View view) {
                    owner.onLongClick(getAdapterPosition());
                    return true;
                }
            });
        }
        public TextView name() { return name; }
        public TextView aside() { return aside; }
        public ImageView icon() { return icon; }
        public View itemView() { return itemView; }
    }

    private static final int CellId = 1;
    private static final int CellTextId = 2;
    private static final int CellAsideId = 3;
    private static final int CellIconId = 4;

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType)
    {
        float px = theme.dipPx();
        int iconWidth = (int)(40f*px);

        Context ctx = parent.getContext();
        ConstraintLayout v = new ConstraintLayout(ctx);
        v.setId(CellId);

        TextView name = new TextView(ctx);
        name.setId(CellTextId);
        v.addView(name);
        name.setMaxLines(1);
        name.setEllipsize(TextUtils.TruncateAt.END);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
            name.setBreakStrategy(Layout.BREAK_STRATEGY_SIMPLE);

        name.setPadding(iconWidth, 0, 0, 0);

        ImageView icon = new ImageView(ctx);
        icon.setPadding(0, 0, theme.padding(), 0);
        icon.setId(CellIconId);
        v.addView(icon);


        switch (viewType)
        {
            case 0:
            {
                name.setTextSize(TypedValue.COMPLEX_UNIT_SP, 24);
                ConstraintSet set = new ConstraintSet();

                set.constrainWidth(icon.getId(), iconWidth);
                set.constrainHeight(icon.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);
                name.setGravity(Gravity.CENTER);

                set.constrainHeight(name.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);
                set.constrainWidth(name.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);

                set.applyTo(v);
                break;
            }
            case 1:
            {
                TextView aside = new TextView(ctx);
                aside.setId(CellAsideId);
                v.addView(aside);

                name.setGravity(Gravity.CENTER);

                name.setTextSize(TypedValue.COMPLEX_UNIT_SP, 16);
                aside.setTextSize(TypedValue.COMPLEX_UNIT_SP, 10);
                ConstraintSet set = new ConstraintSet();

                set.constrainWidth(icon.getId(), iconWidth);
                set.constrainHeight(icon.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);

                set.constrainHeight(name.getId(), ConstraintLayout.LayoutParams.MATCH_PARENT);
                set.constrainWidth(name.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);

                set.connect(aside.getId(), ConstraintLayout.LayoutParams.BOTTOM, v.getId(), ConstraintLayout.LayoutParams.BOTTOM);
                set.connect(aside.getId(), ConstraintLayout.LayoutParams.LEFT, icon.getId(), ConstraintLayout.LayoutParams.RIGHT);
                set.constrainWidth(aside.getId(), ConstraintLayout.LayoutParams.WRAP_CONTENT);

                set.applyTo(v);
                break;
            }
        }

        {
            ConstraintLayout.LayoutParams args = new ConstraintLayout.LayoutParams(ConstraintLayout.LayoutParams.MATCH_PARENT, (int)(50f*px));
            v.setLayoutParams(args);
        }
        return new ViewHolder(v, this);
    }
}
