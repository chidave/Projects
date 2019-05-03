/*
 * Copyright (C) 2012-2014 Jamie Nicol <jamie@thenicols.net>
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

import android.app.Activity;
import android.content.AsyncQueryHandler;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.app.ListFragment;
import android.support.v4.app.LoaderManager;
import android.support.v4.content.CursorLoader;
import android.support.v4.content.Loader;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.ToggleButton;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Hashtable;
import java.util.List;
import java.util.*;
import org.jamienicol.episodes.db.EpisodesTable;
import org.jamienicol.episodes.db.ShowsProvider;
import org.jamienicol.episodes.db.ShowsTable;
import org.jamienicol.episodes.services.RefreshShowService;

public class ShowsListFragment
	extends ListFragment
	implements LoaderManager.LoaderCallbacks<Cursor>
{
	private static final String TAG = ShowsListFragment.class.getName();
	private static final int LOADER_ID_SHOWS = 0;
	private static final int LOADER_ID_EPISODES = 1;

	//Variables for shared preferences
	private static final String KEY_PREF_SHOWS_FILTER = "pref_shows_filter";
	private static final int SHOWS_FILTER_ALL = 0;
	private static final int SHOWS_FILTER_STARRED = 1;
	private static final int SHOWS_FILTER_UNCOMPLETED = 2;
	private static final int SHOWS_FILTER_GENRE = 3;
	private static final int SHOWS_FILTER_GENRE_ACTION = 4;
	private static final int SHOWS_FILTER_GENRE_ADVENTURE = 5;
	private static final int SHOWS_FILTER_GENRE_ANIMATION = 6;
	private static final int SHOWS_FILTER_GENRE_COMEDY = 7;
	private static final int SHOWS_FILTER_GENRE_CRIME = 8;
	private static final int SHOWS_FILTER_GENRE_DRAMA = 9;
	private static final int SHOWS_FILTER_GENRE_FAMILY = 10;
	private static final int SHOWS_FILTER_GENRE_FANTASY = 11;
	private static final int SHOWS_FILTER_GENRE_HORROR = 12;
	private static final int SHOWS_FILTER_GENRE_MYSTERY = 13;
	private static final int SHOWS_FILTER_GENRE_ROMANCE = 14;
	private static final int SHOWS_FILTER_GENRE_SCIFI = 15;
	private static final int SHOWS_FILTER_GENRE_SUSPENSE = 16;
	private static final int SHOWS_FILTER_GENRE_THRILLER = 17;
	private static final int SHOWS_FILTER_AIRING = 18;


	private static final String KEY_PREF_SHOWS_SORT = "pref_shows_sort";
	private static final int SHOWS_SORT_ALPHA_ASCEND = 1;
	private static final int SHOWS_SORT_ALPHA_DESCEND = -1;
	private static final int SHOWS_SORT_RATING_DESCEND = -10;
	private static final int SHOWS_SORT_RATING_ASCEND = 10;
	private static final int SHOWS_SORT_RUNTIME_ASCEND = 2;
	private static final int SHOWS_SORT_RUNTIME_DESCEND = -2;
	private static final int SHOWS_SORT_WATCHED_ASCEND = 3;
	private static final int SHOWS_SORT_WATCHED_DESCEND = -3;


	private ShowsListAdapter listAdapter;
	private Cursor showsData;
	private Cursor episodesData;

	public interface OnShowSelectedListener {
		public void onShowSelected(int showId);
	}
	private OnShowSelectedListener onShowSelectedListener;

	@Override
	public void onAttach(Activity activity) {
		super.onAttach(activity);

		try {
			onShowSelectedListener = (OnShowSelectedListener)activity;
		} catch (ClassCastException e) {
			final String message =
				String.format("%s must implement OnShowSelectedListener",
				              activity.toString());
			throw new ClassCastException(message);
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setHasOptionsMenu(true);
    }

	public View onCreateView(LayoutInflater inflater,
	                         ViewGroup container,
	                         Bundle savedInstanceState) {
		return inflater.inflate(R.layout.shows_list_fragment, container, false);
	}

	@Override
	public void onActivityCreated(Bundle savedInstanceState) {
		super.onActivityCreated(savedInstanceState);

		listAdapter = new ShowsListAdapter(getActivity(),
		                                   null,
		                                   null);
		setListAdapter(listAdapter);

		getLoaderManager().initLoader(LOADER_ID_SHOWS, null, this);
		getLoaderManager().initLoader(LOADER_ID_EPISODES, null, this);
	}

	@Override
	public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
		inflater.inflate(R.menu.shows_list_fragment, menu);
	}

	@Override
	public void onPrepareOptionsMenu(Menu menu) {

		// hide refresh all option if no shows exist
		final boolean showsExist =
			(showsData != null && showsData.moveToFirst());

		menu.findItem(R.id.menu_refresh_all_shows).setVisible(showsExist);

		/* set the currently selected filter's menu item as checked */
		final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(getActivity());
		final int filter =	prefs.getInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_ALL);
		final int sort = prefs.getInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_ALPHA_ASCEND);

		//On first load, the filter is set to "All Shows"
		menu.findItem(R.id.menu_filter_all).setChecked(true);
		final SharedPreferences preference =
				PreferenceManager.getDefaultSharedPreferences(getActivity());
		final SharedPreferences.Editor editor = preference.edit();
		editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_ALL);
		editor.putString("Genre", "");

		switch (sort) {
			case SHOWS_SORT_ALPHA_ASCEND:
				menu.findItem(R.id.menu_sort_alpha_ascending).setChecked(true);
				break;
			case SHOWS_SORT_ALPHA_DESCEND:
				menu.findItem(R.id.menu_sort_alpha_descending).setChecked(true);
				break;
			case SHOWS_SORT_RUNTIME_ASCEND:
				menu.findItem(R.id.menu_sort_runtime_ascending).setChecked(true);
				break;
			case SHOWS_SORT_RUNTIME_DESCEND:
				menu.findItem(R.id.menu_sort_runtime_descending).setChecked(true);
				break;
			case SHOWS_SORT_RATING_ASCEND:
				menu.findItem(R.id.menu_sort_rating_ascending).setChecked(true);
				break;
			case SHOWS_SORT_RATING_DESCEND:
				menu.findItem(R.id.menu_sort_rating_descending).setChecked(true);
				break;
			case SHOWS_SORT_WATCHED_ASCEND:
				menu.findItem(R.id.menu_sort_watched_ascending).setChecked(true);
				break;
			case SHOWS_SORT_WATCHED_DESCEND:
				menu.findItem(R.id.menu_sort_watched_descending).setChecked(true);
				break;
		}

		editor.apply();

		super.onPrepareOptionsMenu(menu);
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {

		switch (item.getItemId()) {
		case R.id.menu_refresh_all_shows:
			refreshAllShows();
			return true;
		case R.id.menu_sort_alpha_ascending:
		case R.id.menu_sort_alpha_descending:
		case R.id.menu_sort_rating_ascending:
		case R.id.menu_sort_rating_descending:
		case R.id.menu_sort_runtime_ascending:
		case R.id.menu_sort_runtime_descending:
		case R.id.menu_sort_watched_ascending:
		case R.id.menu_sort_watched_descending:
		case R.id.menu_filter_all:
		case R.id.menu_filter_starred:
		case R.id.menu_filter_uncompleted:
		case R.id.menu_filter_genre:
		case R.id.menu_filter_genre_action:
		case R.id.menu_filter_genre_adventure:
		case R.id.menu_filter_genre_animation:
		case R.id.menu_filter_genre_comedy:
		case R.id.menu_filter_genre_crime:
		case R.id.menu_filter_genre_drama:
		case R.id.menu_filter_genre_family:
		case R.id.menu_filter_genre_fantasy:
		case R.id.menu_filter_genre_horror:
		case R.id.menu_filter_genre_mystery:
		case R.id.menu_filter_genre_romance:
		case R.id.menu_filter_genre_scifi:
		case R.id.menu_filter_genre_suspense:
		case R.id.menu_filter_genre_thriller:
		case R.id.menu_filter_airing:


			if (!item.isChecked()) {
				item.setChecked(true);
			}

			final SharedPreferences prefs =
				PreferenceManager.getDefaultSharedPreferences(getActivity());
			final SharedPreferences.Editor editor = prefs.edit();
			if (item.getItemId() == R.id.menu_filter_all) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_ALL);
				editor.putString("Genre", "");
			} else if (item.getItemId() == R.id.menu_filter_starred) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_STARRED);
				editor.putString("Genre", "");
			} else if (item.getItemId() == R.id.menu_filter_uncompleted) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_UNCOMPLETED);
				editor.putString("Genre", "");
			} else if (item.getItemId() == R.id.menu_filter_genre_action) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_ACTION);
				editor.putString("Genre", "Action");
			} else if (item.getItemId() == R.id.menu_filter_genre_adventure) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_ADVENTURE);
				editor.putString("Genre", "Adventure");
			} else if (item.getItemId() == R.id.menu_filter_genre_animation) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_ANIMATION);
				editor.putString("Genre", "Animation");
			} else if (item.getItemId() == R.id.menu_filter_genre_comedy) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_COMEDY);
				editor.putString("Genre", "Comedy");
			} else if (item.getItemId() == R.id.menu_filter_genre_crime) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_CRIME);
				editor.putString("Genre", "Crime");
			} else if (item.getItemId() == R.id.menu_filter_genre_drama) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_DRAMA);
				editor.putString("Genre", "Drama");
			} else if (item.getItemId() == R.id.menu_filter_genre_family) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_FAMILY);
				editor.putString("Genre", "Family");
			} else if (item.getItemId() == R.id.menu_filter_genre_fantasy) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_FANTASY);
				editor.putString("Genre", "Fantasy");
			} else if (item.getItemId() == R.id.menu_filter_genre_horror) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_HORROR);
				editor.putString("Genre", "Horror");
			} else if (item.getItemId() == R.id.menu_filter_genre_mystery) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_MYSTERY);
				editor.putString("Genre", "Mystery");
			} else if (item.getItemId() == R.id.menu_filter_genre_romance) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_ROMANCE);
				editor.putString("Genre", "Romance");
			} else if (item.getItemId() == R.id.menu_filter_genre_scifi) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_SCIFI);
				editor.putString("Genre", "Science-Fiction");
			} else if (item.getItemId() == R.id.menu_filter_genre_suspense) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_SUSPENSE);
				editor.putString("Genre", "Suspense");
			} else if (item.getItemId() == R.id.menu_filter_genre_thriller) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_GENRE_THRILLER);
				editor.putString("Genre", "Thriller");
			} else if (item.getItemId() == R.id.menu_filter_airing) {
				editor.putInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_AIRING);
				editor.putString("Genre", "");
			}


			if (item.getItemId() == R.id.menu_sort_alpha_ascending) {
				editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_ALPHA_ASCEND);
			}else if (item.getItemId() == R.id.menu_sort_alpha_descending) {
				editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_ALPHA_DESCEND);
			}if (item.getItemId() == R.id.menu_sort_rating_ascending) {
				editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_RATING_ASCEND);
			}else if (item.getItemId() == R.id.menu_sort_rating_descending) {
					editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_RATING_DESCEND);
			}else if (item.getItemId() == R.id.menu_sort_runtime_ascending) {
				editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_RUNTIME_ASCEND);
			}else if (item.getItemId() == R.id.menu_sort_runtime_descending) {
				editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_RUNTIME_DESCEND);
			}else if (item.getItemId() == R.id.menu_sort_watched_ascending) {
				editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_WATCHED_ASCEND);
			}else if (item.getItemId() == R.id.menu_sort_watched_descending) {
				editor.putInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_WATCHED_DESCEND);
			}

			editor.apply();

			return true;

		default:
			return super.onOptionsItemSelected(item);
		}
	}

	@Override
	public Loader<Cursor> onCreateLoader(int id, Bundle args) {
		if (id == LOADER_ID_SHOWS) {
			final String[] projection = {
				ShowsTable.COLUMN_ID,
				ShowsTable.COLUMN_NAME,
				ShowsTable.COLUMN_STARRED,
				ShowsTable.COLUMN_BANNER_PATH,
				ShowsTable.COLUMN_RUNTIME,
				ShowsTable.COLUMN_RATING,
                ShowsTable.COLUMN_COMMENT,
                ShowsTable.COLUMN_GENRE,
                ShowsTable.COLUMN_STATUS,
				ShowsTable.COLUMN_LAST_VIEWED
			};
			return new CursorLoader(getActivity(),
			                        ShowsProvider.CONTENT_URI_SHOWS,
			                        projection,
			                        null,
			                        null,
			                        ShowsTable.COLUMN_STARRED + " DESC," +
			                        ShowsTable.COLUMN_NAME + " ASC");

		} else if (id == LOADER_ID_EPISODES) {
			final String[] projection = {
				EpisodesTable.COLUMN_SHOW_ID, EpisodesTable.COLUMN_NAME, EpisodesTable.COLUMN_SEASON_NUMBER, EpisodesTable.COLUMN_FIRST_AIRED, EpisodesTable.COLUMN_WATCHED
			};
			final String selection =
				String.format("%s!=?", EpisodesTable.COLUMN_SEASON_NUMBER);
			final String[] selectionArgs = {
				"0"
			};
			return new CursorLoader(getActivity(),
			                        ShowsProvider.CONTENT_URI_EPISODES,
			                        projection,
			                        selection,
			                        selectionArgs,
			                        null);

		} else {
			throw new IllegalArgumentException("invalid loader id");
		}
	}

	@Override
	public void onLoadFinished(Loader<Cursor> loader, Cursor data) {
		switch (loader.getId()) {
		case LOADER_ID_SHOWS:
			showsData = data;
			listAdapter.swapShowsCursor(data);
			break;

		case LOADER_ID_EPISODES:
			episodesData = data;
			listAdapter.swapEpisodesCursor(data);
			break;
		}

		getActivity().supportInvalidateOptionsMenu();
	}

	@Override
	public void onLoaderReset(Loader<Cursor> loader) {
		onLoadFinished(loader, null);
	}

	@Override
	public void onListItemClick(ListView l, View v, int position, long id) {
		onShowSelectedListener.onShowSelected((int)id);
	}

	private void refreshAllShows() {
		if (showsData != null && showsData.moveToFirst()) {
			do {
				final int idColumnIndex =
					showsData.getColumnIndexOrThrow(ShowsTable.COLUMN_ID);

				final int id = showsData.getInt(idColumnIndex);

				final Intent intent = new Intent(getActivity(),
				                                 RefreshShowService.class);
				intent.putExtra("showId", id);

				getActivity().startService(intent);

			} while (showsData.moveToNext());
		}
	}

	private static class ShowsListAdapter
		extends BaseAdapter
		implements SharedPreferences.OnSharedPreferenceChangeListener
	{
		private static final String TAG = ShowsListAdapter.class.getName();
		private Context context;
		private Cursor showsCursor;
		private int filter;
		private int sort;
		private EpisodesCounter episodesCounter;
		private String genreType;

		// list of shows to be displayed with current filter. maps from
		// the show's position in the list to its position in the cursor.
		private List<Integer> filteredShows;

		public ShowsListAdapter(Context context, Cursor showsCursor, Cursor episodesCursor) {
			this.context = context;

			episodesCounter = new EpisodesCounter(EpisodesTable.COLUMN_SHOW_ID);
			episodesCounter.swapCursor(episodesCursor);

			final SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(context);
			prefs.registerOnSharedPreferenceChangeListener(this);
			filter = prefs.getInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_ALL);
			sort= prefs.getInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_ALPHA_ASCEND);

			filteredShows = new ArrayList<Integer>();
			swapShowsCursor(showsCursor);
		}

		public void swapShowsCursor(Cursor showsCursor) {
			this.showsCursor = showsCursor;

			updateFilter();
			notifyDataSetChanged();
		}

		public void swapEpisodesCursor(Cursor episodesCursor) {
			episodesCounter.swapCursor(episodesCursor);

			if (showsCursor != null) {
				updateFilter();
				notifyDataSetChanged();
			}
		}

		@Override
		public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,
		                                      String key) {
			if (key.equals(KEY_PREF_SHOWS_FILTER)) {
				filter = sharedPreferences.getInt(KEY_PREF_SHOWS_FILTER, SHOWS_FILTER_ALL);
				Log.i(TAG, "FILTER chosen:"+ filter);

				genreType = sharedPreferences.getString("Genre", "");
				Log.i(TAG, "Pref: " + sharedPreferences.getString("Genre", ""));

				if (showsCursor != null) {
					updateFilter();
					notifyDataSetChanged();
				}
			} else if (key.equals(KEY_PREF_SHOWS_SORT)) {
				sort= sharedPreferences.getInt(KEY_PREF_SHOWS_SORT, SHOWS_SORT_ALPHA_ASCEND);
				Log.i(TAG, "SORT chosen:" + sort);
				if (showsCursor != null) {
					updateSort();
					notifyDataSetChanged();
				}
			}
		}

		public static HashMap<String, Integer> sortByValue(HashMap<String, Integer> hm) {
			// Create a list from elements of HashMap
			List<Map.Entry<String, Integer> > list =
					new LinkedList<Map.Entry<String, Integer> >(hm.entrySet());

			// Sort the list
			Collections.sort(list, new Comparator<Map.Entry<String, Integer> >() {
				public int compare(Map.Entry<String, Integer> o1,
								   Map.Entry<String, Integer> o2)
				{
					return (o1.getValue()).compareTo(o2.getValue());
				}
			});

			// put data from sorted list to hashmap
			HashMap<String, Integer> temp = new LinkedHashMap<String, Integer>();
			for (Map.Entry<String, Integer> aa : list) {
				temp.put(aa.getKey(), aa.getValue());
			}
			return temp;
		}

		private List<String> getNamesBySort(List<String> showNames,HashMap<String, Integer> unsortedMap) {
            Map<String, Integer> sorted = sortByValue(unsortedMap);
            showNames.clear();
            for (Map.Entry<String, Integer> entry : sorted.entrySet()) {
                showNames.add(entry.getKey());
            }
            return  showNames;
        }



		private void updateSort() {
			//Technically there's a more dynamic way with requerying DB, requires more work
			List<Integer> sortedShows = new ArrayList<Integer>();
			List<String> showNames = new ArrayList<String>();
			List<String> viewDates = new ArrayList<String>();
			List<String> unwatchedShows = new ArrayList<String>();

			HashMap<String, Integer> runTimeMap = new HashMap<String, Integer>();
			HashMap<String, Integer> ratingMap = new HashMap<String, Integer>();
			HashMap<String, String> watchedMap = new HashMap<String, String>();
			Hashtable<String, Integer> showMap = new Hashtable<String, Integer>();

            //Goes through available filtered shows
			for (Integer i:filteredShows) {
				showsCursor.moveToPosition(i);
				String name = showsCursor.getString(1);
				final int runtimeColumnIndex = showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_RUNTIME);
				final int ratingColumnIndex = showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_RATING);
				final int watchedColumnIndex = showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_LAST_VIEWED);

				String date = showsCursor.getString(watchedColumnIndex);
				Integer runTime = Integer.valueOf(showsCursor.getString(runtimeColumnIndex));
				Integer rating = Integer.valueOf(showsCursor.getString(ratingColumnIndex));

				//Dealing with shows with no watch dates
				if (date.isEmpty()) {
					unwatchedShows.add(name);
				} else {
					viewDates.add(date);
					watchedMap.put(date, name);
				}

				runTimeMap.put(name, runTime);
				ratingMap.put(name, rating);
				showMap.put(name, i);

				showNames.add(name);

			}



			if (sort == SHOWS_SORT_RUNTIME_ASCEND || sort == SHOWS_SORT_RUNTIME_DESCEND) {
				showNames = getNamesBySort(showNames,runTimeMap);
			} else if (sort == SHOWS_SORT_RATING_ASCEND|| sort == SHOWS_SORT_RATING_DESCEND) {
				showNames = getNamesBySort(showNames,ratingMap);
			}else if (sort == SHOWS_SORT_ALPHA_ASCEND|| sort == SHOWS_SORT_ALPHA_DESCEND) {
				Collections.sort(showNames);
			}else if (sort == SHOWS_SORT_WATCHED_ASCEND|| sort == SHOWS_SORT_WATCHED_DESCEND) {
				//Does reverse sort by default because most recent would have the later date first
				Collections.sort(viewDates, new Comparator<String>() {
					DateFormat f = new SimpleDateFormat("MM/dd/yyyy hh:mm:ss");
					@Override
					public int compare(String o1, String o2) {
						try {
							return f.parse(o1).compareTo(f.parse(o2));
						} catch (ParseException e) {
							throw new IllegalArgumentException(e);
						}
					}
				});
				showNames.clear();
				showNames.addAll(unwatchedShows);
				for (String date : viewDates) {
					showNames.add(watchedMap.get(date));
				}
			}

			switch (sort) {

				case SHOWS_SORT_ALPHA_DESCEND:
				case SHOWS_SORT_RUNTIME_DESCEND:
				case SHOWS_SORT_RATING_DESCEND:
				case SHOWS_SORT_WATCHED_ASCEND:
					Collections.reverse(showNames);
					break;
			}

			for (String name : showNames) {
				int index = showMap.get(name);
				sortedShows.add(index);
			}

			filteredShows.clear();
			filteredShows = sortedShows;

		}

		private void updateFilter() {

			filteredShows.clear();

			if (showsCursor == null || !showsCursor.moveToFirst()) {
				return;
			}

			do {

				switch (filter) {
					case SHOWS_FILTER_STARRED:
						final int starredColumnIndex = showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_STARRED);
						if (showsCursor.getInt(starredColumnIndex) > 0) {
							filteredShows.add(showsCursor.getPosition());
						}
						break;

					case SHOWS_FILTER_UNCOMPLETED:
						final int idColumnIndex = showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_ID);
						final int id = showsCursor.getInt(idColumnIndex);

						if (episodesCounter.getNumWatchedEpisodes(id) < episodesCounter.getNumAiredEpisodes(id)) {
							filteredShows.add(showsCursor.getPosition());
						}
						break;

					case SHOWS_FILTER_GENRE:
					case SHOWS_FILTER_GENRE_ACTION:
					case SHOWS_FILTER_GENRE_ADVENTURE:
					case SHOWS_FILTER_GENRE_ANIMATION:
					case SHOWS_FILTER_GENRE_COMEDY:
					case SHOWS_FILTER_GENRE_CRIME:
					case SHOWS_FILTER_GENRE_DRAMA:
					case SHOWS_FILTER_GENRE_FAMILY:
					case SHOWS_FILTER_GENRE_FANTASY:
					case SHOWS_FILTER_GENRE_HORROR:
					case SHOWS_FILTER_GENRE_MYSTERY:
					case SHOWS_FILTER_GENRE_ROMANCE:
					case SHOWS_FILTER_GENRE_SCIFI:
					case SHOWS_FILTER_GENRE_SUSPENSE:
					case SHOWS_FILTER_GENRE_THRILLER:
						final int genreColumnIndex = showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_GENRE);
						String genreList = showsCursor.getString(genreColumnIndex);
						String[] genreArray= genreList.split("[, ]+");

						for(String genreItem : genreArray) {
							if(genreItem.equals(genreType)) {
								filteredShows.add(showsCursor.getPosition());
								break;
							}
						}
						break;

					case SHOWS_FILTER_AIRING:
						final int airingColumnIndex = showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_STATUS);
						String status = showsCursor.getString(airingColumnIndex);
						Log.i("ShowsListFragment", status);
						if (status.equals("Continuing")) {
							filteredShows.add(showsCursor.getPosition());
						}
						break;

					default:
						filteredShows.add(showsCursor.getPosition());
						break;
				}
			} while (showsCursor.moveToNext());
			updateSort();
		}

		@Override
		public int getCount() {
			if (showsCursor == null) {
				return 0;
			} else {
				return filteredShows.size();
			}
		}

		@Override
		public Object getItem(int position) {
			return null;
		}

		@Override
		public long getItemId(int position) {
			showsCursor.moveToPosition(filteredShows.get(position));

			final int idColumnIndex =
				showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_ID);
			return showsCursor.getInt(idColumnIndex);
		}

		@Override
		public View getView(int position,
		                    View convertView,
		                    ViewGroup parent) {

			final LayoutInflater inflater = LayoutInflater.from(context);
			if(convertView == null) {
				convertView = inflater.inflate(R.layout.shows_list_item,
				                               parent,
				                               false);
			}

			showsCursor.moveToPosition(filteredShows.get(position));

			final int idColumnIndex =
				showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_ID);
			final int id = showsCursor.getInt(idColumnIndex);

            final int ratingColumnIndex =
                    showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_RATING);
            final int rating = showsCursor.getInt(ratingColumnIndex);
            Log.d("ShowsListFragment", String.format("Rating: %s", rating));

            final int commentColumnIndex =
                    showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_COMMENT);
            final String comment = showsCursor.getString(commentColumnIndex);
            Log.d("ShowsListFragment", String.format("Comment: %s", comment));

			final ContentResolver contentResolver =
				context.getContentResolver();

			final TextView nameView =
				(TextView)convertView.findViewById(R.id.show_name_view);
			final int nameColumnIndex =
				showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_NAME);
			final String name = showsCursor.getString(nameColumnIndex);
			nameView.setText(name);

			final ImageView bannerView =
				(ImageView)convertView.findViewById(R.id.banner_view);
			final int bannerPathColumnIndex =
				showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_BANNER_PATH);
			final String bannerPath = showsCursor.getString(bannerPathColumnIndex);

			bannerView.setImageResource(R.drawable.blank_show_banner);
			if (bannerPath != null && !bannerPath.equals("")) {
				final String bannerUrl =
					String.format("http://thetvdb.com/banners/%s", bannerPath);

				final DisplayImageOptions options =
					new DisplayImageOptions.Builder()
					.cacheInMemory(true)
					.cacheOnDisk(true)
					.build();
				ImageLoader.getInstance().displayImage(bannerUrl,
				                                       bannerView,
				                                       options);
			}

			final ToggleButton starredToggle =
				(ToggleButton)convertView.findViewById(R.id.show_starred_toggle);
			final int starredColumnIndex =
				showsCursor.getColumnIndexOrThrow(ShowsTable.COLUMN_STARRED);
			final boolean starred =
				showsCursor.getInt(starredColumnIndex) > 0 ? true : false;

			starredToggle.setOnCheckedChangeListener(null);
			starredToggle.setChecked(starred);

			starredToggle.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
					public void onCheckedChanged(CompoundButton buttonView,
					                             boolean isChecked) {
						final AsyncQueryHandler handler =
							new AsyncQueryHandler(contentResolver) {};
						final ContentValues showValues = new ContentValues();
						showValues.put(ShowsTable.COLUMN_STARRED, isChecked);

						final Uri showUri =
							Uri.withAppendedPath(ShowsProvider.CONTENT_URI_SHOWS,
							                     String.valueOf(id));
						handler.startUpdate(0,
						                    null,
						                    showUri,
						                    showValues,
						                    null,
						                    null);
					}
				});

			final int numAired = episodesCounter.getNumAiredEpisodes(id);
			final int numWatched = episodesCounter.getNumWatchedEpisodes(id);
			final int numUpcoming = episodesCounter.getNumUpcomingEpisodes(id);

			final ProgressBar progressBar =
				(ProgressBar)convertView.findViewById(R.id.show_progress_bar);
			progressBar.setMax(numAired);
			progressBar.setProgress(numWatched);

			final TextView watchedCountView =
				(TextView)convertView.findViewById(R.id.watched_count_view);
			String watchedCountText = context.getString(R.string.watched_count,
			                                            numWatched,
			                                            numAired);
			if (numUpcoming != 0) {
				watchedCountText += " " +
					context.getString(R.string.upcoming_count,
					                  numUpcoming);
			}

			//watchedCountText +=  ", given a rating of " + rating + "/10";
			watchedCountView.setText(watchedCountText);

			return convertView;
		}
	}
}
