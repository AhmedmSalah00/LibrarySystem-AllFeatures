#include "user_management.h" // Include its own header
#include "utility.h"         // For UserProfile, getString, printWelcomeMsg, etc.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Adds a new user profile to the file.
 * @param user UserProfile struct containing the new user's details.
 */
void addUser(UserProfile user) {
    FILE *fp = fopen(USER_DATA_FILE, "ab+"); // Open in append binary mode
    if (fp == NULL) {
        printf("Error: Could not open user data file '%s' for adding.\n", USER_DATA_FILE);
        return;
    }
    fwrite(&user, sizeof(UserProfile), 1, fp);
    fclose(fp);
}

/**
 * @brief Finds a user profile by ID.
 * @param userID The ID of the user to find.
 * @param user_out Pointer to a UserProfile struct to store the found data.
 * @return 1 if found, 0 otherwise.
 */
int findUserByID(const char *userID, UserProfile *user_out) { // Corrected function name
    FILE *fp = fopen(USER_DATA_FILE, "rb");
    if (fp == NULL) return 0; // File doesn't exist

    UserProfile current_user;
    int found = 0;
    while (fread(&current_user, sizeof(UserProfile), 1, fp) == 1) {
        if (strcmp(current_user.userID, userID) == 0) {
            *user_out = current_user; // Copy found user data
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found;
}

/**
 * @brief Updates an existing user profile in the file.
 * @param user UserProfile struct containing the updated user's details.
 * @return 1 if updated, 0 if user not found.
 */
int updateUser(UserProfile user) {
    FILE *fp = fopen(USER_DATA_FILE, "rb+"); // Open in read/write binary mode
    if (fp == NULL) {
        printf("Error: Could not open user data file '%s' for updating.\n", USER_DATA_FILE);
        return 0;
    }

    UserProfile current_user;
    int flag = 0;
    long currentPos;

    while (fread(&current_user, sizeof(UserProfile), 1, fp) == 1) {
        if (strcmp(current_user.userID, user.userID) == 0) {
            currentPos = ftell(fp); // Get current position (after reading current_user)
            fseek(fp, currentPos - sizeof(UserProfile), SEEK_SET); // Go back to start of record
            fwrite(&user, sizeof(UserProfile), 1, fp); // Overwrite with updated data
            flag = 1;
            break; // Found and updated, exit loop
        }
    }
    fclose(fp);
    return flag;
}

/**
 * @brief Deletes a user profile from the file.
 * Creates a temporary file, copies all records except the one to delete,
 * then replaces the original file with the temporary one.
 * @param userID The ID of the user to delete.
 * @return 1 if deleted, 0 if user not found.
 */
int deleteUser(const char *userID) {
    FILE *fp = fopen(USER_DATA_FILE, "rb");
    FILE *ft = fopen(USER_TEMP_FILE, "wb");
    if (fp == NULL || ft == NULL) {
        if (fp) fclose(fp);
        if (ft) fclose(ft);
        printf("Error: Could not open user data files for deletion.\n");
        return 0;
    }

    UserProfile current_user;
    int flag = 0;
    while (fread(&current_user, sizeof(UserProfile), 1, fp) == 1) {
        if (strcmp(userID, current_user.userID) != 0) {
            fwrite(&current_user, sizeof(UserProfile), 1, ft); // Write to temp file if not the one to delete
        } else {
            flag = 1; // User found and skipped (effectively deleted)
        }
    }

    fclose(ft);
    fclose(fp);

    remove(USER_DATA_FILE); // Delete original file
    rename(USER_TEMP_FILE, USER_DATA_FILE); // Rename temp file to original

    return flag;
}

/**
 * @brief Reads all user profiles from the file into an array.
 * @param users_array Array to store user profiles.
 * @param max_sz Maximum size of the array.
 * @return Number of users read.
 */
int viewAllUsers(UserProfile users_array[], int max_sz) {
    FILE *fp = fopen(USER_DATA_FILE, "rb");
    if (fp == NULL) return 0; // File doesn't exist

    UserProfile current_user;
    int i = 0;
    while (fread(&current_user, sizeof(UserProfile), 1, fp) == 1 && i < max_sz) {
        users_array[i] = current_user;
        i++;
    }
    fclose(fp);
    return i;
}

/**
 * @brief Generates a unique User ID based on the first name.
 * @param firstName The first name of the user.
 * @param newUserIDBuffer Buffer to store the generated ID.
 * @param bufferSize Size of the buffer.
 */
void generateUniqueUserID(const char *firstName, char *newUserIDBuffer, size_t bufferSize) { // Corrected function name and arguments
    UserProfile users[READ_ARRAY_SZ * 5]; // Max users to check
    int count = viewAllUsers(users, READ_ARRAY_SZ * 5);
    int maxIDNum = 0;

    // Convert first name to lowercase for consistent ID generation
    char lowerFirstName[MAX_NAME];
    for (int i = 0; firstName[i] && i < MAX_NAME - 1; i++) {
        lowerFirstName[i] = tolower(firstName[i]);
    }
    lowerFirstName[strlen(firstName)] = '\0';


    // Count how many users have the same first name prefix
    for (int i = 0; i < count; i++) {
        // Check if the stored userID starts with the lowercased first name
        // and then try to parse the number part
        if (strncmp(users[i].userID, lowerFirstName, strlen(lowerFirstName)) == 0) {
            // Get the numeric part of the ID
            const char *numPart = users[i].userID + strlen(lowerFirstName);
            if (strlen(numPart) > 0 && isdigit(numPart[0])) { // Ensure there's a number part
                int idNum = stringToInt((char*)numPart); // Convert "001" from "john001" to 1
                if (idNum > maxIDNum) {
                    maxIDNum = idNum;
                }
            }
        }
    }
    // Format the new ID: lowercase first name + incremented number (e.g., john001, john002)
    snprintf(newUserIDBuffer, bufferSize, "%s%03d", lowerFirstName, maxIDNum + 1);
}
