#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h> // For system()
#include <string.h> // For strlen, strcpy, strcspn
#include <ctype.h>  // For isdigit, tolower
#include <time.h>   // For time_t, struct tm, localtime, mktime, strftime, strptime

// --- Global Constants ---
#define ESC 27 // ASCII value for ESC key
#define MAX_USR_SZ 20 // Max size for username
#define MAX_PWD_SZ 20 // Max size for password
#define MAX_ID 10     // Max size for BookID, UserID, ShelfNo, StockCount
#define MAX_NAME 50   // Max size for Author, Genre, Faculty, First/Last Name
#define MAX_TITLE 100 // Max size for Book Title
#define MAX_YR 5      // Max size for Year of Publication (e.g., "YYYY")
#define DATE_SZ 64    // Max size for date strings (e.g., "DD-MM-YYYY")
#define READ_ARRAY_SZ 20 // Max items to read from file into array at once (e.g., for pagination)

#define PUBLIC_LIBRARY 1
#define SCHOOL_LIBRARY 2

#define FINE_PER_DAY 5 // Example fine amount per day (e.g., 5 currency units)
#define FORBIDDEN_DAYS 7 // Days a user is forbidden after late return

#define PASS 1
#define FAIL 0
#define FALSE 0
#define TRUE 1

// --- Structures ---

// Structure for Admin Accounts
typedef struct {
    char userName[MAX_USR_SZ];
    char password[MAX_PWD_SZ];
} AdminAccount;

// Structure for User Profiles (Students/Public Library Users)
typedef struct {
    char userID[MAX_ID];
    char password[MAX_PWD_SZ]; // ADDED: Password for user login
    char fName[MAX_NAME];
    char lName[MAX_NAME];
    char faculty[MAX_NAME]; // For school, or "Public" for public library
    int fineAmount;         // Accumulated fine for public library users
    char forbiddenUntil[DATE_SZ]; // Date (DD-MM-YYYY) until user is forbidden from borrowing
} UserProfile;

// Structure for Books in the Catalog
typedef struct {
    char bookID[MAX_ID];
    char title[MAX_TITLE];
    char author[MAX_NAME];
    char genre[MAX_NAME];
    char year[MAX_YR];
    char stockCount[MAX_ID]; // Stored as string, converted to int for operations
    char shelfNo[MAX_ID];
} Book;

// Structure for Borrowed Book Records
typedef struct {
    char bookID[MAX_ID];
    char userID[MAX_ID];
    char issueDate[DATE_SZ]; // Date borrowed (DD-MM-YYYY)
    char dueDate[DATE_SZ];   // Date due (DD-MM-YYYY)
    char returnDate[DATE_SZ]; // Date returned (DD-MM-YYYY) or "N/A" if not returned
} BorrowedBook;


// --- Global Variables (declared extern here, defined in utility.c) ---
extern Book *books_array; // Dynamic array for books
extern int bookCount;     // Current number of books loaded
extern UserProfile currentLoggedInUser; // Stores profile of currently logged-in user


// --- Function Prototypes ---

// General Utilities
void getString(char *buff, size_t sz);
void getPassword(char *buff, size_t sz); // Re-introduced for admin login
void printWelcomeMsg();
void printUserWelcomeMsg(UserProfile user); // Renamed from printStudentWelcomeMsg
int stringToInt(char *str);
void intToString(char *str, int num);
int getch_portable(); // Basic portable getch replacement

// Date Utilities
void getCurrentDate(char *dateStr, size_t sz);
// Converts a date string (DD-MM-YYYY) to total days since 0000-00-00 (arbitrary epoch)
long dateToDays(const char *dateStr);
// Converts total days back to a date string (DD-MM-YYYY)
void daysToDate(long days, char *dateStr, size_t sz);
// Adds a number of days to a given date string and returns the new date string
void addDaysToDate(const char *startDateStr, int daysToAdd, char *newDateStr, size_t sz);
// Compares two date strings (DD-MM-YYYY)
// Returns <0 if date1 < date2, 0 if date1 == date2, >0 if date1 > date2
int compareDates(const char *date1Str, const char *date2Str);
// Calculates the number of days between two dates (date2 - date1)
long daysBetweenDates(const char *date1Str, const char *date2Str);
// Checks if a date string is valid (DD-MM-YYYY)
int isValidDate(const char *dateStr);


#endif // UTILITY_H
