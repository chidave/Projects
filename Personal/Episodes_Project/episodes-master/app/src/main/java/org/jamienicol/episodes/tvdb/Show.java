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

package org.jamienicol.episodes.tvdb;

import java.util.Date;
import java.util.List;

public class Show
{
	private int id;
	private String name;
	private String language;
	private String overview;
	private Date firstAired;
	private String bannerPath;
	private String fanartPath;
	private String posterPath;
	private String genre;
	private String contentRating;
	private String cast;
	private String runtime;
	private String imdb;
	private String status;
	private List<Episode> episodes;
	private int rating;
	private String lastViewed;

	public Show() {
		id = 0;
		name = "";
		overview = "";
		firstAired = null;
		bannerPath = "";
		fanartPath = "";
		posterPath = "";
		genre = "";
		contentRating = "";
		cast = "";
		runtime = "";
		imdb = "";
		status = "";
		episodes = null;
		rating = 0;
		lastViewed = "";
	}

	public int getId() {
		return id;
	}

	public void setId(int id) {
		this.id = id;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getOverview() {
		return overview;
	}

	public void setOverview(String overview) {
		this.overview = overview;
	}

	public Date getFirstAired() {
		return firstAired;
	}

	public void setFirstAired(Date firstAired) {
		this.firstAired = firstAired;
	}

	public String getLanguage() {
		return language;
	}

	public void setLanguage(String language) {
		this.language = language;
	}

	public String getGenre() {
		return genre;
	}

	public void setGenre(String genre) {
		this.genre = genre;
	}

	public String getContentRating() {
		return contentRating;
	}

	public void setContentRating(String contentRating) {
		this.contentRating = contentRating;
	}

	public String getCast() {
		return cast;
	}

	public void setCast(String cast) {
		this.cast = cast;
	}

	public String getRuntime() {
		return runtime;
	}

	public void setRuntime(String runtime) {
		this.runtime = runtime;
	}

	public String getImdb() {
		return imdb;
	}

	public void setImdb(String imdb) {
		this.imdb = imdb;
	}

	public String getStatus() {
		return status;
	}

	public void setStatus(String status) {
		this.status = status;
	}

	public String getBannerPath() {
		return bannerPath;
	}

	public void setBannerPath(String bannerPath) {
		this.bannerPath = bannerPath;
	}

	public String getFanartPath() {
		return fanartPath;
	}

	public void setFanartPath(String fanartPath) {
		this.fanartPath = fanartPath;
	}

	public String getPosterPath() {
		return posterPath;
	}

	public void setPosterPath(String posterPath) {
		this.posterPath = posterPath;
	}

	public List<Episode> getEpisodes() {
		return episodes;
	}

	public void setEpisodes(List<Episode> episodes) {
		this.episodes = episodes;
	}

	public int getRating() {
		return rating;
	}

	public void setRating(int rating) {
		this.rating = rating;
	}

	public String getLastViewed() {
		return lastViewed;
	}

	public void setLastViewed(String lastViewed) {
		this.lastViewed = lastViewed;
	}
}
