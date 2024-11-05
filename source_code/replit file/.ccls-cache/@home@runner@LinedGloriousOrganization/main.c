#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_LOCATIONS 25
#define MAX_DRIVERS 80
#define MAX_DRIVERS_TYPE 16
#define MAX_VEHICLE_TYPE_LENGTH 50
#define MAX_CAB_TYPE_LEN 20
#define MAX_DRIVER_NAME_LEN 50
#define MAX_PHONE_NUMBER_LEN 15
#define MAX_CAR_PLATE_NUMBER_LEN 15
#define MAX_FILENAME_LEN 100

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD          "\x1b[1m"
#define ANSI_UNDERLINE     "\x1b[4m"
#define ANSI_CLEAR_SCREEN  "\033[2J"
#define ANSI_MOVE_CURSOR   "\033[%d;%dH"

// global variables
int n_mini, n_sedan,n_suv,n_auto,n_bike;
char username_check[20], password_check[20];
//-----------------------------------------------------------//
//-----------------------------------------------------------//

// structure definitions 
typedef struct {
  char username[20];
  char password[20];
  char first_name[50];
  char last_name[50];
  char phoneNumber[20];
  char email[20];
} User; 

User user;

typedef struct {
    int id;
    char name[50];
    float latitude;
    float longitude;
} Location;

typedef struct {
    int id;
    char name[50];
    float latitude;
    float longitude;
    char phoneNumber[15];
    char carNumber[15];
    char carType[15];
    int dropLocation;
    double etaToPickup;
	float rating;
} Driver;


typedef struct {
    int numLocations; int numDrivers;
    int numMini; int numSedan;
    int numSuv; int numBike;
    int numAuto;
    Location locations[MAX_LOCATIONS];
    Driver drivers[MAX_DRIVERS];
    Driver Mini[MAX_DRIVERS_TYPE];
    Driver Sedan[MAX_DRIVERS_TYPE];
    Driver SUV[MAX_DRIVERS_TYPE];
    Driver Bike[MAX_DRIVERS_TYPE];
    Driver Auto[MAX_DRIVERS_TYPE];
    double distances[MAX_LOCATIONS][MAX_LOCATIONS];
} Graph;


typedef struct {
    char name[MAX_VEHICLE_TYPE_LENGTH];
    float fareRate;
    int availability;
    float fareperkm;
} CabType;

typedef struct {
    char name[MAX_DRIVER_NAME_LEN];
    char phoneNumber[MAX_PHONE_NUMBER_LEN];
    char carPlateNumber[MAX_CAR_PLATE_NUMBER_LEN];
    int cabCapacity;
    char cabType[10];
    double rating;
} driver;

typedef struct {
    char cabType[MAX_CAB_TYPE_LEN];
    char requestedTime[10];
} BookingRequest;
//---------------------------------------------------------------------------------//
void displayAvailableCabTypes(CabType cabTypes[],float distance) {
    printf("\n\nAvailable Cab Types:\n");
    for (int i = 0; i < 5; i++) {
        if (cabTypes[i].availability == 1) {
            printf("%d. %s - Fare Rate: %.2f\n", i , cabTypes[i].name, cabTypes[i].fareRate + ((distance-3)*cabTypes[i].fareperkm));
        }
      
    }
 
}

float displayest(CabType cabTypes[],float distance,int i) {
    int k;
       k =  cabTypes[i].fareRate + ((distance-3)*cabTypes[i].fareperkm);
        
  return k;
}

void assignCabTypes(Driver drivers[], Graph* graph, int numDrivers, int numMini, int numSedan, int numSuv, int numBike, int numAuto) {
    int i, miniIndex = 0, sedanIndex = 0, suvIndex = 0, bikeIndex = 0, autoIndex = 0;
    for (i = 0; i < numDrivers; i++) {
        if (i < numMini) {
            graph->Mini[miniIndex++] = drivers[i];
            strcpy(drivers[i].carType, "mini");
        } else if (i < numMini + numSedan) {
            graph->Sedan[sedanIndex++] = drivers[i];
            strcpy(drivers[i].carType, "sedan");
        } else if (i < numMini + numSedan + numSuv) {
            graph->SUV[suvIndex++] = drivers[i];
            strcpy(drivers[i].carType, "suv");
        } else if (i < numMini + numSedan + numSuv + numBike) {
            graph->Bike[bikeIndex++] = drivers[i];
            strcpy(drivers[i].carType, "bike");
        } else if (i < numMini + numSedan + numSuv + numBike + numAuto) {
            graph->Auto[autoIndex++] = drivers[i];
            strcpy(drivers[i].carType, "auto");
        } else {
            break;
        }
    }

    graph->numMini = miniIndex;
    graph->numSedan = sedanIndex;
    graph->numSuv = suvIndex;
    graph->numBike = bikeIndex;
    graph->numAuto = autoIndex;
}

//check haversine 
double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0; // distance between lats & convert to radians
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = 6371 * c; // Earth's radius in kilometers

    return distance;
}

void readData(Driver drivers[], int numDrivers) {//In summary, the readData function opens a file, reads driver information from it, and stores the data in an array of Driver structures. It assumes that the file follows a specific format where each line contains the driver's ID, name, latitude, longitude, phone number, and car number, separated by spaces.
    FILE* infile;
    infile = fopen("drivers.txt", "r");
    if (infile == NULL) {
        fprintf(stderr, "\nError opening file\n");
        exit(1);//check
    }

    int i = 0;
    while (i < numDrivers && fscanf(infile, "%d %s %f %f %s %s %f", &drivers[i].id, drivers[i].name, &drivers[i].latitude, &drivers[i].longitude, drivers[i].phoneNumber, drivers[i].carNumber, &drivers[i].rating) == 7) {
        i++;
    }

    fclose(infile);
}

