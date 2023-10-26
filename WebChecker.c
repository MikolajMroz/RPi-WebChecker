
/*
Mikolaj Mroz / Web Status Checker Mini-project
2003114

README: 
Program must be run as admin/sudo in order for the GPIO pins to be inistialised.
Otherwise, just use the following in CLI:

sudo gpio -g mode 17 out
sudo gpio -g mode 18 out

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <mariadb/mysql.h>		//Use MariaDB

#define MAX_STRING 128
#define YELLOW 1 				//Set Yellow WPiPin (GPIO 18, Pin 12) 
#define RED  0					//Set Red WPiPin (GPIO 17, Pin 11)
#define PINGTIMING 100 			//Set PING interval (milliseconds)
#define WEBSITE1 "test.d1b6gqf9yd0abh.amplifyapp.com"	//Define websites to check
#define WEBSITE2 "gjdfgj.comds"							//*Placeholder unreachable website to demonstrate flash*

int checkStatus(char pingCommand[]){ 	//Check ping response, return down status
int isDown = 0;
if (system(pingCommand) == 0) { 	//If ping returns an error code (code>0)
isDown = 0;							//Then return that the website is unreachable.
} else {
isDown = 1;
}
return isDown;
}

void blink(int led){ 		//Blinks at a defined const interval
for (int i=0;i<3;i++){ 		//Change '3' to increase or decrease # of flashes
	digitalWrite(led, 1); 
	delay(PINGTIMING); 
	digitalWrite(led, 0);
	delay(PINGTIMING);
	}
}

void initialise(){
printf("Initialising Website Status Checker...\n");
wiringPiSetup();						//Initiates wiringPi
system("sudo gpio -g mode 17 out"); 	//Set GPIO pins to OUTPUT mode
system("sudo gpio -g mode 18 out"); 	//...Required for LEDs.
}										//Change to correct pin numbers if necessary

void uploadToDB(MYSQL *con, char affectedWebsite[])
{
time_t t;						//Defines time variable
time(&t);
char query[MAX_STRING] = {0}; 	//Required to combine variables and strings in snprintf
int errorCode = 1;				//returns 1 (isDown value)

snprintf(query, MAX_STRING, "INSERT INTO pinglogs (affectedWebsite, errorCode, timeCaptured) VALUES ('%s', %d, '%s')", affectedWebsite, errorCode, ctime(&t)); //Craft Query
if (mysql_query(con, query)) 	//Submit Query
	{	
	printf("Error submitting Query\n");
	mysql_close(con);
	exit(1);	
	}
}
int main(){

initialise();

//Standard MySQL/MariaDB connection API
MYSQL *con = mysql_init(NULL); //MySQL Connector
if (con == NULL){
printf("An error has occured when connecting to the database.\n");
exit(1);
}

printf("Please enter the database password:\n");
char password[32]; 			//Maximum password is 32 chars long. Change if necessary.
scanf("%s", password);		//Passwords should never be hardcoded in
							//User must input password themselves

//Define database variables (IP/URL, username, password, database, port)
if (mysql_real_connect(con, "pingerror-db.cyqhd8liffpt.us-east-1.rds.amazonaws.com", "admin", password, "errorpingdb", 3306, NULL, 0) == NULL)
{
	printf("Incorrect Credentials\n");
	mysql_close(con);
	exit(1);
}

//Infinite Loop to repeatedly send out pings
for (;;){

	delay(500);
	switch (checkStatus("ping -c1 " WEBSITE1 " -w 2 -qn")){
	case 1: 					
	uploadToDB(con, WEBSITE1); 	
	blink(YELLOW); 				
	
	case 0: 					
	digitalWrite(YELLOW, 1); 	
	}

	delay(500);
	switch (checkStatus("ping -c1 " WEBSITE2 " -w 2 -qn")){
	case 1:
	blink(RED);
	uploadToDB(con, WEBSITE2);
	case 0:
	digitalWrite(RED, 1);
	}
	}

mysql_close(con); 			//When broken out of the loop (CTRL + C)...
exit(0);	  				//Close MySQL connection and exit
}
