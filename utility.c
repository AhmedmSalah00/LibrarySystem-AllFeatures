#include "utility.h" // Include its own header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// For _getch() on Windows
#ifdef _WIN32
#include <conio.h>
#endif

// Global variables defined here (as they are extern in utility.h)
Book *books_array = NULL;
int bookCount = 0;
UserProfile currentLoggedInUser;

// --- General Utility Function Implementations ---

/**
 * @brief Basic portable getch replacement.
 * On Windows, uses _getch(). On other systems, uses getchar() which requires Enter.
 * @return The character read.
 */
int getch_portable() {
#ifdef _WIN32
    return _getch();
#else
    // For non-Windows, getchar() is used. This means user must press Enter.
    int c = getchar();
    // Clear remaining buffer if it's not a newline or EOF.
    // This is crucial to prevent subsequent fgets/getchar from reading leftover newlines.
    if (c != '\n' && c != EOF) {
        int temp_c;
        while ((temp_c = getchar()) != '\n' && temp_c != EOF);
    }
    return c;
#endif
}

/**
 * @brief Gets a string input from the user.
 * @param buff Pointer to the character array to store the string.
 * @param sz Maximum size of the buffer.
 */
void getString(char *buff, size_t sz) {
    if (fgets(buff, sz, stdin) != NULL) {
        // Find the newline character
        char *newline_pos = strchr(buff, '\n');
        if (newline_pos != NULL) {
            *newline_pos = '\0'; // Replace newline with null terminator
        } else {
            // If no newline found, it means the buffer was too small or EOF was reached.
            // In this case, clear the rest of the input line to prevent issues with next input.
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } else {
        // Handle error or EOF for fgets
        buff[0] = '\0'; // Ensure buffer is empty
    }
}

/**
 * @brief Gets a password input from the user, displaying asterisks.
 * @param buff Pointer to the character array to store the password.
 * @param sz Maximum size of the buffer.
 */
void getPassword(char *buff, size_t sz) {
    char ch;
    int i = 0;

    while (1) {
        ch = getch_portable(); // Use portable getch

        if (ch == 13) { // Enter key
            break;
        } else if (ch == 8) { // Backspace key
            if (i > 0) {
                printf("\b \b"); // Erase character from console
                i--;
                buff[i] = '\0';
            }
        } else if (i < sz - 1) { // Store character if buffer not full
            if (isprint((unsigned char)ch)) {
                putch('*'); // Display asterisk
                buff[i] = ch;
                i++;
            }
        }
    }
    buff[i] = '\0'; // Null-terminate the string
    printf("\n"); // Newline after password input
}

/**
 * @brief Prints the standard welcome message and clears the screen.
 */
void printWelcomeMsg() {
    system("cls || clear"); // Clear console for Windows or Linux/macOS
    printf("\n---- Welcome to Library Management System ----\n");
    printf("----------------------------------------------\n\n");
}

/**
 * @brief Prints the user welcome message with user details and clears the screen.
 * @param user The UserProfile struct to display.
 */
void printUserWelcomeMsg(UserProfile user) {
    system("cls || clear"); // Clear console for Windows or Linux/macOS
    printf("User: %s %s (ID: %s)\n", user.fName, user.lName, user.userID); // MODIFIED: Display user's full name
    printf("Faculty/Type: %s\n", user.faculty);
    if (user.fineAmount > 0) {
        printf("Outstanding Fine: %d\n", user.fineAmount);
    }
    if (strcmp(user.forbiddenUntil, "N/A") != 0 && compareDates(user.forbiddenUntil, "CURRENT_DATE") >= 0) {
        printf("Forbidden to borrow until: %s\n", user.forbiddenUntil);
    }
    printf("\n---- Welcome to Library Management System ----\n");
    printf("----------------------------------------------\n\n");
}

/**
 * @brief Converts a string to an integer.
 * @param str The string to convert.
 * @return The integer value.
 */
int stringToInt(char *str) {
    int num = 0;
    if (str == NULL || *str == '\0') return 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (isdigit((unsigned char)str[i])) {
            num = num * 10 + (str[i] - '0');
        } else {
            return 0;
        }
    }
    return num;
}

/**
 * @brief Converts an integer to a string.
 * @param str Buffer to store the string.
 * @param num The integer to convert.
 */
void intToString(char *str, int num) {
    sprintf(str, "%d", num);
}

// --- Date Utility Function Implementations ---

/**
 * @brief Gets the current date in "DD-MM-YYYY" format.
 * @param dateStr Buffer to store the date string.
 * @param sz Size of the buffer.
 */
void getCurrentDate(char *dateStr, size_t sz) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info != NULL) {
        strftime(dateStr, sz, "%d-%m-%Y", tm_info);
    } else {
        strncpy(dateStr, "ERROR", sz);
        dateStr[sz - 1] = '\0';
    }
}

/**
 * @brief Converts a date string (DD-MM-YYYY) to a struct tm.
 * @param dateStr The date string.
 * @param tm_struct Pointer to a struct tm to populate.
 * @return 0 on success, -1 on failure.
 */
