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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <curl/curl.h>
#include <jansson.h>
#include <string.h>
#include <errno.h>
#include "apihandler.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void* user_p)
{

	size_t realsize = size * nmemb;
	size_t new_capacity=0;
	size_t required_size;
	
	dataBuffer *buff = user_p;
	
	required_size = buff->size + realsize + 1;

	if (new_capacity<required_size)
	{
		new_capacity=buff->capacity*2;

		if (new_capacity==0) new_capacity=4096;

		if (required_size > new_capacity ) new_capacity=required_size;
		
		char *dataPtr = realloc(buff->data,new_capacity);

		if (dataPtr!=NULL)
		{
			buff->data=dataPtr;
			buff->capacity = new_capacity;
		}
		else
		{
			fprintf(stderr,"WriteMemoryCallback() error: realloc() failed!\nError:%s. (Error code :%d)\n",strerror(errno),errno);
			exit(EXIT_FAILURE);
		}

	}
		memcpy(&(buff->data[buff->size]),contents, realsize);
		buff->size+=realsize;
		buff->data[buff->size] = 0;
		
	return realsize;
}


dataBuffer *curl_perform(CURL *curl_handler,const char *_url)
{
	CURLcode result;
	
	dataBuffer *data = calloc(1,sizeof(dataBuffer));
	
	if (data==NULL)
	{
		fprintf(stderr,"curl_perform() error : malloc() failed!\nError:%s.\nError code:%d\n",strerror(errno),errno);
		exit(EXIT_FAILURE);
	}
	else
	{
		data->data=NULL;
		data->size = 0;
		data->capacity=0;
	}
	
	curl_easy_setopt(curl_handler,CURLOPT_URL,_url);
	curl_easy_setopt(curl_handler,CURLOPT_USERAGENT,"where_is_iss_at/1.0/(Github:Where is ISS at?)");
	curl_easy_setopt(curl_handler,CURLOPT_CONNECTTIMEOUT,30L);
	curl_easy_setopt(curl_handler,CURLOPT_TIMEOUT,30L);
	curl_easy_setopt(curl_handler,CURLOPT_FOLLOWLOCATION,1L);
	curl_easy_setopt(curl_handler,CURLOPT_WRITEFUNCTION,WriteMemoryCallback);
	curl_easy_setopt(curl_handler,CURLOPT_WRITEDATA,data);
		
	result=curl_easy_perform(curl_handler);
	
	if(result!=CURLE_OK)
	{
		fprintf(stderr,"curl_perform() error:Error downloading file!\nError:%s.\nError code:%d.\n",curl_easy_strerror(result),result);
		curl_easy_cleanup(curl_handler);
		exit(EXIT_FAILURE);
	}
	
	return data;
}

