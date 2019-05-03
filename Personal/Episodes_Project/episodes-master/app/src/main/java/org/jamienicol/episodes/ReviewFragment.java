package org.jamienicol.episodes;

import android.content.AsyncQueryHandler;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.LoaderManager;
import android.support.v4.content.CursorLoader;
import android.support.v4.content.Loader;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.text.method.KeyListener;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import org.jamienicol.episodes.db.ShowsTable;
import org.jamienicol.episodes.db.ShowsProvider;

public class ReviewFragment
        extends Fragment
        implements LoaderManager.LoaderCallbacks<Cursor>
{
    private int showId;
    private EditText reviewText;
    private Button saveButton;
    private Button cancelButton;
    private boolean editToggle;
    private String currentText;

    public static ReviewFragment newInstance(int showId) {
        final ReviewFragment instance = new ReviewFragment();

        final Bundle args = new Bundle();
        args.putInt("showId", showId);

        instance.setArguments(args);
        return instance;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        showId = getArguments().getInt("showId");
    }

    public View onCreateView(LayoutInflater inflater,
                             ViewGroup container,
                             Bundle savedInstanceState) {
        final View view = inflater.inflate(R.layout.review_fragment,
                container,
                false);

        saveButton = (Button)view.findViewById(R.id.save_review);
        cancelButton = (Button)view.findViewById(R.id.cancel_review);
        reviewText = (EditText)view.findViewById(R.id.review);
        editToggle = false;

        reviewText.setTag(reviewText.getKeyListener());
        reviewText.setKeyListener(null);
        currentText = reviewText.getText().toString();

        saveButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if(editToggle) {
                    String t = reviewText.getText().toString();
                    Log.d("ReviewFragment", String.format("onTextChanged: %s", t.toString()));
                    final ContentResolver contentResolver = getActivity().getContentResolver();
                    final AsyncQueryHandler handler = new AsyncQueryHandler(contentResolver) {
                    };
                    final Uri episodeUri = Uri.withAppendedPath(ShowsProvider.CONTENT_URI_SHOWS, String.valueOf(showId));
                    final ContentValues reviewValue = new ContentValues();

                    reviewValue.put(ShowsTable.COLUMN_COMMENT, t.toString());
                    handler.startUpdate(0, null, episodeUri, reviewValue, null, null);

                    Toast toast = Toast.makeText(view.getContext(), "Saving review...", Toast.LENGTH_SHORT);
                    toast.show();

                    saveButton.setText("Edit");
                    reviewText.setTag(reviewText.getKeyListener());
                    reviewText.setKeyListener(null);
                    reviewText.setBackgroundColor(Color.WHITE);
                    cancelButton.setVisibility(View.GONE);
                    editToggle = false;
                } else {
                    saveButton.setText("Save");
                    reviewText.setKeyListener((KeyListener)reviewText.getTag());
                    reviewText.setBackgroundColor(Color.LTGRAY);
                    reviewText.requestFocus();
                    currentText = reviewText.getText().toString();
                    cancelButton.setVisibility(View.VISIBLE);
                    editToggle = true;
                }
            }
        });

        cancelButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                saveButton.setText("Edit");
                reviewText.setTag(reviewText.getKeyListener());
                reviewText.setKeyListener(null);
                reviewText.setText(currentText);
                reviewText.setBackgroundColor(Color.WHITE);
                cancelButton.setVisibility(View.GONE);
                editToggle = false;
            }
        });

        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        final Bundle loaderArgs = new Bundle();
        loaderArgs.putInt("showId", showId);
        getLoaderManager().initLoader(0, loaderArgs, this);
    }

    @Override
    public Loader<Cursor> onCreateLoader(int id, Bundle args) {
        final int showId = args.getInt("showId");
        Uri uri = Uri.withAppendedPath(ShowsProvider.CONTENT_URI_SHOWS,
            String.valueOf(showId));

        final String[] projection = {
            ShowsTable.COLUMN_ID,
            ShowsTable.COLUMN_COMMENT
        };

        return new CursorLoader(getActivity(),
            uri,
            projection,
            null,
            null,
            null);
    }

    @Override
    public void onLoadFinished(Loader<Cursor> loader, Cursor data) {
        if (data != null && data.moveToFirst()) {
            int reviewIndex = data.getColumnIndexOrThrow(ShowsTable.COLUMN_COMMENT);
            if (data.isNull(reviewIndex)) {
                reviewText.setVisibility(View.GONE);
            } else {
                reviewText.setText(data.getString(reviewIndex).trim());
                reviewText.setVisibility(View.VISIBLE);
            }
        }
    }

    @Override
    public void onLoaderReset(Loader<Cursor> loader) {
        onLoadFinished(loader, null);
    }
}
