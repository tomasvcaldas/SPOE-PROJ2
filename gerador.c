#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_LENGTH 10

float clockUnit;
float genTime;
int id = 0;


typedef enum {NORTH, SOUTH, EAST, WEST} Direction;


typedef struct {
  Direction direction;
  int id;
  float parkedTime;
  char fifoName[FIFO_LENGTH] ;
} Vehicle;

void* vehicleFunc(void *arg){
	void *ret = NULL;
	Vehicle vehicle = *(Vehicle*) arg;
	mkfifo(vehicle.fifoName,0660);
	int fdWrite;

	printf("Entered in thread \n");

	switch(vehicle.direction){
	case NORTH:
		fdWrite = open("fifoN",O_WRONLY | O_NONBLOCK);
		break;
	case SOUTH:
		fdWrite = open("fifoS",O_WRONLY | O_NONBLOCK);
		break;
	case WEST:
		fdWrite = open("fifoW",O_WRONLY | O_NONBLOCK);
		break;
	case EAST:
		fdWrite = open("fifoE",O_WRONLY | O_NONBLOCK);
		break;
	}


	if(fdWrite != -1){
		write(fdWrite,&vehicle,sizeof(Vehicle));
		close(fdWrite);
	}

	printf("Passed the fdWrite \n");



	return ret;
}

int genVehicle(){
	Vehicle vehicle;
	vehicle.id = id;
	id++;

	pthread_t mainVehicle;

	int dir = rand() %  4;
	switch(dir){
	case 0:
		vehicle.direction = NORTH;
		vehicle.fifoName = "fifoN";
		break;
	case 1:
		vehicle.direction = SOUTH;
		vehicle.fifoName = "fifoS";
		break;
	case 2:
		vehicle.direction = EAST;
		vehicle.fifoName = "fifoE";
		break;
	case 3:
		vehicle.direction = WEST;
		vehicle.fifoName = "fifoW";
		break;
	}

	float pTime = ((rand() %10)+1) * clockUnit;
	vehicle.parkedTime = pTime;


	if(pthread_create(&mainVehicle, NULL, vehicleFunc,&vehicle) != 0) perror("Can't create Main Vehicle thread! \n");

	int nextCarTime;
	int park = rand() % 10;

	if(park < 2) nextCarTime= 2;
	else if(park < 5) nextCarTime = 1;
	else nextCarTime = 0;


	return nextCarTime;
}


int main(int argc, char* argv[]){

	if(argc != 3){
				perror("Invalid number of arguments! \n");
				exit(1);
			}

	srand(time(NULL));
	genTime = atoi(argv[0]);
	clockUnit = atoi(argv[1]);
	float numberOfTicks;
	int ticksForNextCar = 0;

	numberOfTicks = (genTime / clockUnit) *1000; // Number of events that are going to happen
	printf("Number of ticks: %f \n",numberOfTicks);

	do{
		if(ticksForNextCar == 0) ticksForNextCar = genVehicle();
		else ticksForNextCar--;
		usleep(clockUnit * 1000);
		numberOfTicks--;
	} while( numberOfTicks > 0);



	pthread_exit(NULL);
}