static int parseDate(const char *dateStr, struct tm *tm_struct) {
    if (dateStr == NULL || tm_struct == NULL) return -1;

    // Handle "CURRENT_DATE" keyword
    char actualDateStr[DATE_SZ];
    if (strcmp(dateStr, "CURRENT_DATE") == 0) {
        getCurrentDate(actualDateStr, DATE_SZ);
    } else {
        strncpy(actualDateStr, dateStr, DATE_SZ - 1);
        actualDateStr[DATE_SZ - 1] = '\0';
    }

    // Initialize tm_struct to zero to avoid undefined behavior
    memset(tm_struct, 0, sizeof(struct tm));

    // Use sscanf to parse DD-MM-YYYY
    int day, month, year;
    if (sscanf(actualDateStr, "%d-%d-%d", &day, &month, &year) != 3) {
        return -1; // Parsing failed
    }

    tm_struct->tm_mday = day;
    tm_struct->tm_mon = month - 1; // Month is 0-indexed (0-11)
    tm_struct->tm_year = year - 1900; // Year is years since 1900

    // Normalize and validate the date
    time_t raw_time = mktime(tm_struct);
    if (raw_time == (time_t)-1) {
        return -1; // Date is not representable or invalid
    }

    // Check if the parsed date matches the original date (for validation)
    char checkDateStr[DATE_SZ];
    strftime(checkDateStr, DATE_SZ, "%d-%m-%Y", tm_struct);
    if (strcmp(actualDateStr, checkDateStr) != 0) {
        return -1; // Date was invalid (e.g., Feb 30)
    }

    return 0; // Success
}

/**
 * @brief Checks if a date string is valid (DD-MM-YYYY).
 * @param dateStr The date string to validate.
 * @return 1 if valid, 0 otherwise.
 */
int isValidDate(const char *dateStr) {
    struct tm temp_tm;
    return (parseDate(dateStr, &temp_tm) == 0);
}

/**
 * @brief Converts a date string (DD-MM-YYYY) to total days since epoch.
 * This is a simplified approach for relative date calculations.
 * @param dateStr The date string (DD-MM-YYYY). Can be "CURRENT_DATE".
 * @return Total days since epoch, or -1 on error.
 */
long dateToDays(const char *dateStr) {
    struct tm tm_struct;
    if (parseDate(dateStr, &tm_struct) != 0) {
        return -1; // Error parsing date
    }
    // Convert struct tm to time_t (seconds since epoch)
    time_t raw_time = mktime(&tm_struct);
    if (raw_time == (time_t)-1) {
        return -1; // Error converting to time_t
    }
    // Convert seconds to days (approximate, ignoring leap seconds etc.)
    return raw_time / (60 * 60 * 24);
}

/**
 * @brief Converts total days since epoch back to a date string (DD-MM-YYYY).
 * @param days Total days since epoch.
 * @param dateStr Buffer to store the date string.
 * @param sz Size of the buffer.
 */
void daysToDate(long days, char *dateStr, size_t sz) {
    time_t raw_time = days * (60 * 60 * 24); // Convert days back to seconds
    struct tm *tm_info = localtime(&raw_time);
    if (tm_info != NULL) {
        strftime(dateStr, sz, "%d-%m-%Y", tm_info);
    } else {
        strncpy(dateStr, "ERROR", sz);
        dateStr[sz - 1] = '\0';
    }
}

/**
 * @brief Adds a number of days to a given date string and returns the new date string.
 * @param startDateStr The starting date string (DD-MM-YYYY).
 * @param daysToAdd The number of days to add.
 * @param newDateStr Buffer to store the new date string.
 * @param sz Size of the buffer.
 */
void addDaysToDate(const char *startDateStr, int daysToAdd, char *newDateStr, size_t sz) {
    struct tm tm_struct;
    if (parseDate(startDateStr, &tm_struct) != 0) {
        strncpy(newDateStr, "ERROR", sz);
        newDateStr[sz - 1] = '\0';
        return;
    }

    // Add days directly to tm_mday and let mktime normalize it
    tm_struct.tm_mday += daysToAdd;

    time_t new_time = mktime(&tm_struct);
    if (new_time == (time_t)-1) {
        strncpy(newDateStr, "ERROR", sz);
        newDateStr[sz - 1] = '\0';
        return;
    }

    struct tm *new_tm_info = localtime(&new_time);
    if (new_tm_info != NULL) {
        strftime(newDateStr, sz, "%d-%m-%Y", new_tm_info);
    } else {
        strncpy(newDateStr, "ERROR", sz);
        newDateStr[sz - 1] = '\0';
    }
}

/**
 * @brief Compares two date strings (DD-MM-YYYY).
 * @param date1Str The first date string. Can be "CURRENT_DATE".
 * @param date2Str The second date string. Can be "CURRENT_DATE".
 * @return <0 if date1 < date2, 0 if date1 == date2, >0 if date1 > date2.
 * Returns -2 on parsing error for either date.
 */
int compareDates(const char *date1Str, const char *date2Str) {
    struct tm tm1, tm2;

    if (parseDate(date1Str, &tm1) != 0 || parseDate(date2Str, &tm2) != 0) {
        return -2; // Error parsing one or both dates
    }

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 == (time_t)-1 || time2 == (time_t)-1) {
        return -2; // Error converting to time_t
    }

    // Compare time_t values
    if (time1 < time2) return -1;
    if (time1 > time2) return 1;
    return 0;
}

/**
 * @brief Calculates the number of days between two dates (date2 - date1).
 * @param date1Str The first date string (DD-MM-YYYY). Can be "CURRENT_DATE".
 * @param date2Str The second date string (DD-MM-YYYY). Can be "CURRENT_DATE".
 * @return The number of days, or -1 on error.
 */
long daysBetweenDates(const char *date1Str, const char *date2Str) {
    struct tm tm1, tm2;

    if (parseDate(date1Str, &tm1) != 0 || parseDate(date2Str, &tm2) != 0) {
        return -1; // Error parsing one or both dates
    }

    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    if (time1 == (time_t)-1 || time2 == (time_t)-1) {
        return -1; // Error converting to time_t
    }

    // Calculate difference in seconds and convert to days
    long seconds_diff = (long)difftime(time2, time1);
    return seconds_diff / (60 * 60 * 24);
}
