#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 
//global variables init
#define globalPatientNum 15
#define globalUnitNum 8
#define globalCapacityUnit 3
//main functions initilazed
void *patient(void *num);
void *unit(void *);
void randwait();
//sem for unit capacity check
sem_t unitCapacity;
//check healthcare staff for units
sem_t healthCareStaff;
//sems for critical zones in functions
sem_t criticalZonePatient;
sem_t criticalZoneUnit;
//main flag for globalPatientNum
int allDone = 0;
int patientCounterThree=0; // Counter until three patient
int patientCounter=0; //main counter for patient 
int unitID=0; //hold unit's id
int flag=0; 
int main(int argc, char *argv[])
{
	pthread_t btid[globalUnitNum];
	pthread_t tid[globalPatientNum];
    int listPatient[globalPatientNum]; 
    int listUnit[globalUnitNum];
	for (int i = 0; i < globalUnitNum; i++) {
		listUnit[i] = i;
	}
	for (int i = 0; i < globalPatientNum; i++) {
		listPatient[i] = i;
	}
	//creating main semaphores
	sem_init(&unitCapacity, 0, globalCapacityUnit);
	sem_init(&healthCareStaff, 0, 0);
	sem_init(&criticalZonePatient, 0, 1);
	sem_init(&criticalZoneUnit, 0, 1);
	//initliazing the units
	for(int i=0;i<globalUnitNum;i++){
		pthread_create(&btid[i], NULL, unit, (void *)&listUnit[i]);
	}
	//initiliaze the patients
	for (int i = 0; i < globalPatientNum; i++) {
		pthread_create(&tid[i], NULL, patient, (void *)&listPatient[i]);
	}
	// Join each of the threads to wait for them to finish.
	for (int i = 0; i < globalPatientNum; i++) {
		pthread_join(tid[i],NULL);
	}
    allDone=1; // end the main process loop
	for(int i=0;i<globalUnitNum;i++){ //post all of the unit's staffs so program is able to finish
		sem_post(&healthCareStaff); 
	}
	for(int i=0;i<globalUnitNum;i++){
		pthread_join(btid[i],NULL);
	}
	system("PAUSE");   
	return 0;
}
void *patient(void *number) {
	int num = *(int *)number;
	sem_wait(&criticalZonePatient); //in the same time only one patient can join
    ////critical area
	printf("Patient %d arrived to the hospital.\n", num);
	patientCounter++;
	sem_wait(&unitCapacity); //sem waits for until room capacity is done
	printf("Patient %d joining unit %d.\n", num,unitID);
	sem_post(&healthCareStaff);
	if(patientCounterThree==0){ 
		printf("Staff %d is cleaning and ventilating unit %d for prevent COVID-19.\n", unitID,unitID);
	}
	patientCounterThree++;
	printf("Patient %d is waiting for COVID-19 test.\n", num);
	randwait();
    ////end of critical area
	sem_post(&criticalZonePatient); // after that new patiens are able to join
}
void *unit(void *unitIdNumber)
{
	while (!allDone) { 
		sem_wait(&healthCareStaff); //critical zone for staff
		if (!allDone)  //if statement to control main flag is done
		{ 
			sem_wait(&criticalZoneUnit); //critical zone for unit
			if(3-patientCounterThree > 0 && 3-patientCounterThree <3){
				printf("Healthcare Staff called that %d patients are able to join unit %d for COVID-19 testing.\n",3-patientCounterThree,unitID); //staff is calling patients
			}
			if(patientCounterThree==3)
            { 
				printf("The unit %d full and patients' tests done, now patients are leaving the unit %d.\n",unitID,unitID);
				printf("The unit %d is cleaning and ventilating.\n",unitID);
				int staticPatientCounterThree = patientCounterThree;  //assigning to temp variable because patientCounterThree will decrease in loop
				for(int i=0;i<staticPatientCounterThree;i++){
					sem_post(&unitCapacity); //resize the room with 3 capacity again
					patientCounterThree--; 
				}
				unitID = *(int *)unitIdNumber; //prevent the starvation so we change the room id
				randwait();    
			}
			sem_post(&criticalZoneUnit); //end of critical zone for unit
		}
		else 
        { 
			if(flag==0)  //check the global patient num is completed
            {
				flag++;
				randwait();
				printf("All patient's tests done. Day is over. Staffs' job is done today!!!.\n");
			}
		}
	}
}
void randwait() {
	int len = 1+rand() % 2; //random number assign between 1-3 for waiting
     sleep(len);
}