void readLocations(Location locations[], int numLocations) {
    FILE* infile;
    infile = fopen("locations.txt", "r");
    if (infile == NULL) {
        fprintf(stderr, "\nError opening file\n");
        exit(1);
    }

    int i = 0;
    while (i < numLocations && fscanf(infile, "%d %s %f %f", &locations[i].id, locations[i].name, &locations[i].latitude, &locations[i].longitude) == 4) {
        i++;
    }

    fclose(infile);
}
//check
void addEdge(double Distances[MAX_LOCATIONS][MAX_LOCATIONS], int node1, int node2, Location locations[MAX_LOCATIONS]) {
    Distances[node1 - 1][node2 - 1] = haversine(locations[node1 - 1].latitude, locations[node1 - 1].longitude, locations[node2 - 1].latitude, locations[node2 - 1].longitude);
    Distances[node2 - 1][node1 - 1] = Distances[node1 - 1][node2 - 1]; // For undirected graph
}

    void initialiseEdges(double matrix[MAX_LOCATIONS][MAX_LOCATIONS], Location locations[MAX_LOCATIONS]) {
    addEdge(matrix, 0, 1, locations);     // Velachery - Thoraipakkam
    addEdge(matrix, 0, 4, locations);     // Velachery - Madipakkam
    addEdge(matrix, 1, 2, locations);     // Thoraipakkam - Sholinganallur
    addEdge(matrix, 1, 10, locations);    // Thoraipakkam - Medavakkam
    addEdge(matrix, 2, 10, locations);    // Sholinganallur - Medavakkam
    addEdge(matrix, 3, 8, locations);     // Guindy - Saidapet
    addEdge(matrix, 3, 6, locations);     // Guindy - T Nagar
    addEdge(matrix, 8, 6, locations);     // Saidapet - T Nagar
    addEdge(matrix, 6, 12, locations);    // T Nagar - Nungambakkam
    addEdge(matrix, 6, 5, locations);     // T Nagar - Ashok Nagar
    addEdge(matrix, 12, 11, locations);   // Nungambakkam - Egmore
    addEdge(matrix, 12, 13, locations);   // Nungambakkam - Vadapalani
    addEdge(matrix, 13, 5, locations);    // Vadapalani - Ashok Nagar
    addEdge(matrix, 13, 9, locations);    // Vadapalani - Porur
    addEdge(matrix, 10, 14, locations);   // Medavakkam - Tambaram
    addEdge(matrix, 4, 14, locations);    // Madipakkam - Tambaram
    addEdge(matrix, 15, 16, locations);   // Chromepet - Pallavaram
    addEdge(matrix, 15, 14, locations);   // Chromepet - Tambaram
    addEdge(matrix, 16, 14, locations);   // Pallavaram - Tambaram
    addEdge(matrix, 17, 11, locations);   // Egmore - Kilpauk
    addEdge(matrix, 17, 18, locations);   // Egmore - Royapettah
    addEdge(matrix, 11, 7, locations);    // Kilpauk - Anna Nagar
    addEdge(matrix, 18, 21, locations);   // Royapettah - Triplicane
    addEdge(matrix, 18, 22, locations);   // Royapettah - Marina Beach
    addEdge(matrix, 21, 22, locations);   // Triplicane - Marina Beach
    addEdge(matrix, 19, 20, locations);   // Besant Nagar - Adyar
    addEdge(matrix, 19, 22, locations);   // Besant Nagar - Marina Beach
    addEdge(matrix, 20, 24, locations);   // Mylapore - Adyar
    addEdge(matrix, 20, 21, locations);   // Mylapore - Triplicane
    addEdge(matrix, 23, 14, locations);   // Kelambakkam - Tambaram
    addEdge(matrix, 23, 2, locations);    // Kelambakkam - Sholinganallur
    addEdge(matrix, 23, 24, locations);   // Kelambakkam - OMR (Old Mahabalipuram Road)
}//

void initGraph(Graph* graph) {
    // Read locations
    graph->numLocations = 25;
    readLocations(graph->locations, graph->numLocations);

    // Read drivers
    graph->numDrivers = 80;
    readData(graph->drivers, graph->numDrivers);

    // Assign cab types to drivers
    int numMini = 20;
    int numSedan = 20;
    int numSuv = 20;
    int numBike = 10;
    int numAuto = 10;
    assignCabTypes(graph->drivers, graph, graph->numDrivers, numMini, numSedan, numSuv, numBike, numAuto);
//check
    // Initialize distance matrix
    for (int i = 0; i < MAX_LOCATIONS; i++) {
        for (int j = 0; j < MAX_LOCATIONS; j++) {
            graph->distances[i][j] = INFINITY;
        }
    }//It initializes the distance matrix in the Graph structure. The distance matrix represents the distances between locations in the graph. Initially, all distances are set to INFINITY to indicate that there is no direct connection between locations.

    initialiseEdges(graph->distances, graph->locations);
}//


void printLocations(Graph* graph) {
    printf("\nLocations:\n");
    for (int i = 0; i < graph->numLocations; i++) {
        printf("%d. %s\n", graph->locations[i].id, graph->locations[i].name);
    }
}

void printDrivers(Graph* graph) {
    printf("\nDrivers:\n");
    for (int i = 0; i < graph->numDrivers; i++) {
        printf("%d. %s (%s)\n", graph->drivers[i].id, graph->drivers[i].name, graph->drivers[i].carType);
    }
}


int minDistance(int dist[], int sptSet[], int V) {
    int min = INT_MAX, min_index;//In graph theory, a graph consists of a set of vertices and a set of edges that connect pairs of vertices. 

    for (int v = 0; v < V; v++)
        if (sptSet[v] == 0 && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}

void dijkstra(Graph* graph, int src, int dest) {
    int V = graph->numLocations;
    int dist[V];
    int sptSet[V];
    int prev[V];

    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = 0;
        prev[i] = -1;
    }

    dist[src] = 0;
//Dijkstra's algorithm is being executed to find the shortest path from a source vertex to all other vertices in the graph.
    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet, V);//Select the vertex (u) with the minimum distance from the set of vertices not yet included in the shortest path 
        sptSet[u] = 1;
        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && graph->distances[u][v] && dist[u] != INT_MAX && dist[u] + graph->distances[u][v] < dist[v]) {
                dist[v] = dist[u] + graph->distances[u][v];
                prev[v] = u;
            }
        }
    }

    if (dist[dest] == INT_MAX) {
        printf("No path found from %s to %s\n", graph->locations[src].name, graph->locations[dest].name);
        return;
    }

    // Construct the path
    int path[MAX_LOCATIONS];
    int pathLength = 0;
    int current = dest;
    while (current != -1) {
        path[pathLength++] = current;
        current = prev[current];
    }
    
    // Print the path
    printf("\nShortest path details:\n");
    printf("Pickup Location: %s\n", graph->locations[src].name);
    printf("Drop Location: %s\n", graph->locations[dest].name);
    printf("Distance: %d km\n", dist[dest]);
    printf("Path: ");
    for (int i = pathLength - 1; i >= 0; i--) {
        printf("%s", graph->locations[path[i]].name);
        if (i != 0) {
            printf(" -> ");
        }
    }
    printf("\n");
  
}

float dijkstracp(Graph* graph, int src, int dest) {
    int V = graph->numLocations;
    int dist[V];
    int sptSet[V];
    int prev[V];

    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = 0;
        prev[i] = -1;
    }

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet, V);
        sptSet[u] = 1;
        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && graph->distances[u][v] && dist[u] != INT_MAX && dist[u] + graph->distances[u][v] < dist[v]) {
                dist[v] = dist[u] + graph->distances[u][v];
                prev[v] = u;
            }
        }
    }

    if (dist[dest] == INT_MAX) {
        
        return;
    }

    // Construct the path
    int path[MAX_LOCATIONS];
    int pathLength = 0;
    int current = dest;
    while (current != -1) {
        path[pathLength++] = current;
        current = prev[current];
    }
