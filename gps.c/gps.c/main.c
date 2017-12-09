#include "libraries.h"

//Example of a string received from, for instance, an USART
unsigned char gps_buffer[1000] = "$GPRMC,000324.00,V,0000.0000,N,00000.0000,E,00.00,000.0,,,,N*43\r\n$GPGSV,1,1,00*79\r\n$GPRMC,223051.00,A,4110.7757,N,00836.4317,W,00.00,300.0,051217,,,A*42\r\n", latitude_compass, longitude_compass;
int gps_hour, gps_min, gps_sec, gps_day, gps_month, gps_year;
float latitude_coordinate, longitude_coordinate;

static void gpsParser();
static void splitInfo(int i);
static void provisoryString(int x, int k, int i, char* string);

void main()
{
	clock_t begin = clock();
	gpsParser();
	printf("Time:%dh%dmin%ds\n",gps_hour,gps_min,gps_sec);
	printf("Date:%d/%d/%d\n",gps_day,gps_month,gps_year);
	printf("Latitude:%c,%f\n", latitude_compass, latitude_coordinate);
	printf("Longitude:%c,%f\n", longitude_compass, longitude_coordinate);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time spent: %.15f",time_spent);
	getchar();
}


static void gpsParser()
{
	int i = 0, j = 0;
	char label[7] = "$GPRMC";
	
	char provisory[7];
	memset(provisory, 0, sizeof provisory);

	//xSemaphoreTake(binSemaphore, portMAX_DELAY);

	for (i = 0; i<1000; i++)
	{
		if (i + 75 < 1000)
		{
			if (gps_buffer[i] == '$')
			{
				for (j = 0; j < 6; j++)
				{
					provisory[j] = gps_buffer[i + j];
				}
				if (strcmp(provisory, label) == 0)
				{
					i = i + 5;
					if (gps_buffer[i + 12] == 'V')
					{
						continue;
					}
					else
					{
						splitInfo(i);
						memset(provisory, 0, sizeof provisory);
					}
				}
			}
		}
		else
			return;
	}
}

static void splitInfo(int i)
{
	int x = 0, k = 0;	
	char provisory_time[2], geographic[10];

	provisoryString(2, 2, i, provisory_time);
	gps_hour = atoi(provisory_time);
	provisoryString(4, 2, i, provisory_time);
	gps_min = atoi(provisory_time);
	provisoryString(6, 2, i, provisory_time);
	gps_sec = atoi(provisory_time);

	provisoryString(14, 9, i, geographic);
	latitude_coordinate = atof(geographic);
	latitude_compass = gps_buffer[i + 24];

	provisoryString(26,9,i,geographic);
	longitude_coordinate =atof(geographic);
	longitude_compass = gps_buffer[i + 37 ];
	
	provisoryString(51, 2, i, provisory_time);
	gps_day = atoi(provisory_time);
	provisoryString(53, 2, i, provisory_time);
	gps_month = atoi(provisory_time);
	provisoryString(55, 2, i, provisory_time);
	gps_year = atoi(provisory_time);
}

static void provisoryString(int x, int k, int i, char* string)
{
	for (int j = 0; j < k; j++)
	{
		string[j] = gps_buffer[i + x];
		x++;
	}
}