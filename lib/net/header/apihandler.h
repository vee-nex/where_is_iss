/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <stdlib.h>
#include <time.h>
#include <curl/curl.h>

#ifndef APIHANDLER_H
#define APIHANDLER_H
typedef struct
{
	char *data;
	size_t size;
	size_t capacity;
}dataBuffer;

typedef struct
{
	double timestamp;
	double longitude;
	double latitude;
	double altitude;
	double velocity;
	struct tm realtime;
	
	char *display_name;
	char *country;
	char *city;
	char *town;
	char *region;
	char *iso_3166_2_lvl4;
	short isOnSea;
}issData;

dataBuffer *curl_perform(CURL *curl_handler,const char *_url);
issData *get_iss_position(CURL *curl_handler);
void curl_perform_cleanup(dataBuffer **buffer);
void free_issData(issData **i);


#endif