return dist[dest];
  
}
//***************************************************
int findClosestDriver(Graph* graph, int locationIndex, char* cabType) {
    Driver* drivers = NULL;
    int numDrivers = 0;

    if (strcmp(cabType, "mini") == 0) {
        drivers = graph->Mini;
        numDrivers = graph->numMini;
    } else if (strcmp(cabType, "sedan") == 0) {
        drivers = graph->Sedan;
        numDrivers = graph->numSedan;
    } else if (strcmp(cabType, "suv") == 0) {
        drivers = graph->SUV;
        numDrivers = graph->numSuv;
    } else if (strcmp(cabType, "bike") == 0) {
        drivers = graph->Bike;
        numDrivers = graph->numBike;
    } else if (strcmp(cabType, "auto") == 0) {
        drivers = graph->Auto;
        numDrivers = graph->numAuto;
    } else {
        printf("Invalid cab type.\n");
        return -1;
    }

    if (numDrivers == 0) {
        printf("No available drivers for the specified cab type.\n");
        return -1;
    }

    double minDistance = INFINITY;
    int closestDriverIndex = -1;

    for (int i = 0; i < numDrivers; i++) {
        double distance = haversine(graph->locations[locationIndex].latitude, graph->locations[locationIndex].longitude, drivers[i].latitude, drivers[i].longitude);

        if (distance < minDistance) {
            minDistance = distance;
            closestDriverIndex = i;
        }
    }

    return closestDriverIndex;
}
int isValidCabType(const char* cabType) {
    if (strcmp(cabType, "mini") == 0 ||
        strcmp(cabType, "sedan") == 0 ||
        strcmp(cabType, "suv") == 0 ||
        strcmp(cabType, "bike") == 0 ||
        strcmp(cabType, "auto") == 0) {
        return 1;  // Valid cab type
    } else {
      
        return 0;  // Invalid cab type
    }
}


void allotCab(Graph* graph, int pickupLocation, int dropLocation, char* cabType) {
    int pickupIndex = pickupLocation - 1;
    int dropIndex = dropLocation - 1;

    if (!isValidCabType(cabType)) {
        printf("Invalid cab type.\n");
        return;
    }

    int closestDriver = findClosestDriver(graph, pickupIndex, cabType);

    if (closestDriver == -1) {
        printf("No available drivers for the specified cab type.\n");
        return;
    }

    printf("Cab of type '%s' allotted successfully.\n", cabType);
    printf("Driver Details:\n");

    Driver* driver = NULL;

    // Determine the driver array based on the cab type
    if (strcmp(cabType, "mini") == 0) {
        driver = &(graph->Mini[closestDriver]);
    } else if (strcmp(cabType, "sedan") == 0) {
        driver = &(graph->Sedan[closestDriver]);
    } else if (strcmp(cabType, "suv") == 0) {
        driver = &(graph->SUV[closestDriver]);
    } else if (strcmp(cabType, "bike") == 0) {
        driver = &(graph->Bike[closestDriver]);
    } else if (strcmp(cabType, "auto") == 0) {
        driver = &(graph->Auto[closestDriver]);
    }

    // Calculate distance and ETA to pickup location
    double distanceToPickup = haversine(driver[closestDriver].latitude, driver[closestDriver].longitude, graph->locations[pickupIndex].latitude, graph->locations[pickupIndex].longitude);

    double averageSpeed = 60.0; // Average speed in km/h
    double etaToPickup = distanceToPickup / averageSpeed * 60.0; // ETA in minutes

    // Assign ETA to driver
    driver->etaToPickup = etaToPickup;

    // Print driver details
    printf("Driver ID: %d\n", driver->id);
    printf("Driver Name: %s\n", driver->name);
    printf("Phone Number: %s\n", driver->phoneNumber);
    printf("Car Number: %s\n", driver->carNumber);
    printf("Car Type: %s\n", cabType);
    printf("Estimated Time of Cab Arrival: %.2f minutes\n", driver->etaToPickup);
	printf("Rating: %.1f\n", driver->rating);
    
}

void checkCabInput(char* cabType, int cabChoice) {
    while (1) {
        printf("\nAvailable cab choices: 0 for bike, 1 for auto, 2 for suv, 3 for mini, 4 for sedan");
        

        printf("\nEnter the cab type: ");
        scanf("%s", cabType);

        if (((cabChoice == 0) && strcmp(cabType, "bike") == 0) ||((cabChoice == 1) && strcmp(cabType, "auto") == 0) ||
            ((cabChoice == 2) && strcmp(cabType, "suv") == 0) ||
            ((cabChoice == 3) && strcmp(cabType, "mini") == 0) ||
            ((cabChoice == 4) && strcmp(cabType, "sedan") == 0)) {
            // The entered cab type matches the cab choice
            
            break;
        }

        printf("\nNote: The cab choice and type should match. Please try again.\n");
    }
}




double getSurgeFee() {
    time_t now;
    struct tm* timeinfo;

    time(&now);
    timeinfo = localtime(&now);

    int hour = timeinfo->tm_hour;
    int min = timeinfo->tm_min;

    if ((hour >= 10 && hour < 11) || (hour >= 19 && hour < 21)) {
        return 1.2; // Surge fee of 20%
    } else {
        return 1.0; // No surge fee
    }
}



