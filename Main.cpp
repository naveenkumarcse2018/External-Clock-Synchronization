/*
	K. Naveen Kumar
	CS19MTECH11009
	DC Programming Assignment-I
	IIT Hyderabad
*/

/*
	This file contains the code for main() to start our program execution and create_process_and_socket() function to
	create process and socket for that process which will be used to communicate with other processes through the sockets.
*/

/* Including the headers files that are necessary for our program*/
using namespace std;
#define MAX_BUFFER 512
#define DIFF_PORTS 35000
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <string>


/*
	The below variables are declared as global so that they can be accessible everywhere in the program
*/
int *socket_file_discripter; /* As we are using dynamic no.of processes, we use pointer variable to stor socket discriptor for processes*/

int n; /* Total Number of process, input will come from the input file*/

int k; /*Total number of iterations/rounds , this will take value from input file*/

int lamda_p ,lamda_q ,lambda_send ,lamda_drift; /*The are the parameters mentioned in the assignment discription paper.*/

pthread_cond_t conditioned_thread = PTHREAD_COND_INITIALIZER; /**/

pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER; /**/

int process_done = 0; /*This variable is used to maintain count for the process at the creation of sockets to them.*/

time_t **tableValues; /*As asked in the assignment, this table is used to store the time values of each rounds of processes. We can dynamically construct 2-D array using this variable*/




#include "ClockClass.cpp" /*This includes the ClockClass.cpp file,which will have code related to local clock*/
#include "ProcessClass.cpp" /*This includes the ProcessClass.cpp file, which has the code for communication*/


/*	The create_process_and_socket() function is responsible for creating the process and socket for that process.  This
	function takes one argument as pid (which represents process id).

*/

void create_process_and_socket(int pid)
{
	 
	/*Creating Constructor for the class ProcessClass. The Constructor will take one argument as process id.
	  As we are using threads, n processes will be created using different constructors in different threads.
	*/ 
	ProcessClass process(pid); 
	
	int status = process.socket_create(); /* For each process we are creating socket here.*/

	/*
		The below snippet of line shows the barrier synchronization of the process. We use this synchronization because
		process after creating the socket, it has to wait untill all other processes to create their sockets. So that they
		can communicate with each other. If we don't use this type synchronization, then process will send/receive messages to
		the processes which haven't created their sockets(No means of communication then). So, we use this barrier synchronizations. 
	*/
	process.process_Synchronization_Barrier(); /*Process calling barrier synchronization function*/

	if(status < 0) /*If socket creating fails*/
	{
		printf("Socket is not created for process %d\n",pid);
		return;
	}  
	

	/*Below code will call synchronize_request for other processes from the current process.In this code pid is our current
	  process. So we are not making synchronize_Request to pid itself. We are sending two arguments for synchronize_Request
	  function namely process id and round number.
	*/
	for(int i=0;i<k;i++) /*Looping upto k rounds*/
	{ 
		for(int j=0;j<n;j++) /* Looping for each process*/
			{
				if( j != pid) /*Checks the condition that j is current process id or not. If not it sends request to process j*/
					process.synchronize_Request(j,i+1);/* Calling Synchronize_request function*/
			
			}
		/*After each round of first for loop, we store the clock value of the process in the table*/
		tableValues[pid][i] = process.local_clock.Timeread();

		/*Below snippet of code takes the time from the process local time which is stored in the above table. Then it converts
		  that values into appropriate format. When localtime function below we will get entire time format of our local system
		  as month ,date, year and time. So we just need to extract the time from this. I just did the same thing there.

		 */
		struct tm * time_information; /* Structure containing a calendar date and time broken down into its components*/
	  	time_t system_time=tableValues[pid][i];/* time type variable, which stores the local_clock time from the tableValues*/
	  	time ( &system_time ); /*This function is defined in time.h*/
	  	time_information = localtime ( &system_time ); /*Converts to local time*/
	  	char *stringTime= asctime(time_information); /*Convert tm structure to string*/
	  	
	  	/*Below two lines extract hh::mm::ss format from the stringTime*/
	  	stringTime[19]='\0'; 
	  	stringTime=&stringTime[11];


		printf("\n::::Process %d clock after round %d is %s :::: \n",pid+1,i+1,stringTime);
		printf("Round %d completed in process %d \n",i+1,pid+1);
		
	}
	process.process_Synchronization_Barrier();  
}



