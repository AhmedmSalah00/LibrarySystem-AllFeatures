#ifndef USERMENU_H
#define USERMENU_H

#include "utility.h" // For common constants and structs

// --- Function Prototypes ---
void studentMenu();
int getBookIndexByID(char bookID[MAX_ID]);
void viewBooksByPage(int sIndex, int lIndex, int total, int isStudentView);
void viewAllBooks(int isStudentView);
void addBook();
void updateBook();
void deleteBook();
void searchBooks(int isStudentView);

#endif // USERMENU_H
