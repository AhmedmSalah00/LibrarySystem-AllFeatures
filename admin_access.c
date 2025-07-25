#include "admin_access.h" // Include its own header
#include "utility.h"      // For AdminAccount, getString, getPassword, printWelcomeMsg, etc.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For tolower

/**
 * @brief Checks if an admin account exists based on username and password (flag=0)
 * or just username (flag=1).
 * @param p AdminAccount struct containing username and password to check.
 * @param flag 0 for full check, 1 for username only check.
 * @return 1 if found, 0 otherwise.
 */
int checkAdmin(AdminAccount p, int flag) {
    FILE *fp = fopen(ADMIN_DATA_FILE, "rb");
    if (fp == NULL) return 0; // File doesn't exist, so no admin accounts

    AdminAccount a;
    int found = 0;
    while (fread(&a, sizeof(a), 1, fp) == 1) {
        if (flag == 0) { // Check username and password
            if (strcmp(a.userName, p.userName) == 0 && strcmp(a.password, p.password) == 0) {
                found = 1;
                break;
            }
        } else if (flag == 1) { // Check username only
            if (strcmp(a.userName, p.userName) == 0) {
                found = 1;
                break;
            }
        }
    }
    fclose(fp);
    return found;
}

/**
 * @brief Reads all admin accounts from the file into an array.
 * @param p Array to store admin accounts.
 * @param sz Maximum size of the array.
 * @return Number of accounts read.
 */
int viewAdmins(AdminAccount p[], int sz) {
    FILE *fp = fopen(ADMIN_DATA_FILE, "rb");
    if (fp == NULL) return 0; // File doesn't exist

    AdminAccount a;
    int i = 0;
    while (fread(&a, sizeof(a), 1, fp) == 1 && i < sz) {
        p[i] = a;
        i++;
    }
    fclose(fp);
    return i;
}

/**
 * @brief Adds a new admin account to the file.
 * @param p AdminAccount struct containing the new admin's details.
 */
void addAdmin(AdminAccount p) {
    FILE *fp = fopen(ADMIN_DATA_FILE, "ab+"); // Open in append binary mode
    if (fp == NULL) {
        printf("Error: Could not open admin data file '%s' for adding.\n", ADMIN_DATA_FILE);
        return;
    }
    fwrite(&p, sizeof(p), 1, fp);
    fclose(fp);
}

/**
 * @brief Updates an existing admin account's password.
 * @param p AdminAccount struct containing the username and new password.
 * @return 1 if updated, 0 if username not found.
 */
int updateAdmin(AdminAccount p) {
    FILE *fp = fopen(ADMIN_DATA_FILE, "rb+"); // Open in read/write binary mode
    if (fp == NULL) {
        printf("Error: Could not open admin data file '%s' for updating.\n", ADMIN_DATA_FILE);
        return 0;
    }

    AdminAccount a;
    int flag = 0;
    long currentPos;

    while (fread(&a, sizeof(a), 1, fp) == 1) {
        if (strcmp(a.userName, p.userName) == 0) {
            currentPos = ftell(fp); // Get current position (after reading 'a')
            fseek(fp, currentPos - sizeof(a), SEEK_SET); // Go back to the start of the record
            fwrite(&p, sizeof(p), 1, fp); // Overwrite with updated data
            flag = 1;
            break; // Found and updated, exit loop
        }
    }
    fclose(fp);
    return flag;
}

/**
 * @brief Deletes an admin account from the file.
 * Creates a temporary file, copies all records except the one to delete,
 * then replaces the original file with the temporary one.
 * @param userName The username of the account to delete.
 * @return 1 if deleted, 0 if username not found.
 */
int deleteAdmin(char *userName) {
    FILE *fp = fopen(ADMIN_DATA_FILE, "rb");
    FILE *ft = fopen(ADMIN_TEMP_FILE, "wb");
    if (fp == NULL || ft == NULL) {
        if (fp) fclose(fp); // Close if opened
        if (ft) fclose(ft); // Close if opened
        printf("Error: Could not open admin data files for deletion.\n");
        return 0;
    }

    AdminAccount a;
    int flag = 0;
    while (fread(&a, sizeof(a), 1, fp) == 1) {
        if (strcmp(userName, a.userName) != 0) {
            fwrite(&a, sizeof(a), 1, ft); // Write to temp file if not the one to delete
        } else {
            flag = 1; // Account found and skipped (effectively deleted)
        }
    }

    fclose(ft);
    fclose(fp);

    remove(ADMIN_DATA_FILE); // Delete original file
    rename(ADMIN_TEMP_FILE, ADMIN_DATA_FILE); // Rename temp file to original

    return flag;
}

/**
 * @brief Checks if it's the first time an admin is logging in (i.e., no admin accounts exist).
 * This is determined by checking if the admin data file is empty or does not exist.
 * @return 1 if no admin accounts, 0 otherwise.
 */
int checkFirstAdminLogin() {
    FILE *fp = fopen(ADMIN_DATA_FILE, "rb");
    if (fp == NULL) {
        return 1; // File doesn't exist, so it's the first login
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp); // Get file size
    fclose(fp);
    return (size == 0); // If file is empty, it's the first login
}

/**
 * @brief Guides the user through creating the first admin account.
 * This function is called when no admin accounts are found.
 */
void createFirstAdminAccount() {
    char passV[MAX_PWD_SZ]; // For password verification
    AdminAccount newAdmin;

    printWelcomeMsg();
    printf("This is the first login. Please create an Admin account.\n\n");
    printf("New User Name: ");
    getString(newAdmin.userName, MAX_USR_SZ);
    printf("New Password: ");
    getPassword(newAdmin.password, MAX_PWD_SZ);
    printf("Re-type Password: ");
    getPassword(passV, MAX_PWD_SZ);

    if (strcmp(passV, newAdmin.password) == 0) {
        addAdmin(newAdmin); // Add the new admin account
        printWelcomeMsg();
        printf("Congratulations! Admin account created successfully.\n\n");
        printf("Press Enter to continue and log in.\n");
        getch_portable();
        adminLogin(); // Automatically proceed to login
    } else {
        printWelcomeMsg();
        printf("Error: Passwords do not match. Please try again.\n\n");
        printf("Press Enter to continue...\n");
        getch_portable();
        createFirstAdminAccount(); // Retry account creation
    }
}

/**
 * @brief Handles admin login process.
 * Prompts for username and password and validates against stored accounts.
 */
void adminLogin() {
    AdminAccount loginAttempt;

    printWelcomeMsg();
    printf("Please Enter Username and Password.\n\n");
    printf("User Name: ");
    getString(loginAttempt.userName, MAX_USR_SZ);
    printf("Password: ");
    getPassword(loginAttempt.password, MAX_PWD_SZ);

    if (checkAdmin(loginAttempt, 0) == 1) { // Check both username and password
        printWelcomeMsg();
        printf("Login Successful.\n\n");
        printf("Press Enter to continue.\n");
        getch_portable();
        // adminMenu() will be called from library_menus.c
    } else {
        printWelcomeMsg();
        printf("Sorry! Username or Password do not match.\n\n");
        printf("Press Enter to re-login.\n");
        getch_portable();
        adminLogin(); // Retry login
    }
}
