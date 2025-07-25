#ifndef LOADCSVDATA_H
#define LOADCSVDATA_H

#include "utility.h" // <--- THIS IS THE CRUCIAL FIX: Include utility.h first for common definitions
#include <stdio.h>   // For FILE type

// --- Constants (relevant to CSV parsing) ---
#define BUFFER_SZ 1024
#define STU_COL_NUM 4
#define BK_COL_NUM 7
#define PASS 1
#define FAIL 0
#define FALSE 0
#define TRUE 1

#define STU_DATA_CSV_FILE "StudentData.csv"
#define BK_DATA_CSV_FILE "BookData.csv"
#define TMP_BK_DATA_CSV_FILE "tmpBookData.csv"

// --- Field Offsets for CSV Parsing ---
// Student Profile data:
#define STU_ID_COL 0
#define STU_FNAME_COL 1
#define STU_LNAME_COL 2
#define STU_FACULTY_COL 3

// Books data:
#define BOOK_ID_COL 0
#define TITLE_COL 1
#define AUTHOR_COL 2
#define GENRE_COL 3
#define YEAR_COL 4
#define STK_CNT_COL 5
#define SHELF_NO_COL 6

// --- Function Prototypes ---
// Note: StuProfile is now UserProfile, but I'll keep the name for consistency with your original CSV parsing
// This function name is slightly misleading as it only reads from CSV, not finds in the new UserData.dat
int getStudentProfileFromCSV(char stuId[MAX_ID], UserProfile *st); // Changed StuProfile to UserProfile
// static int loadStudentValuesFromCSV(char *line, char *sFields[STU_COL_NUM]); // Removed
int readBooksFromCSV();
// static int loadBookValuesFromCSV(char *line, char *lFields[BK_COL_NUM]); // Removed
int getRowCountFromCSV(FILE *fpl);
void writeBooksToCSV();

#endif // LOADCSVDATA_H
