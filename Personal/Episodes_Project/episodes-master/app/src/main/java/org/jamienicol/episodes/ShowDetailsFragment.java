/*
 * Copyright (C) 2013 Jamie Nicol <jamie@thenicols.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package org.jamienicol.episodes;

import android.content.AsyncQueryHandler;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.LoaderManager;
import android.support.v4.content.CursorLoader;
import android.support.v4.content.Loader;
import android.text.Html;
import android.text.method.LinkMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;
import java.text.DateFormat;
import java.util.Date;
import android.widget.RatingBar;
import android.util.Log;

import org.jamienicol.episodes.db.EpisodesTable;
import org.jamienicol.episodes.db.ShowsProvider;
import org.jamienicol.episodes.db.ShowsTable;
import android.widget.RatingBar;
import android.util.Log;

public class ShowDetailsFragment
	extends Fragment
	implements LoaderManager.LoaderCallbacks<Cursor>
{
	private static final String TAG = "ShowDetailsFragment";
	private int showId;
	private TextView overviewView;
	private TextView firstAiredView;
	private TextView genreView;
	private TextView castView;
	private TextView runtimeView;
	private TextView imdbView;
	private TextView statusView;
	private TextView contentView;
	private TextView lastViewedView;

	public static ShowDetailsFragment newInstance(int showId) {
		ShowDetailsFragment instance = new ShowDetailsFragment();

		Bundle args = new Bundle();
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
		View view = inflater.inflate(R.layout.show_details_fragment,
				container,
				false);

		overviewView = (TextView)view.findViewById(R.id.overview);
		firstAiredView = (TextView)view.findViewById(R.id.first_aired);
		genreView = (TextView)view.findViewById(R.id.genre);
		castView = (TextView)view.findViewById(R.id.cast);
		runtimeView = (TextView)view.findViewById(R.id.runtime);
		imdbView = (TextView)view.findViewById(R.id.imdb);
		statusView = (TextView)view.findViewById(R.id.status);
		contentView = (TextView)view.findViewById(R.id.content_rating);
		lastViewedView = (TextView)view.findViewById(R.id.last_viewed);


		return view;
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		Bundle loaderArgs = new Bundle();
		loaderArgs.putInt("showId", showId);
		getLoaderManager().initLoader(0, loaderArgs, this);
	}

	@Override
	public Loader<Cursor> onCreateLoader(int id, Bundle args) {
		int showId = args.getInt("showId");
		Uri uri = Uri.withAppendedPath(ShowsProvider.CONTENT_URI_SHOWS,
				String.valueOf(showId));
		String[] projection = {
			ShowsTable.COLUMN_OVERVIEW,
			ShowsTable.COLUMN_FIRST_AIRED,
			ShowsTable.COLUMN_GENRE,
			ShowsTable.COLUMN_STATUS,
			ShowsTable.COLUMN_RATING,
			ShowsTable.COLUMN_CONTENT_RATING,
			ShowsTable.COLUMN_CAST,
			ShowsTable.COLUMN_RUNTIME,
			ShowsTable.COLUMN_IMDB,
            ShowsTable.COLUMN_LAST_VIEWED
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

			int overviewColumnIndex =
				data.getColumnIndexOrThrow(ShowsTable.COLUMN_OVERVIEW);
			if (data.isNull(overviewColumnIndex)) {
				overviewView.setVisibility(View.GONE);
			} else {
				overviewView.setText(data.getString(overviewColumnIndex).trim());
				overviewView.setVisibility(View.VISIBLE);
			}

			int firstAiredColumnIndex =
				data.getColumnIndexOrThrow(ShowsTable.COLUMN_FIRST_AIRED);
			if (data.isNull(firstAiredColumnIndex)) {
				firstAiredView.setVisibility(View.GONE);
			} else {
				Date firstAired =
					new Date(data.getLong(firstAiredColumnIndex) * 1000);
				DateFormat df = DateFormat.getDateInstance();
				String firstAiredText = getString(R.string.first_aired,
				                                  df.format(firstAired));
				firstAiredView.setText(firstAiredText);
				firstAiredView.setVisibility(View.VISIBLE);
			}

			int genreColumnIndex =
					data.getColumnIndexOrThrow(ShowsTable.COLUMN_GENRE);
			if (data.isNull(genreColumnIndex)) {
				genreView.setVisibility(View.GONE);
			} else {
				String genreText = String.format("Genre(s): %s", data.getString(genreColumnIndex));
				genreView.setText(genreText);
				genreView.setVisibility(View.VISIBLE);
			}

			int castColumnIndex =
					data.getColumnIndexOrThrow(ShowsTable.COLUMN_CAST);
			if (data.isNull(castColumnIndex)) {
				castView.setVisibility(View.GONE);
			} else {
				String castText = String.format("Cast members: %s", data.getString(castColumnIndex));
				String replaceStr = castText.replace(",",", ");
				replaceStr = replaceStr.replace("  "," ");
				castView.setText(replaceStr);
				castView.setVisibility(View.VISIBLE);
			}

			int statusColumnIndex =
					data.getColumnIndexOrThrow(ShowsTable.COLUMN_STATUS);
			if (data.isNull(statusColumnIndex)) {
				statusView.setVisibility(View.GONE);
			} else {
				String statusText = String.format("Status: %s", data.getString(statusColumnIndex));
				statusView.setText(statusText);
				statusView.setVisibility(View.VISIBLE);
			}

			int contentColumnIndex =
					data.getColumnIndexOrThrow(ShowsTable.COLUMN_CONTENT_RATING);
			if (data.isNull(contentColumnIndex)) {
				contentView.setVisibility(View.GONE);
			} else {
				String contentText = String.format("Content Rating: %s", data.getString(contentColumnIndex));
				contentView.setText(contentText);
				contentView.setVisibility(View.VISIBLE);
			}

			int runtimeColumnIndex =
					data.getColumnIndexOrThrow(ShowsTable.COLUMN_RUNTIME);
			if (data.isNull(runtimeColumnIndex)) {
				runtimeView.setVisibility(View.GONE);
			} else {
				String runtimeText = String.format("Average Runtime: %s minutes", data.getString(runtimeColumnIndex));
				runtimeView.setText(runtimeText);
				runtimeView.setVisibility(View.VISIBLE);
			}

			int imdbColumnIndex =
					data.getColumnIndexOrThrow(ShowsTable.COLUMN_IMDB);
			if (data.isNull(imdbColumnIndex)) {
				imdbView.setVisibility(View.GONE);
			} else {
				String imdbText = String.format("<a href=\"%s\"> View on IMDB</a>", data.getString(imdbColumnIndex));
				imdbView.setText(Html.fromHtml(imdbText));
				imdbView.setMovementMethod(LinkMovementMethod.getInstance());
				imdbView.setVisibility(View.VISIBLE);
			}

			int lastViewedColumnIndex =
					data.getColumnIndexOrThrow(ShowsTable.COLUMN_LAST_VIEWED);
			if (data.isNull(lastViewedColumnIndex)) {
				lastViewedView.setVisibility(View.GONE);
			} else {
				if(data.getString(lastViewedColumnIndex).equals("")) {
					String lastViewedText = String.format("Last Viewed: N/A");
					lastViewedView.setText(lastViewedText);
				}
				else {
					String date = data.getString(lastViewedColumnIndex);
					String[] dateList = date.split(" ");
					String lastViewedText = String.format("Last Viewed: %s", dateList[0]);
					lastViewedView.setText(lastViewedText);
				}
				lastViewedView.setVisibility(View.VISIBLE);
			}


		} else {
			overviewView.setVisibility(View.GONE);
			firstAiredView.setVisibility(View.GONE);
			genreView.setVisibility(View.GONE);
			castView.setVisibility(View.GONE);
			statusView.setVisibility(View.GONE);
			contentView.setVisibility(View.GONE);
			runtimeView.setVisibility(View.GONE);
			imdbView.setVisibility(View.GONE);
			lastViewedView.setVisibility(View.GONE);
		}

	}



	@Override
	public void onLoaderReset(Loader<Cursor> loader) {
		onLoadFinished(loader, null);
	}
}