int main(){
	int i,j;  
	/*Below few lines reading the input from the input file and stored in global variables declared above in the program*/
	 
	 cin >> n; /*no. of processes or time servers*/
	 cin>>k; /*no of iterations/rounds of modification of error factor between any 2 clocks*/
	 cin>> lamda_p; /*parameter for exponential wait between 2 successive synchronization request.*/
	 cin>> lamda_q;/*parameter for exponential wait between 2 successive synchronization reply*/
	 cin >> lambda_send;/*parameter used to simulate exponential delay in message send.*/
	 cin >> lamda_drift ; /*parameter for exponential drift of local clock time*/	


	 /*If n value is not atleast 2, then we print below message and terminate the program*/
	 if(n<2){
	 	printf("Atleast two processes are needed to make communication \n");
	 	return 0;
	 }
	
	/*As mentioned above socket_file_discripter is used to store socket discriptor for the process dynamically.
	  Now we are creating memory of array of size n, which gives array of n units which will be used to store socket
	  discriptor for each process. Here n is no.of proceses
	*/
	socket_file_discripter = new int[n]; /*Allocating memory of size n*/


	/*As we are given above, local clock of each process in eacah round will be stored in the table format, so now we are 
	 creating a 2-D dynamic array to store these values. Below code will create dynamic 2-D array of tableValues
	*/ 
	tableValues = new time_t*[n];for(int i=0;i<n;i++)tableValues[i] = new time_t[k];


	/*Creating threads for each process*/
	thread threads[n]; /*Array of threads of size n*/
	for(i=0;i<n;i++) /*Creating thread of each process. 'n' threads for 'n' processes*/
			threads[i] =  thread(create_process_and_socket,i);  /*thread takes two arguments, 1. routine/function address 2. process id. Each thread will execute create_process_and_socket function. So, that means each process is mainted by the individual threads */ 
	for(i=0;i<n;i++) 
			threads[i].join();/* Joining the threads so that main will wait until all processes complete their execution*/

	
	/*
		Below snippet of code will be used to print the required table as mentioned in the assignment.
		Below code also used to calculate the mean and variance of time of each round.
		Mean= simpley Averaging the times of each round
		To calculate the variance I used formula that looks like  1/n[sum(X-mean)**2] for each processes time in each round
	*/
	cout<<endl;
	cout<<endl;
	double sum[k] = {0,0}; /*Sum variable is used in calculting the mean.*/
	printf(":::\t");for(j=0;j<k;j++)printf("Round%d\t",j+1); /*Just printing the rounds numbers */
	printf("\n");	
	
	/*
		The following code is used to sum the each process's clock in each round. The sum value of each round is stored in sum array
	*/
	for(int i=0;i<k;i++) /*For each round*/
	{
		for(j=0;j<n;j++) /*Each process in round i*/
		{
			 sum[i] += tableValues[j][i]; /*Summing the each process time in round i and stored in array sum[i]*/
		}
	}

	
	/*Printing the table. For each process loopig over all rounds and displaying the time*/

	for (int i = 0; i < n; i++)
	{
		printf("P%d: \t",i+1); /* To display process number*/
		for (int j = 0; j < k; j++)
		{
			cout<<tableValues[i][j]<<"\t"; /*Displaying the local clock value of process*/
		}
		cout<<endl;
	}



	double mean[k]={0}; /*To store mean value of each round. So we declared array to mean of rounds*/
	double variance[k] = {0}; /*To store variance of each round*/


	/*Here , Calculating mean of each round. We already calculated sum over each round above. So we just divide it with
	  no.of processes to get mean.
	*/
	cout<<endl;
	printf("Mean\t");
	for(j=0;j<k;j++) /* Loop over each round*/
	{
		mean[j]=sum[j]/n;	 /* Getting the mean */
		printf("%0.2f\t",mean[j]); /* Displaying the mean of each round*/
	}


	/*Below code calculates the variance of the local clocks over each round.
	  I just used variance calculation formula from statistics. 
	*/

	for(j=0;j<k;j++) /* Looping over each round*/
	{
		for(i=0;i<n;i++) /*For each process in round j*/
		{
			double Var_Mean=tableValues[i][j]-mean[j]; /* Using formula , subtracting the mean from the clock*/
			variance[j] += Var_Mean*Var_Mean; /* So squaring the value here*/
		}
		variance[j] /= n; /*Now dividing with n to get variance value of round j*/
	}
	/*Below snippet of code will display the variance of each round*/
	printf("\nVar\t");
	for(j=0;j<k;j++)
		printf("%f\t",variance[j]);
	printf("\n");

	return 0;
}