void welcomeMessage()
{
    printf("\n\n\n\n\n");
    printf("\n\t\t\t  **-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**\n");
    printf("\n\t\t\t        =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=");
    printf("\n\t\t\t        =                   WELCOME                  =");
    printf("\n\t\t\t        =                     TO                     =");
    printf("\n\t\t\t        =                    VIVAR                   =");
    printf("\n\t\t\t        =              CAB BOOKING SYSTEM            =");
    printf("\n\t\t\t        =                                            =");
    printf("\n\t\t\t        =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
    printf("\n\t\t\t  **-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**\n");
    printf("\n\n\n\t\t\t Enter any key to continue.....");
    getchar();
}
void displayTitle() {
    printf("\n=====================================\n");
    printf("      CAB BOOKING FEEDBACK SYSTEM\n");
    printf("=====================================\n\n");
}

void displayThankYouMessage() {
    printf("\n\nThank you for your valuable feedback!\n");
    printf("We appreciate your time and input.\n\n");
}

void displayRatingsPrompt() {
    printf("How would you rate your User experience (1-5): ");
}

int getRatingInput() {
    int rating;
    scanf("%d", &rating);
    return rating;
}

void displayCommentsPrompt() {
    printf("\nPlease leave your comments (max 100 characters):\n");
}

void getCommentsInput(char* comments) {
    fgets(comments, 100, stdin);
    // Remove the trailing newline character from fgets
    comments[strcspn(comments, "\n")] = '\0';
}

void displayFeedbackSummary(int rating, const char* comments) {
    printf("\n------------------- FEEDBACK SUMMARY -------------------\n");
    printf("User-interface Rating: ");
    for (int i = 0; i < rating; i++) {
        printf("⭐");
    }
    printf("\n\n");
    printf("Comments:\n%s\n", comments);
    printf("-------------------------------------------------------\n");
}

void viewFeedback() {
    FILE *feed = fopen("feedback.txt", "r");
    if (feed == NULL) {
        printf("Error opening file: feedback.txt\n");
        return;
    }

    printf("Feedback from users:\n");

    char line[256]; // Adjust the buffer size according to your needs
    while (fgets(line, sizeof(line), feed) != NULL) {
        printf("%s", line);
    }

    fclose(feed);
}

void flushInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void collectUserFeedback() {
    displayTitle();

    displayRatingsPrompt();
    int rating = getRatingInput();
    flushInputBuffer();

    if (rating < 1 || rating > 5) {
        printf("\nInvalid rating. Please enter a rating between 1 and 5.\n");
        return;
    }

    displayCommentsPrompt();
    char comments[100];
    getCommentsInput(comments);

    displayThankYouMessage();
    displayFeedbackSummary(rating, comments);

    FILE* feed = fopen("feedback.txt", "a");
    if (feed == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(feed, "User-interface Rating: %d\n", rating);
    comments[strcspn(comments, "\n")] = '\0';
    fprintf(feed, "Comments: %s\n", comments);

    fclose(feed);
}

int isBookingTimeValid(char* requestedTime) {
    // Get the current time in IST
    time_t rawTime = time(NULL);
    struct tm* currentTime = localtime(&rawTime);

    // Adjust the current time for IST (UTC +5:30)
    currentTime->tm_hour += 5;
    currentTime->tm_min += 30;

    // Handle overflow of minutes
    if (currentTime->tm_min >= 60) {
        currentTime->tm_hour += 1;
        currentTime->tm_min -= 60;
    }

    // Handle overflow of hours
    if (currentTime->tm_hour >= 24) {
        currentTime->tm_hour -= 24;
    }

    // Parse the requested time
    int requestedHour, requestedMinute;
    sscanf(requestedTime, "%d:%d", &requestedHour, &requestedMinute);

    // Calculate the time difference in hours
    int currentHour = currentTime->tm_hour;
    int timeDifference = requestedHour - currentHour;

    // Handle the case when the requested time is on the next day
    if (timeDifference < 0) {
        timeDifference += 24;
    }

    // Check if the time difference is within the valid range (2 to 20 hours)
    if (timeDifference >= 2 && timeDifference <= 20) {
        return 1; // Valid booking time
    } else {
        return 0; // Invalid booking time
    }
}


void assignDriver(char* cabType, driver* drivers, int numDrivers) {
    int i;
    int* availableDriverIndices = NULL;
    int numAvailableDrivers = 0;

    // Get the current time.
    time_t currentTime = time(NULL);

    // Determine the filename based on the cabType
    char filename[100];
    if (strcmp(cabType, "Mini") == 0) {
        strcpy(filename, "Minidrivers.txt");
    } else if (strcmp(cabType, "Sedan") == 0) {
        strcpy(filename, "Sedandrivers.txt");
    } else if (strcmp(cabType, "SUV") == 0) {
        strcpy(filename, "SUVdrivers.txt");
    } else if (strcmp(cabType, "Auto") == 0) {
        strcpy(filename, "Autodrivers.txt");
    } else if (strcmp(cabType, "Bike") == 0) {
        strcpy(filename, "Bikedrivers.txt");
    } else {
        printf("Invalid cab type.\n");
        return;
    }

    // Open the file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    // Read the drivers from the file
    int numFileDrivers;
    fscanf(file, "%d", &numFileDrivers);
    driver* fileDrivers = malloc(numFileDrivers * sizeof(Driver));

    for (i = 0; i < numFileDrivers; i++) {
        fscanf(file, "%s %s %s %s", fileDrivers[i].name, fileDrivers[i].phoneNumber, fileDrivers[i].carPlateNumber, fileDrivers[i].cabType);
    }

    fclose(file);

    // Find the available drivers for the requested cab type
    for (i = 0; i < numFileDrivers; i++) {
        if (strcmp(fileDrivers[i].cabType, cabType) == 0) {
            numAvailableDrivers++;
            availableDriverIndices = realloc(availableDriverIndices, numAvailableDrivers * sizeof(int));
            availableDriverIndices[numAvailableDrivers - 1] = i;
        }
    }

    // Randomly assign a driver
    if (numAvailableDrivers > 0) {
        int randomIndex = rand() % numAvailableDrivers;
        int assignedDriverIndex = availableDriverIndices[randomIndex];
        printf("Driver assigned: %s\n", fileDrivers[assignedDriverIndex].name);
        printf("Phone number: %s\n", fileDrivers[assignedDriverIndex].phoneNumber);
        printf("Car plate number: %s\n", fileDrivers[assignedDriverIndex].carPlateNumber);
    } else {
        // No drivers available for the requested cab type
        printf("No drivers available for the requested cab type.\n");
    }

    free(availableDriverIndices);
    free(fileDrivers);
}
Graph graph;

void Admin() {
  initGraph(&graph);
  int choice;
  char input[100];

  while (true) {
    printf("1. Manage number of cabs\n2. View locations\n3. View drivers\n4. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
      case 1: {
        printf("Available cabs:\n");
        printf("Enter the number of Mini: ");
        scanf("%d", &n_mini);
        printf("Enter the number of Sedan: ");
        scanf("%d", &n_sedan);
        printf("Enter the number of SUV: ");
        scanf("%d", &n_suv);
        printf("Enter the number of Autos: ");
        scanf("%d", &n_auto);
        printf("Enter the number of Bikes: ");
        scanf("%d", &n_bike);
		printf("\n");
		printf("Redirecting...\n");
		sleep(2);
        break;
      }
      case 2: {
        printLocations(&graph);
		printf("\n");
		printf("Redirecting...\n");
		sleep(2); 
        break;
      }
      case 3: {
        printDrivers(&graph);
		printf("\n");
		printf("Redirecting...\n");
		sleep(2);
        break;
      }
      case 4: {
        printf("Exiting...\n");
        return;
      }
      default: {
        printf("Invalid choice. Enter again.\n");
        break;
      }
    }
  }
}
void maskPasswordInput(char password[]) { 
  struct termios term, oldterm;
  tcgetattr(0, &oldterm);
  term = oldterm;
  term.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(0, TCSANOW, &term);
  int i = 0;
  char ch;
  while ((ch = getchar()) != '\n' && ch != EOF) {
    if (ch == '\b' || ch == '\177') { 
      if (i > 0) {
        i--;
        putchar('\b'); 
        putchar(' ');  
        putchar('\b');
        fflush(stdout);
      }
    } else {
      password[i++] = ch;
      putchar('*');
      fflush(stdout);
    }
  }
  password[i] = '\0';
  tcsetattr(0, TCSANOW, &oldterm);
}

bool isValidEmail(char *email) {
  int i, at = -1, dot = -1;
  int len = strlen(email);
  for (i = 0; i < len; i++) {
    if (email[i] == '@')
      at = i;
    else if (email[i] == '.')
      dot = i;
  }
  if (at == -1 || dot == -1)
    return false;
  if (at > dot)
    return false;
  if (at == 0 || dot == 0 || dot == len - 1)
    return false;
  return true;
}

bool isValidPhoneNumber(char *number) {
  int i;
  int len = strlen(number);
  if (len != 10)
    return false;
  for (i = 0; i < len; i++) {
    if (!isdigit(number[i]))
      return false;
  }
  return true;
}

int isStrongPassword(char password[]) {
  int i, hasUpper = 0, hasLower = 0, hasNumber = 0, hasSpecial = 0;
  int len = strlen(password);
  if (len < 8) {
    printf("\nPassword should be at least 8 characters long.\n");
    return 0;
  }
  for (i = 0; i < len; i++) {
    if (isupper(password[i])) {
      hasUpper = 1;
    } else if (islower(password[i])) {
      hasLower = 1;
    } else if (isdigit(password[i])) {
      hasNumber = 1;
    } else if (ispunct(password[i])) {
      hasSpecial = 1;
    }
  }
  if (!hasUpper) {
    printf("\nPassword should contain at least one uppercase letter.\n");
    return 0;
  }
  if (!hasLower) {
    printf("\nPassword should contain at least one lowercase letter.\n");
    return 0;
  }
  if (!hasNumber) {
    printf("\nPassword should contain at least one number.\n");
    return 0;
  }
  if (!hasSpecial) {
    printf("\nPassword should contain at least one special character.\n");
    return 0;
  }
  return 1;
}

void registerUser() {
  User user, existingUser;
  int usernameExists;
  FILE *fptr;
  do {
    usernameExists = 0;
    printf("\nEnter username: ");
    scanf("%s", user.username);
    getchar(); 

    fptr = fopen("users.txt", "r");
    if (fptr != NULL) {
      while (fread(&existingUser, sizeof(existingUser), 1, fptr)) {
        if (strcasecmp(existingUser.username, user.username) == 0) {
          usernameExists = 1;
          printf("Username already exists. Please choose a different one.\n");
          break;
        }
      }
      fclose(fptr);
    }
  } while (usernameExists);

  char confirmedPassword[50];

  while (1) {
    printf("\nEnter password: ");
    maskPasswordInput(user.password);
    if (isStrongPassword(user.password)) {
      while (1) {
        printf("\nConfirm password: ");
        maskPasswordInput(confirmedPassword);
        if (strcmp(user.password, confirmedPassword) == 0) {
          break;
        } else {
          printf("\nPasswords do not match. Please re-enter password.\n");
          printf("\nEnter password: ");
          maskPasswordInput(user.password);
        }
      }
      break;
    }
  }
  sleep(1);
  printf("\n\nRegistration successful!!\n\n");
  sleep(2);
  printf("\n-------------Profile Details-------------\n");
  printf("\nEnter the details for your profile\n");

  printf("\nEnter your first name: ");
  scanf("%s", user.first_name);
  printf("Enter your last name: ");
  scanf(" %[^\n]s", user.last_name);

  do {
    printf("Enter phone number: ");
    scanf("%s", user.phoneNumber);
    if (isValidPhoneNumber(user.phoneNumber)) {
      break;
    } else {
      printf("Invalid phone number. Please enter a valid one.\n");
    }
  } while (1);
  do {
    printf("Enter email: ");
    scanf("%s", user.email);
    if (isValidEmail(user.email)) {
      break;
    } else {
      printf("\nInvalid email. Please enter a valid one.\n");
    }
  } while (1);
  
  fptr = fopen("users.txt", "a");
    if (fptr != NULL) {
        fwrite(&user, sizeof(user), 1, fptr);
        fclose(fptr);
        printf("Details stored successfully.\n");
    } else {
        printf("Error occurred while opening the file.\n");
  }
}

int loginUser() {
  User user;
  char username[20], password[20];
  int choice;
  int failedAttempts = 0;
  do {
    sleep(1);
    printf("\nEnter username: ");
    scanf("%s", username);
    getchar();
    strcpy(username_check, username);
    printf("Enter password: ");
    maskPasswordInput(password);
    strcpy(password_check, password);
    FILE *fptr = fopen("users.txt", "r");
    if (fptr != NULL) {
      int loginSuccessful = 0;

      while (fread(&user, sizeof(user), 1, fptr)) {

        if (strcasecmp(user.username, username) == 0 &&
            strcmp(user.password, password) == 0) {
          sleep(2);
          printf("\nLogin successful.\n");
          sleep(2);
          loginSuccessful = 1;
		  strcpy(username_check,user.username);
          return 1;
        }
      }
      fclose(fptr);
      if (loginSuccessful) {
        return 1;
      } else {
        failedAttempts++;
        if (failedAttempts >= 3) {
          printf("\nToo many failed attempts. Locking out for 30 seconds.\n");
          int wait_time = 30;
          for (int i = wait_time; i >= 0; i--) {
            printf("%d \r", i);
            fflush(stdout);
            sleep(1);
          }
          printf("Lock released");
          printf("\n");
          failedAttempts = 0;
        } else {
          printf("\nInvalid username or password.\n");
          sleep(1);
          printf("\n1. Try again\n2. Register\n");
          printf("\nEnter your choice: ");
          scanf("%d", &choice);
          if (choice == 2) {
            registerUser();
          }
        }
      }
    } else {
      printf("Error occurred while opening the file.\n");
    }
  } while (choice == 1);
  return 0;
}
void cab_faq() {
  FILE *file = fopen("cab_faqs.txt", "r");
  if (file == NULL) {
    printf("Error opening the file.\n");
    return;
  }
  char line[256];
  while (fgets(line, sizeof(line), file) != NULL) {
    printf("%s", line);
  }

  fclose(file);
  printf("Redirecting to Menu...");
  sleep(3);
} //profile.h
void update_profile() {
    FILE *file = fopen("users.txt", "r+");
    if (file == NULL) {
        printf("\nError opening the file.\n");
        return;
    }
	while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(user.username, username_check) == 0) {
            printf("Select the detail you want to change:\n");
            printf("1. First Name\n");
            printf("2. Last Name\n");
            printf("3. Phone Number\n");
            printf("4. Email\n");
            printf("Enter your choice: ");

            int choice;
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    printf("Enter new First Name: ");
                    scanf("%s", user.first_name);
                    break;
                case 2:
                    printf("Enter new Last Name: ");
                    scanf("%s", user.last_name);
                    break;
                case 3:
                    printf("Enter new Phone Number: ");
                    scanf("%s", user.phoneNumber);
                    break;
                case 4:
                    printf("Enter new Email: ");
                    scanf("%s", user.email);
                    break;
                default:
                    printf("Invalid choice.\n");
                    fclose(file);
                    return;
            }

            fseek(file, -sizeof(User), SEEK_CUR);
            fwrite(&user, sizeof(User), 1, file);

            printf("Profile updated successfully!\n");
			printf("Redirecting..");
			sleep(3);
            break;
        }
    }

    fclose(file);
}

