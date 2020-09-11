/*
	K. Naveen Kumar
	CS19MTECH11009
	DC Programming Assignment-I
	IIT Hyderabad
*/

/*
	This file called ClockClass.cpp implements the local clock for process. This class is having three functions namely 
	Timeread(), update() and IncrementDriftFactor(). The objective of these functions already given in the assignment.

*/


/*********** Class Starts here ***************/
class localclock /* We are creating class for localclock here.*/
{
	/* We want make variables and methods of the class accessible from any other snippet of the program.
	   So , to give accesse through entire program we are making them available by declaring as public. The public
	   members are visible to all other classes. Now each process is able to access these members/variables in the program.
	*/

 	public: /* public key word used to declare "public". This is called public access modifier*/

 	/* Below two variables are declared to maintain driftFactor and error factor in the time calculation.*/
	int error_factor=0;
	int driftFactor=0;
	

	/* The Timeread() method is used to read the system time and getting the required format for our task. Then this function
		will return the value at the end, which is calculated using the formula given in the Assignment discription.
		This fucntion is invoked by processes (and their threads to read the current time)
	*/
	time_t Timeread() /*This function is not taking any arguments. This function just reads the time*/
	{
		struct timespec start; /* Variable to store the time value*/
		clock_gettime(CLOCK_MONOTONIC, &start); /* For obtaining time with much higher precision(nanoseconds) */
		return ((start.tv_sec*1e9) + start.tv_nsec)*1e-9 + error_factor*driftFactor; /* Time is read in the form of seconds and nanoseconds. So we are converting nano seconds into seconds and adding error_factor*driftFactor and we are returning this value*/
	}

	


	/*
		The update() function is used by the process to correct their clock. This functions takes one argument as optimal_delta
		and add this optimal_delta to error_factor variable of the process which calls this function.
	*/

	void update(int optimal_delta) /* Taking one argument as int value */
	{
		error_factor=error_factor+optimal_delta; /*Updating the error_factor variable using the argument that is passed to this function.*/
	}
	


	/*
		This IncrementDriftFactor function runs in a separate thread. The clock drift is exponentially distributed w.r.t Lambda_drift
		and can be positive as well as negative. This function takes no arguments.
	*/
	int IncrementDriftFactor()   /*As we are using thread, for each process clock will drift in background */
	{
		while(true){

			/*Below snippet of code will calculate the exponential distributions*/
			srand((unsigned)std :: time(0));
			double clock_drift = -1.0 + (rand() / ( RAND_MAX / 2.0 ));

			driftFactor = driftFactor + clock_drift; /*Updating the driftFactor bye the clock_drift that we got here.*/
			sleep(1);

			
					}
	}

};
/*********** Class Ends here ***************/