issData *get_iss_position(CURL *curl_handler)
{	
	sleep(.5);
	char *iss_url="https://api.wheretheiss.at/v1/satellites/25544";
	dataBuffer *iss_data=curl_perform(curl_handler,iss_url);
	time_t time;
	
	issData *iss = calloc(1,sizeof(issData));
	if (iss==NULL)
	{
		fprintf(stderr,"get_iss_position() error:calloc() failed!\n");
		exit(EXIT_FAILURE);
	}
	json_error_t error;
	json_t *root;
	json_t *timestamp;
	json_t *longitude;
	json_t *latitude;
	json_t *altitude;
	json_t *velocity;
	
	if ((root=json_loads((const char*)iss_data->data,1,&error))!=NULL)
	{
		timestamp = json_object_get(root,"timestamp");
		longitude = json_object_get(root,"longitude");
		latitude  = json_object_get(root,"latitude");
		altitude  = json_object_get(root,"altitude");
		velocity  = json_object_get(root,"velocity");
		
		if (!json_is_null(timestamp) && (timestamp!=NULL))
		{
			if (json_is_number(timestamp))
			{	
				iss->timestamp=json_number_value(timestamp);
			}
			else
			{
				fprintf(stderr,"get_iss_position() error: JSON parsing error!\nTimestamp variable is not a number!\n");
				json_decref(root);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			fprintf(stderr,"get_iss_position() error: JSON parsing failed!\ntimestamp variable is empty");
			json_decref(root);
			exit(EXIT_FAILURE);
		}
		
		if (!json_is_null(longitude) && (longitude!=NULL))
		{
			if (json_is_number(longitude))
			{
				iss->longitude = json_number_value(longitude);
			}
			else
			{
				fprintf(stderr,"get_iss_position() error: JSON parsing error!\nLongtitude variable is not a number!\n");
				json_decref(root);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			fprintf(stderr,"get_iss_position() error: JSON parsing failed!\nlongitude is empty!\n");
			json_decref(root);
			exit(EXIT_FAILURE);			
		}

		if(!json_is_null(latitude) && (latitude!=NULL))
		{
			if (json_is_number(latitude))
			{
				iss->latitude = json_number_value(latitude);
			}
			else
			{
				fprintf(stderr,"get_iss_position() error: JSON parsing error!\nLatitude variable is not a number!\n");
				json_decref(root);
				exit(EXIT_FAILURE);
			}			
		}
		else
		{
			fprintf(stderr,"get_iss_position() error: JSON parsing failed!\nlatitude variable is empty!\n");
			json_decref(root);
			exit(EXIT_FAILURE);
		}
		
		if (!json_is_null(velocity) && (velocity!=NULL))
		{
			if (json_is_number(velocity))
			{	
				iss->velocity = json_number_value(velocity);
			}
			else
			{
				fprintf(stderr,"get_iss_position() error: JSON parsing error!\nVelocity object is not a number!\n");
				json_decref(root);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			fprintf(stderr,"get_iss_position() error: JSON parsing failed!\nVelocity variable is empty!\n");
		}
		
		if (!json_is_null(altitude) && (longitude!=NULL))
		{
			if (json_number_value(altitude))
			{
				iss->altitude = json_number_value(altitude);
			}
			else
			{
				fprintf(stderr,"get_iss_position() error: JSON parsing error!\nAltitude object is not a string!\n");
				json_decref(root);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			fprintf(stderr,"get_iss_position() error: JSON parsing failed!\nAltitude variable is empty\n");
			json_decref(root);
			exit(EXIT_FAILURE);
		}
			json_decref(root);
	}
	else
	{
		fprintf(stderr,"get_iss_position() error:JSON parsing is failed!\nError at line %d.\nError:%s\n",error.line,error.text);
		if (root!=NULL)
		{
			json_decref(root);
		}
		exit(EXIT_FAILURE);
	}
	
	time=(time_t)iss->timestamp;
	struct tm *local_Time = localtime(&time);
	iss->realtime.tm_year = 1900+local_Time->tm_year;
	iss->realtime.tm_mon  = local_Time->tm_mon+1;
	iss->realtime.tm_mday = local_Time->tm_mday;
	iss->realtime.tm_hour = local_Time->tm_hour;
	iss->realtime.tm_min = local_Time->tm_min;
	iss->realtime.tm_sec = local_Time->tm_sec;
	
	char geocoding_url[1024];	
	
	snprintf(geocoding_url,sizeof(geocoding_url),"https://nominatim.openstreetmap.org/reverse?format=json&lat=%f&lon=%f",iss->latitude,iss->longitude);
	sleep(1);
	dataBuffer *iss_location = curl_perform(curl_handler,geocoding_url);
	json_t *location_root;
	json_t *osm_error;
	json_t *address;
	json_t *display_name;
	json_t *country;
	json_t *city;
	json_t *town;
	json_t *region;
	json_t *iso_3166_2;
	
	if ((location_root=json_loads((const char*)iss_location->data,0,&error))!=NULL)
	{
		osm_error=json_object_get(location_root,"error");
 
		if (!json_is_null(osm_error) && osm_error!=NULL)
		{
			iss->isOnSea=1;
		}
		else
		{
			display_name = json_object_get(location_root,"display_name");
			address = json_object_get(location_root,"address");
			country = json_object_get(address,"country");
			city = json_object_get(address,"city");
			town = json_object_get(address,"town");
			region = json_object_get(address,"region");
			iso_3166_2  = json_object_get(address,"ISO3166-2-lvl4");
			
			if (display_name!=NULL)
			{
				if (iss->display_name==NULL)
				{
					iss->display_name = strdup(json_string_value(display_name));
				}
			}
			
			if (address!=NULL)
			{
				if (country!=NULL)
				{							
					if (iss->country==NULL)
					{
						iss->country = strdup(json_string_value(country));
					}
				}
				
				if (city!=NULL)
				{
					if (iss->city==NULL)
					{
						iss->city = strdup(json_string_value(city));
					}
				}
				
				if (town!=NULL)
				{
					if (iss->town==NULL)
					{
						iss->town = strdup(json_string_value(town));
					}
				}
				
				if (region!=NULL)
				{
					if (iss->region==NULL)
					{
						iss->region = strdup(json_string_value(region));
					}
				}
				
				if (iso_3166_2!=NULL)
				{
					if (iss->iso_3166_2_lvl4==NULL)
					{
						iss->iso_3166_2_lvl4 = strdup(json_string_value(iso_3166_2));
					}
				}
			}
			 

		}
		
		json_decref(location_root);
	}
	else
	{
		fprintf(stderr,"get_iss_position() error:Parsing of OpenMap JSON is failed!\nCouldn't get data!\n");
		exit(EXIT_FAILURE);
	}
	
	curl_perform_cleanup(&iss_data);
	curl_perform_cleanup(&iss_location);

	return iss;
}

void curl_perform_cleanup(dataBuffer **buffer)
{
	if (buffer == NULL || *buffer == NULL) 
		return;
	free((void*)(*buffer)->data);
	free((void*)(*buffer));
}
void free_issData(issData **i)
{
    if (i == NULL || *i == NULL)
        return;
        
    free((void*)(*i)->display_name);
    free((void*)(*i)->country);
    free((void*)(*i)->city);
    free((void*)(*i)->town);
    free((void*)(*i)->region);
    free((void*)(*i)->iso_3166_2_lvl4);
    free(*i);
    *i = NULL; 
}