void view_profile() {
  FILE *file = fopen("users.txt", "r");
  if (file == NULL) {
    printf("\nError opening the file.\n");
    return;
  }

  User user;
  int found = 0;
  while (fread(&user, sizeof(User), 1, file)) {
	  
    if (strcasecmp(user.username, username_check) == 0 &&
        strcmp(user.password, password_check) == 0) {
      printf("Name: %s %s\n", user.first_name, user.last_name);
      printf("Phone Number: %s\n", user.phoneNumber);
      printf("Email: %s\n", user.email);
      printf("\n");
      found = 1;
      printf("Redirecting to Menu...\n");
      sleep(3);
      break;
    }
  }

  if (!found) {
    printf("User not found.\n");
  }

  fclose(file);
} 
int profile();
void cab_allotment() {
    Graph graph;
    initGraph(&graph);
    CabType cabTypes[5];

strcpy(cabTypes[0].name, "Bike");
cabTypes[0].fareRate = 7.5;
cabTypes[0].availability = 1;
cabTypes[0].fareperkm = 2.5;

strcpy(cabTypes[1].name, "Auto");
cabTypes[1].fareRate = 10.0;
cabTypes[1].availability = 1;
cabTypes[1].fareperkm = 5.0;
  
strcpy(cabTypes[2].name, "SUV");
cabTypes[2].fareRate = 30.0;
cabTypes[2].availability = 1;
cabTypes[2].fareperkm = 20.5;

strcpy(cabTypes[3].name, "Mini");
cabTypes[3].fareRate = 15.0;
cabTypes[3].availability = 1;
cabTypes[3].fareperkm = 10.5;
  
strcpy(cabTypes[4].name, "Sedan");
cabTypes[4].fareRate = 25.0;
cabTypes[4].availability = 1;
cabTypes[4].fareperkm = 12.5;
  
int pickupLocation, dropLocation;
    char carType[20];

    int choice = 0;
    while (choice != 5) {
        printf("\nMenu:\n");
        printf("1. Spot Booking\n");
        printf("2. Advance Booking\n");
		printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                printLocations(&graph);
                printf("\n\nEnter pickup location ID: ");
                scanf("%d", &pickupLocation);
                printf("\nEnter drop location ID: ");
                scanf("%d", &dropLocation);
                sleep(2);
                // Display available cab types
                int y = dijkstracp(&graph, pickupLocation, dropLocation);
                displayAvailableCabTypes(cabTypes, y);
                sleep(2);
                printf("\n\nEnter the cab type (0-4): ");
                int cabChoice;
                scanf("%d", &cabChoice);

                // Validate the cab choice
                while (cabChoice < 0 || cabChoice > 4 || cabTypes[cabChoice].availability == 0) {
                    printf("Invalid cab type choice! Please enter a valid cab type (0-4): ");
                    scanf("%d", &cabChoice);
                }

                strcpy(carType, cabTypes[cabChoice - 1].name);

               checkCabInput(carType,cabChoice); // Validate the cab type input
                sleep(2);
              
              
                float j = displayest(cabTypes, y, cabChoice);
                printf("\nEstimated cost: %.2f\n\n", j);

                // Ask for confirmation
                char confirm;
                printf("Confirm allotting the cab? (y/n): ");
                scanf(" %c", &confirm);
               
                if (confirm == 'y' || confirm == 'Y') {
                    printf("\nShortest Path:\n");
                    sleep(2);
                    dijkstra(&graph, pickupLocation, dropLocation);
                    
                    printf("\nAllot Cab:\n");
                    sleep(2);
                    allotCab(&graph, pickupLocation, dropLocation, carType);
    
                    printf("\n");
                   printf("\n");
                  int choice1 = 0;
                  printf("1.Cancel the booking!\n");
                  printf("\n2.Continue\n");
				  printf("\n\nEnter the choice:");
                  scanf("%d",&choice1);
                  switch (choice1){
                    case 1:{
                      sleep(2);
                      float cancellationFee = 0.3 * j;
                      printf("\nCancellation Fee: Rs %.2f\n", cancellationFee);
                      sleep(2);
                       printf("\n\n---------------------BILL-------------------");
                      float estimatedCost = displayest(cabTypes, y, cabChoice);
                      float finalPrice = estimatedCost;
                      printf("\n\n\nFinal Price: Rs %.2f\n", cancellationFee);
                      printf("\n\n---------------THANK YOU FOR BOOKING---------------");
                      break;
                    }
                    case 2:{
                      sleep(2);
                      double surgeFee = getSurgeFee();
                      float estimatedCost = displayest(cabTypes, y, cabChoice);
                      float finalPrice = estimatedCost * surgeFee;
                      sleep(2);
                      printf("\n\n------------------BILL------------------");
                printf("\n\nSurge Fee: %.1f%%\n\n", (surgeFee - 1.0) * 100.0);
                      sleep(2);
                printf("\n\nFinal Price: Rs %.2f\n", finalPrice);
                   printf("\n\n---------------THANK YOU FOR BOOKING---------------");  
                   collectUserFeedback();
                    
                }   
                }
                } else{
                    // Ask for further action
                    char action;
                    printf("\n\nWhat would you like to do?\n");
                    printf("1. Cancel cab booking\n");
                    printf("2. Reselect cab type\n");
                    printf("\nEnter your choice (1-2): ");
                    scanf(" %c", &action);
                    sleep(2);
                    while (action != '1' && action != '2') {
                        printf("Invalid choice! Please enter a valid choice (1-2): ");
                        scanf(" %c", &action);
                    }

                    if (action == '1') {
                        sleep(2);
                      
                        printf("Your cab has been cancelled!.\n");
                       double surgeFee = getSurgeFee();
                      
                      sleep(2);
                      printf("\n\n------------------BILL------------------");
                printf("\n\nSurge Fee: %.1f%%\n\n", (surgeFee - 1.0) * 100.0);
                      sleep(2);
                      printf("\nCancellation fee : Nil");
                printf("\n\nFinal Price: Rs 0\n");
                   printf("\n\n---------------THANK YOU FOR BOOKING---------------");
                    } else {
                      sleep(2);
                      printf("\n\n Re-directing!!\n\n");
                        // Display available cab types and prices again
                        displayAvailableCabTypes(cabTypes, y);
                        sleep(2);
                        printf("\nEnter the cab type (0-4) to book: ");
                        scanf("%d", &cabChoice);

                        // Validate the cab choice
                        while (cabChoice < 0 || cabChoice > 4 || cabTypes[cabChoice].availability == 0) {
                            sleep(2);
                            printf("Invalid cab type choice! Please enter a valid cab type (0-4): ");
                            scanf("%d", &cabChoice);
                        }

                        strcpy(carType, cabTypes[cabChoice - 1].name);
                        sleep(2);
                        checkCabInput(carType,cabChoice); // Validate the cab type input
                        float j = displayest(cabTypes, y, cabChoice);
                        sleep(2);
                        printf("\nEstimated cost: %.2f\n\n", j);
                        
                        printf("\nShortest Path:\n");
                      sleep(2);
                        dijkstra(&graph, pickupLocation, dropLocation);

                        printf("\nAllot Cab:\n");
                      sleep(2);
                        allotCab(&graph, pickupLocation, dropLocation, carType);

                        printf("\n");
                     

                      double surgeFee = getSurgeFee();
                float estimatedCost = displayest(cabTypes, y, cabChoice);
                float finalPrice = estimatedCost * surgeFee;
                  sleep(2);
                    printf("\n\n--------------BILL---------------");
                printf("\n\nSurge Fee: %.1f%%\n\n", (surgeFee - 1.0) * 100.0);
                      sleep(2);
                printf("\nFinal Price: Rs %.2f\n", finalPrice);
                   printf("\n\n---------------THANK YOU FOR BOOKING---------------"); 
                   collectUserFeedback();
                    }
            	}
			break;
            }
		
			case 2: {
			printLocations(&graph);
                 do {
                   sleep(2);
                    printf("Enter pickup location ID (0-24): ");
                    if (scanf("%d", &pickupLocation) != 1) {
                        printf("Invalid input! Please enter a numeric value.\n");
                        while (getchar() != '\n'); // Clear input buffer
                        continue;
                    }
                    if (pickupLocation < 0 || pickupLocation > 24) {
                      sleep(2);
                        printf("Invalid pickup location ID! Please enter a value between 0 and 24.\n");
                    }
                } while (pickupLocation < 0 || pickupLocation > 24);

                do {
                    printf("Enter drop location ID (0-24): ");
                    if (scanf("%d", &dropLocation) != 1) {
                   
                        printf("Invalid input! Please enter a numeric value.\n");
                        while (getchar() != '\n'); // Clear input buffer
                        continue;
                    }
                    if (dropLocation < 0 || dropLocation > 24) {
                        printf("Invalid drop location ID! Please enter a value between 0 and 24.\n");
                    }
                } while (dropLocation < 0 || dropLocation > 24);
                time_t rawTime = time(NULL);
                struct tm* timeInfo = localtime(&rawTime);

                timeInfo->tm_hour += 5;
                timeInfo->tm_min += 30;

                if (timeInfo->tm_min >= 60) {
                  timeInfo->tm_hour += 1;
                  timeInfo->tm_min -= 60;}
				
                if (timeInfo->tm_hour >= 24) {
                  timeInfo->tm_hour -= 24;}

                char timeStr[9]; 
                strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeInfo);

                printf("Current IST time: %s\n", timeStr);
                printf("***NOTE THAT THE BOOKING TIME SHOULD LAY WITHTIN THE RANGE OF 2 TO 10 HOURS ON THE HOUR BOOKING***\n ");

                BookingRequest request; 

                printf("Enter booking time (railway timings, HH:MM format): ");
                scanf("%s", request.requestedTime);

                if (!isBookingTimeValid(request.requestedTime)) {
                   printf("Invalid booking time.The booking time should lay within the range of 2 to 10 hours on the hour booking.\n");
                  break;
                  }
                float y = dijkstracp(&graph, pickupLocation, dropLocation);
                displayAvailableCabTypes(cabTypes, y);
                printf("Enter the cab type (0-4): ");
                int cabChoice;
                scanf("%d", &cabChoice);
                
                // Validate the cab choice
                while (cabChoice < 0 || cabChoice > 4 || cabTypes[cabChoice].availability == 0) {
                    printf("Invalid cab type choice! Please enter a valid cab type (0-4): ");
                    scanf("%d", &cabChoice);
                }

                strcpy(carType, cabTypes[cabChoice - 1].name);
				
				        checkCabInput(carType,cabChoice);
                float j = displayest(cabTypes, y, cabChoice);
                printf("\nEstimated cost: %.2f\n\n", j);
                printf("\nAdvance Booking Fee: %.2f\n\n",j*0.2);
                
                char confirm;
                printf("Confirm allotting the cab? (y/n): ");
                scanf(" %c", &confirm);
                if (confirm == 'y' || confirm == 'Y') {
                  printf("Please Enter cab type for confirmation:");
                  scanf("%s", request.cabType);
                

            
				          printf("\nShortest Path:\n");
                  dijkstra(&graph, pickupLocation, dropLocation);
                  printf("\n");
                  int assignedDriverIndices;
                  FILE* file = fopen("drivers.txt", "r");
                  if (file == NULL) {
                  printf("Error opening file.\n");
                   }

                  int numDrivers;
                  fscanf(file, "%d", &numDrivers);
                  driver* drivers = malloc(numDrivers * sizeof(driver));

                  int i;
                  for (i = 0; i < numDrivers; i++) {
                    fscanf(file, "%s %s %s %lf", drivers[i].name, drivers[i].phoneNumber, drivers[i].carPlateNumber,&drivers[i].rating);
                  }

                  fclose(file);

                  assignDriver(request.cabType, drivers, numDrivers);
                  printf("\n");
                  printf("\n");
                  int choice1 = 0;
                  printf("1.Cancel the booking!\n");
                  printf("\n2.Continue\n");
				          printf("\n\nEnter the choice:");
                  scanf("%d",&choice1);
                  switch (choice1){
                    case 1:{
                      sleep(2);
                      float cancellationFee = 0.3 * j;
                      printf("\nCancellation Fee: Rs %.2f\n", cancellationFee);
                      sleep(2);
                       printf("\n\n---------------------BILL-------------------");
                      float estimatedCost = displayest(cabTypes, y, cabChoice);
                      float finalPrice = estimatedCost;
                      printf("\n\n\nFinal Price: Rs %.2f\n", cancellationFee);
                      printf("\n\n---------------THANK YOU FOR BOOKING---------------");
                      break;
                    }
                    case 2:{
                      float estimatedCost = displayest(cabTypes, y, cabChoice);
                      float finalPrice = estimatedCost + estimatedCost*0.2;

                      printf("-------------------------------------------THE BILL-----------------------------");
                
                      printf(ANSI_UNDERLINE "\nFinal Price: Rs %.2f" ANSI_COLOR_RESET "\n", finalPrice);
                      printf("\n------------------------------------------THANK YOU FOR CHOOSING VIVAR:)-----------------------------------------");
                      free(drivers);
                      collectUserFeedback();
                    }
                  }
                  } else {
                    
					
                    // Ask for further action
                    char action;
                    printf("What would you like to do?\n");
                    printf("1. Cancel cab booking\n");
                    printf("2. Reselect cab type\n");
                    printf("Enter your choice (1-2): ");
                    scanf(" %c", &action);

                    while (action != '1' && action != '2') {
                        printf("Invalid choice! Please enter a valid choice (1-2): ");
                        scanf(" %c", &action);
                    }

                    if (action == '1') {
                      sleep(2);
                      printf("Your cab has been cancelled!.\n");
                      sleep(2);
                      printf("\n\n------------------BILL------------------");
                      printf("\nCancellation fee : Nil");
                      printf("\n\nFinal Price: Rs 0\n");
                      printf("\n\n---------------THANK YOU FOR BOOKING---------------");
                     } else {
                      printf("\n\n Re-directing!!\n\n");
                        // Display available cab types and prices again
                        displayAvailableCabTypes(cabTypes, y);
                        printf("Enter the cab type (0-4): ");
                        int cabChoice;
                        scanf("%d", &cabChoice);
                
                // Validate the cab choice
                        while (cabChoice < 0 || cabChoice > 4 || cabTypes[cabChoice].availability == 0) {
                           printf("Invalid cab type choice! Please enter a valid cab type (0-4): ");
                           scanf("%d", &cabChoice);
                        }

                        strcpy(carType, cabTypes[cabChoice - 1].name);
				
				                checkCabInput(carType,cabChoice);
                        float j = displayest(cabTypes, y, cabChoice);
                        printf("\nEstimated cost: %.2f\n\n", j);
                        printf("\nAdvance Booking Fee: %.2f\n\n",j*0.2);
                        printf("Please Enter cab type for confirmation:");
                        scanf("%s", request.cabType);
                        printf("\nShortest Path:\n");
                        dijkstra(&graph, pickupLocation, dropLocation);
                        printf("\n");
                        int assignedDriverIndices;
                        FILE* file = fopen("drivers.txt", "r");
                        if (file == NULL) {
                          printf("Error opening file.\n");
                        }

                        int numDrivers;
                        fscanf(file, "%d", &numDrivers);
                        driver* drivers = malloc(numDrivers * sizeof(driver));

                        int i;
                        for (i = 0; i < numDrivers; i++) {
                           fscanf(file, "%s %s %s %lf", drivers[i].name, drivers[i].phoneNumber, drivers[i].carPlateNumber, &drivers[i].rating);
                        }

                        fclose(file);

                        assignDriver(request.cabType, drivers, numDrivers);
                        float estimatedCost = displayest(cabTypes, y, cabChoice);
                        float finalPrice = estimatedCost + estimatedCost*0.2;

                        printf("-------------------------------------------THE BILL-----------------------------");
                
                        printf(ANSI_UNDERLINE "\nFinal Price: Rs %.2f" ANSI_COLOR_RESET "\n", finalPrice);
                        printf("\n------------------------------------------THANK YOU FOR CHOOSING VIVAR:)-----------------------------------------");
                       collectUserFeedback();
                    }                      
                                          
                                    
}   
            }
                
				
			case 3:{
                printf("\nExiting...\n");
				profile();
                break;}
            default:{
                printf("Invalid choice! Please try again.\n");
                break;
        }
    }

   
}
}
void Users() {
  sleep(2);
  printf("\n\n----------Welcome to User Portal!----------\n");
  int choice2;
  int shouldRepeat2 = 1;
  while (shouldRepeat2) {
    printf("\n1. Register\n2. Login\n");
    printf("\nEnter your choice: ");
    scanf("%d", &choice2);
    switch (choice2) {
    case 1:
      registerUser();
      break;
    case 2: {
      if (loginUser()) {
        profile();
        shouldRepeat2 = 0;
      }
      break;
    }
    default: {
      printf("Invalid choice.\n");
      break;
    }
    }
  }
} 
int profile() {
  int shouldRepeat3 = 1, choice3 = 0;
  while (shouldRepeat3) {
    printf("\n-----------Welcome to VIVAR-----------\n");
    printf("\n1. View Profile\n");
	printf("2. Update Profile\n");
    printf("3. Cab Allotment\n");
    printf("4. Cab FAQ\n");
	printf("5. Exit: ");
    printf("\nEnter your choice: ");
    scanf(" %d", &choice3);
    switch (choice3) {
    case 1: {
      printf("\n");
      printf("\n---------Your Details---------\n");
      view_profile();
      break;
    }
	case 2:{
		update_profile();
		break;
	}
    case 3: {
      cab_allotment();
      shouldRepeat3 = 0;
      break;
    }
    case 4: {
      cab_faq();
      break;
    }
	case 5:
	{
		Users();
		break;
	}
    default: {
      printf("Invalid choice.\n");
      break;
    }
    }
  }
  return 0;
}

int main() {
  int choice1;
  int w;
  char password[15];
  int shouldRepeat1 = 1;
   welcomeMessage();
  while (shouldRepeat1) {
    printf("Menu:\n");
    printf("1. Admin\n");
    printf("2. User\n");
    printf("3. Exit\n");
    printf("\nEnter your choice: ");
    scanf("%d", &choice1);
    getchar(); 
    switch (choice1) {
    case 1: {
      printf("You selected Admin.\n");
      do {
        printf("\nEnter password: ");
        maskPasswordInput(password);
        printf("\n");
        if (!strcmp(password, "roaster")) {
          w = 1;
          printf("\n\n-------------Welcome Admin---------------\n\n");
          Admin();
          printf("Admin code executed.\n");
          printf("Returning to the menu.\n");
          break;
        } else {
          w = 0;
          printf("Password incorrect(try again)!!");
        }
      } while (w != 1);
      break;
    }
    case 2: {
      printf("You selected User.\n");
      Users();
      shouldRepeat1 = 0;
      break;
    }
    case 3: {
      printf("Exiting the program.\n");
      shouldRepeat1 = 0;
      break;
    }
    default: {
      printf("Invalid choice. Please try again.\n");
      break;
    }
    }
    printf("\n");
  }
  return 0;
}
