#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef _WIN32
#define CLEAR_SCREEN() system("cls")
#else
#define CLEAR_SCREEN() system("clear")
#endif

#define MAX_REPORTS 100
#define MAX_LENGTH 100

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_RED     "\x1b[31m"

typedef struct {
    char area[MAX_LENGTH];
    char incidentType[MAX_LENGTH];
    char date[MAX_LENGTH];
    char status[MAX_LENGTH];
} Report;

Report reports[MAX_REPORTS];
int reportCount = 0;

void ShowMenu();
void LoadReports();
void SaveReports();
void ReportIncident();
void DisplayAllIncidents();
void FilterIncidents();
void UpdateStatus();
void GetValidatedInput(const char* prompt, char* buffer, int isDate);
bool IsValidDateFormat(const char* date);

void ShowMenu() {
    CLEAR_SCREEN();
    printf("\n-------------------\n");
    printf("Incident Reporting System\n");
    printf("-------------------\n");
    printf("1. Report New Incident\n");
    printf("2. View All Incidents\n");
    printf("3. Filter Incidents\n");
    printf("4. Update Incident Status\n");
    printf("0. Exit\n");
    printf("-------------------\n");
}

void LoadReports() {
    FILE *file = fopen("incidents.txt", "r");
    if (file == NULL) return;

    reportCount = 0;
    while (fscanf(file, "Area: %[^\n]\n", reports[reportCount].area) == 1) {
        fscanf(file, "Type: %[^\n]\n", reports[reportCount].incidentType);
        fscanf(file, "Date: %[^\n]\n", reports[reportCount].date);
        fscanf(file, "Status: %[^\n]\n", reports[reportCount].status);
        fscanf(file, "---\n");
        reportCount++;
        if (reportCount >= MAX_REPORTS) break;
    }
    fclose(file);
}

void SaveReports() {
    FILE *file = fopen("incidents.txt", "w");
    if (file == NULL) {
        printf("Error saving data!\n");
        return;
    }

    for (int i = 0; i < reportCount; i++) {
        fprintf(file, "Area: %s\n", reports[i].area);
        fprintf(file, "Type: %s\n", reports[i].incidentType);
        fprintf(file, "Date: %s\n", reports[i].date);
        fprintf(file, "Status: %s\n", reports[i].status);
        fprintf(file, "---\n");
    }
    fclose(file);
}

void GetValidatedInput(const char* prompt, char* buffer, int isDate) {
    while (1) {
        printf("%s", prompt);
        scanf(" %[^\n]", buffer);

        if (strlen(buffer) == 0) {
            printf(COLOR_RED "Input cannot be empty. Please try again.\n" COLOR_RESET);
            continue;
        }

        if (isDate && !IsValidDateFormat(buffer)) {
            printf(COLOR_RED "Invalid date format! Please use dd/mm/yyyy.\n" COLOR_RESET);
            continue;
        }

        break;
    }
}

bool IsValidDateFormat(const char* date) {
    if (strlen(date) != 10)
        return false;

    return isdigit(date[0]) && isdigit(date[1]) &&
           date[2] == '/' &&
           isdigit(date[3]) && isdigit(date[4]) &&
           date[5] == '/' &&
           isdigit(date[6]) && isdigit(date[7]) &&
           isdigit(date[8]) && isdigit(date[9]);
}

void ReportIncident() {
    CLEAR_SCREEN();
    if (reportCount >= MAX_REPORTS) {
        printf("Maximum number of reports reached!\n");
        return;
    }

    printf("\n-- Report New Incident --\n");

    GetValidatedInput("Area where the incident happened (e.g: Center): ", reports[reportCount].area, 0);
    GetValidatedInput("Type of incident (e.g.: fire, theft): ", reports[reportCount].incidentType, 0);
    GetValidatedInput("Date of incident (dd/mm/yyyy): ", reports[reportCount].date, 1);

    strcpy(reports[reportCount].status, "Reported");

    reportCount++;
    SaveReports();
    printf(COLOR_GREEN "\nIncident reported successfully!\n" COLOR_RESET);
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

void DisplayAllIncidents() {
    CLEAR_SCREEN();
    printf("\n-- All Reported Incidents --\n");
    if (reportCount == 0) {
        printf("No incidents reported yet.\n");
    }

    for (int i = 0; i < reportCount; i++) {
        printf("\n%d. " COLOR_CYAN "%s" COLOR_RESET " - " COLOR_MAGENTA "%s" COLOR_RESET "\n", i+1, reports[i].area, reports[i].incidentType);
        printf("   Date: " COLOR_YELLOW "%s" COLOR_RESET "\n", reports[i].date);
        printf("   Status: " COLOR_GREEN "%s" COLOR_RESET "\n", reports[i].status);
    }
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

void FilterIncidents() {
    CLEAR_SCREEN();
    int choice;
    char filter[MAX_LENGTH];

    printf("\n-- Filter Incidents --\n");
    printf("1. Filter by Area\n");
    printf("2. Filter by Incident Type\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    printf("Enter search term: ");
    scanf(" %[^\n]", filter);

    CLEAR_SCREEN();
    printf("\n-- Filtered Results --\n");
    int found = 0;

    for (int i = 0; i < reportCount; i++) {
        int match = 0;
        if (choice == 1 && strstr(reports[i].area, filter) != NULL) match = 1;
        else if (choice == 2 && strstr(reports[i].incidentType, filter) != NULL) match = 1;

        if (match) {
            printf("\n%d. " COLOR_CYAN "%s" COLOR_RESET " - " COLOR_MAGENTA "%s" COLOR_RESET "\n", i+1, reports[i].area, reports[i].incidentType);
            printf("   Date: " COLOR_YELLOW "%s" COLOR_RESET "\n", reports[i].date);
            printf("   Status: " COLOR_GREEN "%s" COLOR_RESET "\n", reports[i].status);
            found = 1;
        }
    }

    if (!found) {
        printf(COLOR_RED "No matching incidents found.\n" COLOR_RESET);
    }
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

void UpdateStatus() {
    CLEAR_SCREEN();
    DisplayAllIncidents();
    if (reportCount == 0) {
        printf("\nPress Enter to continue...");
        getchar(); getchar();
        return;
    }

    int reportNum;
    printf("\nEnter report number to update: ");
    scanf("%d", &reportNum);

    if (reportNum < 1 || reportNum > reportCount) {
        printf(COLOR_RED "Invalid report number!\n" COLOR_RESET);
        printf("\nPress Enter to continue...");
        getchar(); getchar();
        return;
    }

    printf("Current status: " COLOR_GREEN "%s\n" COLOR_RESET, reports[reportNum-1].status);
    GetValidatedInput("Enter new status (e.g., Reported, In Progress, Resolved): ", reports[reportNum-1].status, 0);

    SaveReports();
    printf(COLOR_GREEN "Status updated successfully!\n" COLOR_RESET);
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

int main() {
    LoadReports();

    int option;
    do {
        ShowMenu();
        printf("Enter an option: ");
        scanf("%d", &option);

        switch (option) {
            case 1: ReportIncident(); break;
            case 2: DisplayAllIncidents(); break;
            case 3: FilterIncidents(); break;
            case 4: UpdateStatus(); break;
            case 0: printf("Exiting program...\n"); break;
            default:
                printf(COLOR_RED "Invalid option, please try again.\n" COLOR_RESET);
                printf("\nPress Enter to continue...");
                getchar(); getchar();
                break;
        }
    } while (option != 0);

    return 0;
}
