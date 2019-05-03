/*
 * Copyright (C) 2012 Jamie Nicol <jamie@thenicols.net>
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

import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.Html;
import android.text.method.LinkMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.ImageView;

import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;

import java.text.DateFormat;
import java.util.Date;
import java.util.List;
import org.jamienicol.episodes.services.AddShowService;
import org.jamienicol.episodes.tvdb.Client;
import org.jamienicol.episodes.tvdb.Show;

public class AddShowPreviewFragment
	extends Fragment
{
	private Show show;

	public static AddShowPreviewFragment newInstance(int searchResultIndex) {
		AddShowPreviewFragment instance = new AddShowPreviewFragment();

		Bundle args = new Bundle();
		args.putInt("searchResultIndex", searchResultIndex);

		instance.setArguments(args);
		return instance;
	}

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setHasOptionsMenu(true);
	}

	@Override
	public View onCreateView(LayoutInflater inflater,
	                         ViewGroup container,
	                         Bundle savedInstanceState) {
		View view = inflater.inflate(R.layout.add_show_preview_fragment,
		                             container,
		                             false);

		ImageView headerView = (ImageView)view.findViewById(R.id.search_header_image);
		TextView overviewView = (TextView)view.findViewById(R.id.overview);
		TextView firstAiredView = (TextView)view.findViewById(R.id.first_aired);
		TextView titleView = (TextView)view.findViewById(R.id.search_show_title);
		TextView genreView = (TextView)view.findViewById(R.id.genre);
		TextView castView = (TextView)view.findViewById(R.id.cast);
		TextView runtimeView = (TextView)view.findViewById(R.id.runtime);
		TextView imdbView = (TextView)view.findViewById(R.id.imdb);
		TextView statusView = (TextView)view.findViewById(R.id.status);
		TextView contentView = (TextView)view.findViewById(R.id.content_rating);

		int searchResultIndex = getArguments().getInt("searchResultIndex");

		AddShowSearchResults results = AddShowSearchResults.getInstance();
		List<Show> resultsData = results.getData();

		// Ensure that there is actually data to display, because Android
		// may have destroyed it. If there is data display it, if there
		// isn't do nothing and the activity will handle the situation.
		if (resultsData != null) {
			show = resultsData.get(searchResultIndex);

			//Get api key and create client
			final Client tvdbClient = new Client("25B864A8BC56AFAD");

			// fetch full show + episode information from tvdb
			Show fullShow;
			try {
				fullShow = tvdbClient.getFullShow(show.getId(), show.getLanguage());
			} catch (InterruptedException e) {
				fullShow = null;
			}

			if(fullShow != null) {
				String fanartPath = fullShow.getFanartPath();

				if (fanartPath != null && !fanartPath.equals("")) {
					final String fanartUrl =
							String.format("http://thetvdb.com/banners/%s", fanartPath);

					final DisplayImageOptions options =
							new DisplayImageOptions.Builder()
									.cacheInMemory(true)
									.cacheOnDisk(true)
									.build();
					ImageLoader.getInstance().displayImage(fanartUrl,
							headerView,
							options);
				}
				titleView.setText(fullShow.getName());
				genreView.setText("Genre(s): " + fullShow.getGenre());

				String replaceStr = fullShow.getCast().replace(",",", ");
				replaceStr = replaceStr.replace("  "," ");
				castView.setText("Cast members: " + replaceStr);

				runtimeView.setText("Average Runtime: " + fullShow.getRuntime() + " minutes");

				String imdbText = String.format("<a href=\"%s\"> View on IMDB</a>", fullShow.getImdb());
				imdbView.setText(Html.fromHtml(imdbText));
				imdbView.setMovementMethod(LinkMovementMethod.getInstance());

				statusView.setText("Status: " + fullShow.getStatus());
				contentView.setText("Content Rating: "+ fullShow.getContentRating());
			}
			else {
				headerView.setVisibility(View.GONE);
				titleView.setVisibility(View.GONE);
				genreView.setVisibility(View.GONE);
				castView.setVisibility(View.GONE);
				statusView.setVisibility(View.GONE);
				contentView.setVisibility(View.GONE);
				runtimeView.setVisibility(View.GONE);
				imdbView.setVisibility(View.GONE);
			}


			overviewView.setText(show.getOverview());

			Date firstAired = show.getFirstAired();
			if (firstAired != null) {
				DateFormat df = DateFormat.getDateInstance();
				String text = getString(R.string.first_aired,
				                        df.format(show.getFirstAired()));
				firstAiredView.setText(text);
			} else {
				firstAiredView.setText("");
			}
		}

		return view;
	}

	@Override
	public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
		inflater.inflate(R.menu.add_show_preview_fragment, menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.menu_add_show:
			addShow();
			Intent intent = new Intent(getActivity(), MainActivity.class);
			getActivity().startActivity(intent);
			return true;
		default:
			return super.onOptionsItemSelected(item);
		}
	}

	private void addShow() {
		Intent intent = new Intent(getActivity(), AddShowService.class);
		intent.putExtra("tvdbId", show.getId());
		intent.putExtra("language", show.getLanguage());
		intent.putExtra("showName", show.getName());

		getActivity().startService(intent);
	}
}
