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
#include "apihandler.h"

int main(void)
{

	CURL *curl_handler=curl_easy_init();
	while (1)
	{		
		issData *issPos=get_iss_position(curl_handler);
		if (issPos->isOnSea != 1)
		{
			printf("\tDate:%d.%d.%d %d:%d:%d\n",issPos->realtime.tm_mday,issPos->realtime.tm_mon,issPos->realtime.tm_year,issPos->realtime.tm_hour,issPos->realtime.tm_min,issPos->realtime.tm_sec);
			printf("\nISS'S REAL TIME LOCATION\n");
			printf("\nISS'S Longtitude: %f\n",issPos->longitude);
			printf("ISS Latitude:   %f\n",issPos->latitude);
			printf("ISS Altitude:   %f KM/H\n",issPos->altitude);
			printf("ISS Velocity:   %f KM/H\n",issPos->velocity);
			printf("\nISS is passing over :%s\n",issPos->country);
			printf("\nNAME :%s\n",issPos->display_name);
			printf("\nCity :%s\n",issPos->city);
			printf("\nTown :%s\n",issPos->town);
			printf("\nRegion :%s\n",issPos->region);		
			printf("\niso_3166_2_lvl4 :%s\n",issPos->iso_3166_2_lvl4);
		}
		else
		{
			
			printf("\tISS FLYING OVER SEA!\n");
			printf("\nREAL TIME LOCATION\n");
			printf("\nISS Longtitude: %f\n",issPos->longitude);
			printf("ISS Latitude:   %f\n",issPos->latitude);
			printf("ISS Altitude:   %f KM/H\n",issPos->altitude);
			printf("ISS Velocity:   %f KM/H\n",issPos->velocity);
			

		}
		printf("\033[2J");
		printf("\033[H");
		free_issData(&issPos);
		
	}
	curl_easy_cleanup(curl_handler);
	return 0;
}
