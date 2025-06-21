#include "copyfile.h"

void CopyFile(const char* input_filename, const char* output_filename) {
    // Open input file
    FILE* file = fopen(input_filename, "rb");
    if (file == NULL) {
        Log("Error opening input file");
        return;
    }

    // Get size
    if (fseek(file, 0, SEEK_END) != 0) {
        Log("Error seeking input file");
        fclose(file);
        return;
    }

    long length = ftell(file);
    if (length < 0) {
        Log("Error telling input file size");
        fclose(file);
        return;
    }

    rewind(file);

    // Allocate buffer
    char* buffer = (char*)malloc((length + 1) * sizeof(char));
    if (buffer == NULL) {
        Log("Memory allocation failed");
        fclose(file);
        return;
    }

    // Read file content
    size_t read_length = fread(buffer, sizeof(char), length, file);
    if (read_length != (size_t)length) {
        Log("Reading input file incomplete");
        free(buffer);
        fclose(file);
        return;
    }
    buffer[read_length] = '\0';

    fclose(file);

    // Open output file
    FILE* out_file = fopen(output_filename, "w");
    if (out_file == NULL) {
        Log("Error opening output file");
        free(buffer);
        return;
    }

    // Write to output file
    if (fprintf(out_file, "%s", buffer) < 0) {
        Log("Error writing to output file");
    }

    fclose(out_file);
    free(buffer);
}