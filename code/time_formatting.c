#include <stdio.h>
#include <time.h>

#include "time_formatting.h"

void print_time(long int time_passed)
{
	/* Less than a millisecond */
	if(!(1000 * time_passed / CLOCKS_PER_SEC))
	{
		printf("%d\xCE\xBCs", 1000000 * time_passed / CLOCKS_PER_SEC);
		return;
	}

	/* Less than a second */
	if(!(time_passed / CLOCKS_PER_SEC))
	{
		printf("%dms", 1000 * time_passed / CLOCKS_PER_SEC);
		return;
	}

	/* Less than a minute */
	if(!(time_passed / CLOCKS_PER_SEC / 60))
	{
		printf("%.1fs", time_passed / (double) CLOCKS_PER_SEC);
		return;
	}

	/* Less than an hour */
	if(!(time_passed / CLOCKS_PER_SEC / 60 / 60))
	{
		printf(
			"%02d:%02d",
			time_passed / CLOCKS_PER_SEC / 60,
			time_passed / CLOCKS_PER_SEC % 60
		);
		return;
	}

	/* Over an hour */
	printf(
		"%02d:%02d:%02d",
		time_passed / CLOCKS_PER_SEC / 60 / 60,
		time_passed / CLOCKS_PER_SEC / 60 % 60,
		time_passed / CLOCKS_PER_SEC % 60
	);
	return;

	printf("Unable to format time");
}
